
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <math.h>
#include <sys/soundcard.h>

#include "audio.h"
#include "config.h"

/*
  Cargador de cintas de SPECTRUM.

  01-08-2001 Ibán Cereijo Graña.
*/

bool fin = false;

void ControlC_Callback(int);
void analizar(audio *, int);

//--------------------------------------------------------------------

int main() {

  audio *A = new audio(1, FRECUENCIA_SAMPLING, FORMATO_MUESTRA);

  // para controlar el CTRL-C.
  signal(SIGINT, ControlC_Callback);

  analizar(A, 0);
  analizar(A, 255);

  delete A;
  return 0;
}

//--------------------------------------------------------------------

void ControlC_Callback(int h)
{
  fin = true;
}

// coge una muestra, indica si es un máximo o un mínimo.
int cogemuestra(audio *A, TIPO_MUESTRA &muestra, bool &maximo, bool &minimo)
{
  static TIPO_MUESTRA cola[2*PROF_MAX_MIN + 1];

  //  if (cursor_buffer >= tam_buffer) return 0;

  // desplazo la cola circular.
  memcpy(cola, &cola[1], 2*PROF_MAX_MIN*sizeof(TIPO_MUESTRA));

  // añado la nueva muestra al final de la cola.
  A->lee(&cola[2*PROF_MAX_MIN], 1*sizeof(TIPO_MUESTRA));
  A->escribe(&cola[2*PROF_MAX_MIN], 1*sizeof(TIPO_MUESTRA));

  // la muestra del medio es la que devolvemos.
  muestra = cola[PROF_MAX_MIN];

  maximo = minimo = true;
  for (int j = 0; j <= 2*PROF_MAX_MIN; j++) {
    if (muestra < cola[j]) maximo = false;
    if (muestra > cola[j]) minimo = false;
  }  
  
  return 1;
}

//--------------------------------------------------------------------

