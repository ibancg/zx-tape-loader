
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <math.h>
#include <sys/soundcard.h>
#include <iostream.h>
#include <fstream.h>

#include "audio.h"
#include "config.h"

/*
  Cargador de cintas de SPECTRUM.

  06-08-2001 Ibán Cereijo Graña.
*/

bool fin = false;

void ControlC_Callback(int);

// coge una muestra, indica si es un flanco.
void cogemuestra(audio *A, TIPO_MUESTRA &muestra, bool &flanco);

//--------------------------------------------------------------------

double r[MAX_ORDEN_FILTRO];
double a[MAX_ORDEN_FILTRO];
double b[MAX_ORDEN_FILTRO];
int    ORDEN_FILTRO = 0;

void init() {
  
  int i;

  for (i = 0; i < MAX_ORDEN_FILTRO; i++) a[i] = b[i] = r[i] = 0.0;

  ifstream fib("filtro_b.txt");
  ifstream fia("filtro_a.txt");

  if (!fib) {
    perror("no encuentro coeficientes b del filtro.\n");
    exit(-1);
  }

  i = 0;
  while (fib) fib >> b[i++];
  
  ORDEN_FILTRO = i - 2;

  if (!fia) {
    a[0] = 1.0;
    return;
  }
  
  i = 0;
  while (fia) fia >> a[i++];

  if (i - 2 > ORDEN_FILTRO) ORDEN_FILTRO = i - 2;

  printf("orden del filtro: %d\n", ORDEN_FILTRO);
  cout << a[0] << " -- " << b[0] << endl;
}