/* analiza el buffer obteniendo muestras secuencialmente. */
void analizar(audio *A, int marker)
{
  int            i, j;
  static int     cursor_buffer = -PROF_MAX_MIN;
  bool           maximo, minimo;

  int            max, min; // índices de los últimos máximos y mínimos.
  
  double         delta_min, delta_max;
  TIPO_MUESTRA   muestra;

  double         acumulador, tol;
  double         freq_leader, freq_bit1;
  double         umbral_bit;

  static unsigned char output[65536];
  //  static int           cursor_output = 0;

  unsigned char  byte = 0;

  static int     tam;

  // una cabecera ocupa 17 bytes.
  if (marker == 0) tam = 17;

  while (!fin) {

    if (marker == 0) printf("buscando LEADER(1)...\n");
    else printf("buscando LEADER(2)...\n");
    
    /* primero buscaremos el LEADER. Exigiremos un tono constante en frecuencia
       (próxima a 807.2 Hz) de al menos TAM_LEADER_MIN muestras de duracion. */
    int leader = false;
    
    max =  0;
    min = -1;
  
    delta_min = delta_max = 1.0;

    while (!leader && !fin) {

      acumulador = 0.0;
      for (i = 0; i < TAM_LEADER_MIN; i++) {
      
	cursor_buffer++;
	if (!cogemuestra(A, muestra, maximo, minimo)) {
	  printf("no encuentro el LEADER.\n");
	  return;
	}

	acumulador += delta_max;
      
	if (maximo) {

	  if (cursor_buffer - max < 20) break;
	  tol = fabs(1.0 - (cursor_buffer - max)/delta_max);
	  delta_max = cursor_buffer - max;
	  max = cursor_buffer;
	  if (tol > TOL1) break;
	}      
      }
    
      if (i == TAM_LEADER_MIN) {
	freq_leader = FRECUENCIA_SAMPLING*TAM_LEADER_MIN/acumulador;
	printf("LEADER encontrado!, muestra %i, frecuencia = %f\n", cursor_buffer, freq_leader);
	leader = true;
      }
    }

    // ya he encontrado el LEADER, ahora viene el SYNC PULSE.

    for (i = 0; !fin; i++) {
      
      cursor_buffer++;
      if (!cogemuestra(A, muestra, maximo, minimo)) {
	printf("no encuentro el SYNC PULSE.\n");
	return;
      }

      if ((maximo) && (cursor_buffer - max > 5)) {

	tol = fabs(1.0 - (cursor_buffer - max)/delta_max);
	delta_max = cursor_buffer - max;
	max = cursor_buffer;
	if (tol > TOL2) break;
      }      
    }

    if (fin) break;
    /*    if (marker == 0) sync = cursor_buffer;
	  else sync2 = cursor_buffer;*/
    printf("SYNC PULSE encontrado en la muestra %i, TOL = %f\n", cursor_buffer, tol);

    freq_bit1 = 1023.4*(807.2/freq_leader);
    umbral_bit = FRECUENCIA_SAMPLING/(0.5*(freq_bit1 + 2.0*freq_bit1));

    unsigned char  checksum = 0;

    j = 0;
    for (i = 0; (j < ((tam + 2) << 3)) && !fin; i++) {
      
      cursor_buffer++;
      if (!cogemuestra(A, muestra, maximo, minimo)) {
	printf("no encuentro el bloque de datos.\n");
	return;
      }

      if ((maximo) && (cursor_buffer - max > 5)) {

	delta_max = cursor_buffer - max;

	byte <<= 1;

	if (delta_max > umbral_bit) {
	  byte++; // le añado un bit en la parte baja.
	  tol = fabs(1.0 - delta_max*freq_bit1/FRECUENCIA_SAMPLING);
	} else tol = fabs(1.0 - delta_max*2.0*freq_bit1/FRECUENCIA_SAMPLING);

	// no se puede considerar ni un 1 ni un 0.
	if (tol > TOL3) {
	  printf("error de bit (tol = %f)\n", tol);
	  break;
	}

	j++;
	if ((j & 7) == 0) {
	  if (marker == 0) output[2 + ((j - 1) >> 3)] = byte;
	  else output[23 + ((j - 1) >> 3)] = byte;
	  checksum ^= byte;
	  byte = 0;
	}

	max = cursor_buffer;
      }      
    }

    // el bucle ha sido interrumpido.
    if (j != ((tam + 2) << 3)) continue;

    if (marker == 0) { // vamos a visualizar unos datos.

      if (output[2] != marker) {
	printf("el marker no corresponde.\n");
	continue;
      }

      int ubicacion = output[16] + (output[17] << 8);
      tam = output[14] + (output[15] << 8);
      
      printf("Tipo: ");
      switch (output[3]) {
	
      case 0: 
	printf("Programa ");
	if (ubicacion > 9999)
	    printf("(sin autoejecución)\n");
	    else printf("(autoejecución en línea %u)\n", ubicacion);
	    break;
      case 1: printf("Matriz Numérica\n"); break;
      case 2: printf("Matriz Alfanumérica\n"); break;
      case 3: 
	if ((tam == 6912) && (ubicacion == 16384))
	  printf("SCREEN$ (pantalla)\n");
	else printf("Bytes (código máquina, comienzo en %u)\n", ubicacion);
      break;
      }
      
      output[0] = 19; // lo que ocupa la cabecera.
      output[1] = 0;

      printf("Nombre: ");
      for (i = 4; i < 14; i++) printf("%c", output[i]);
      printf("\nLongitud: %u bytes\n", tam);
    } else {
      output[21] = tam + 2;
      output[22] = (tam + 2) >> 8;
    }

    // comprobamos el checksum.
    if (checksum == 0) {
      printf("CHECKSUM ok!\n");
      break; // todo bien.
    }
    else {
      printf("error de CHECKSUM!\n");
      continue;
    }
  }

  if (fin) return;

  if (marker == 255) {

    printf("escribiendo archivo output.tap\n");
    FILE *f;
    
    if ((f = fopen("output.tap", "wb")) == NULL) {
      perror("Imposible abrir el fichero ");
      exit(1);
    }

    fwrite(output, 25 + tam, 1, f);
    
    fclose(f);
  }
}