int main() {

  //----------- variables ------------

  /* buffer donde almacenamos la entrada actual de la cinta y que escribi-
     remos al final del archivo si no hay error de carga. 128k de tamaño
     por si acaso. */
  unsigned char data[131072];

  /* archivo de salida (en formato TAP) en el que iremos incorporando las
     entradas capturadas. */
  FILE *output;

  // creamos una variable para controlar la E/S de audio.
  audio A(1, FRECUENCIA_SAMPLING, FORMATO_MUESTRA);

  TIPO_MUESTRA muestra; // muestra actual.

  int    i; // para bucles.

  int    cursor_buffer = -PROF_MAX_MIN; // cursor de la tarjeta.
  bool   leader = false; // indica si se ha encontrado el LEADER.

  int    cursor_flanco = 0; // índice del último máximo.  
  double delta_flanco = 1.0; // relación entre la separación del último y penúltimo máximos y la separación entre el penúltimo y el antepenúltimo.
  double delta_acum; // acumulador de distancias para calcular la frecuencia del LEADER.

  double max_tol;
  double tol;
  double freq_leader;
  double freq_bit1;
  double umbral_bit;

  bool   flanco;
  int    tam_entrada;

  //----------------------------------
  
  init();

  if ((output = fopen("output.tap", "wb")) == NULL) {
    perror("Imposible abrir el fichero de salida.");
    exit(1);
  }

  // para controlar el CTRL-C.
  signal(SIGINT, ControlC_Callback);

  // bucle infinito.
  while (!fin) {

    // bucle que intenta enganchar la entrada actual.
    while (!fin) {

      printf("buscando LEADER...\n");

      /* primero buscaremos el LEADER. Exigiremos un tono constante en frecuencia
	 (próxima a 807.2 Hz) de al menos TAM_LEADER_MIN muestras de duración. */
      leader        =  false;
      cursor_flanco =  0;
      delta_flanco  =  1.0;

      while (!leader && !fin) {

	delta_acum = 0.0;
	for (i = 0; (i < TAM_LEADER_MIN) && (!fin); i++) {
      
	  cursor_buffer++;
	  cogemuestra(&A, muestra, flanco);

	  delta_acum += delta_flanco;
      
	  if (flanco) {

	    if (cursor_buffer - cursor_flanco < 20) break;
	    tol = fabs(1.0 - (cursor_buffer - cursor_flanco)/delta_flanco);
	    delta_flanco = cursor_buffer - cursor_flanco;
	    cursor_flanco = cursor_buffer;
	    if (tol > TOL1) break;
	  }      
	}

	if (fin) break;
    
	if (i == TAM_LEADER_MIN) {
	  freq_leader = FRECUENCIA_SAMPLING*TAM_LEADER_MIN/delta_acum;
	  printf(" LEADER encontrado!, muestra %i, frecuencia = %f\n", cursor_buffer, freq_leader);
	  leader = true;
	}
      }

      if (fin) break;

      // ya he encontrado el LEADER, ahora viene el SYNC PULSE.
      for (i = 0; !fin; i++) {
      
	cursor_buffer++;
	cogemuestra(&A, muestra, flanco);

	if ((flanco) && (cursor_buffer - cursor_flanco > PROF_MAX_MIN)) {

	  tol = fabs(1.0 - (cursor_buffer - cursor_flanco)/delta_flanco);
	  delta_flanco = cursor_buffer - cursor_flanco;
	  cursor_flanco = cursor_buffer;
	  if (tol > TOL2) break;
	}      
      }

      if (fin) break;

      printf("  SYNC PULSE encontrado en la muestra %i, TOL = %f\n", cursor_buffer, tol);

      freq_bit1 = 1023.4*(807.2/freq_leader);
      umbral_bit = FRECUENCIA_SAMPLING/(0.5*(freq_bit1 + 2.0*freq_bit1));

      unsigned char checksum = 0;

      // cada 8 bits, tendre aquí un byte.
      unsigned char byte = 0;
      int           cont_bits = 0; // contador de bits.

      tam_entrada = 0;

      max_tol = 0.0;

      // ya he encontrado el SYNC PULSE, ahora recojo los bits.
      for (i = 0; !fin; i++) {
      
	cursor_buffer++;
	cogemuestra(&A, muestra, flanco);

	if ((flanco) && (cursor_buffer - cursor_flanco > PROF_MAX_MIN)) {

	  delta_flanco = cursor_buffer - cursor_flanco;

	  byte <<= 1; // desplazo el byte para dejar sitio al nuevo bit.

	  if (delta_flanco > umbral_bit) {
	    byte++; // es un 1, le añado un bit en la parte baja.
	    tol = fabs(1.0 - delta_flanco*freq_bit1/FRECUENCIA_SAMPLING);
	  } else
	    tol = fabs(1.0 - delta_flanco*2.0*freq_bit1/FRECUENCIA_SAMPLING);
	  
	  if (cont_bits == 0) tol = 0.0;
	  //	  printf("BIT nº %d = %d  (byte nº %d, tol = %f)\n", cont_bits, byte & 1, cont_bits >> 3, tol);
	  //if (((cont_bits + 1) & 7) == 0) printf("BYTE COMPLETO = %d\n", byte);

	  // no se puede considerar ni un 1 ni un 0.
	  if (tol > TOL3) {
	    printf("  error de bit (tol = %f)\n", tol);
	    break;
	  }

	  if (tol > max_tol) {
	    max_tol = tol;
	    printf(" superada máxima tolerancia: %f\n", tol);
	  }

	  cont_bits++;
	  if ((cont_bits & 7) == 0) {
	    data[2 + ((cont_bits - 1) >> 3)] = byte;
	    checksum ^= byte;
	    byte = 0;
	    tam_entrada++;
	    if ((tam_entrada % 1024) == 0) printf("  %i kbytes leídos.\n", tam_entrada/1000);
	  }

	  cursor_flanco = cursor_buffer;
	}      
      }

      if (fin) break;

      /* en esta parte del programa el bucle recogedor de bits sólo ha podido
	 abortar por error de bit. */

      if (tam_entrada == 0) continue;

      // comprobamos el checksum.
      if (checksum == 0) printf("  CHECKSUM ok! %d bytes leidos (%d bits)\n", cont_bits >> 3, cont_bits);
      else {
	printf("  error de CHECKSUM! (%0X) %d bytes leidos (%d bits)\n", checksum, cont_bits >> 3, cont_bits);
	return 1;
	continue;
      }

      // comprobamos el marker.
      if ((data[2] != 0) && (data[2] != 255)) {
	printf("  error: el marker no corresponde.\n");
	continue;
      }

      break;
    }

    if (fin) break;

    printf("  Tamaño de la entrada: %i\n", tam_entrada);

    data[0] = tam_entrada;
    data[1] = tam_entrada >> 8;
  
    if (data[2] == 0) { // es una cabecera, mostramos cierta información.

      int ubicacion = data[16] + (data[17] << 8);
      int tam = data[14] + (data[15] << 8);
    
      printf("     Tipo    : ");
      switch (data[3]) {
	
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

      printf("     Nombre  : ");
      for (i = 4; i < 14; i++) printf("%c", data[i]);
      printf("\n     Longitud: %u bytes\n", tam);
    }

    fwrite(data, 2 + tam_entrada, 1, output);
  }

  fclose(output);
  return 0;
}

//--------------------------------------------------------------------

void ControlC_Callback(int h)
{
  fin = true;
}

void Filtrar(TIPO_MUESTRA* in, TIPO_MUESTRA* out)
{
  static int L = 1;

  double w, y;
  int i, j;

  for(i = 0; i < L; i++){

    w = (double) in[i];
    y = 0.0;

    for (j = ORDEN_FILTRO - 1; j >= 0; j--) {
      w -= a[j + 1]*r[j];
      y += b[j + 1]*r[j];
      r[j + 1] = r[j];
    }

    y += w*b[0];
    r[0] = w;

    out[i] = (TIPO_MUESTRA) y;
  }  
}

// coge una muestra, indica si es un flanco.
void cogemuestra(audio *A, TIPO_MUESTRA &muestra, bool &flanco)
{
  static TIPO_MUESTRA muestra_old = 0;

  // cojo la muestra de la tarjeta.
  A->lee(&muestra, 1*sizeof(TIPO_MUESTRA));
  Filtrar(&muestra, &muestra);
  
  // y la saco por el altavoz.
  //  A->escribe(&muestra, 1*sizeof(TIPO_MUESTRA));

  // evitamos 2 flancos consecutivos (el algoritmo podría equivocarse si una muestra es exactamente 0).
  if (flanco) flanco = false;
  else {
    flanco = false;
    if ((muestra_old <= 0) && (muestra >= 0)) flanco = true; // flanco de subida.
  }

  muestra_old = muestra;
}
