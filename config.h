
#ifndef __CONFIG_H__
#define __CONFIG_H__

// archivo de configuración del loader para SPECTRUM.

#define TIPO_MUESTRA          int16_t
#define FORMATO_MUESTRA       AFMT_S16_LE

#define FRECUENCIA_SAMPLING   44100

/* una muestra se considera un máximo/mínimo si es mayor/menor que un de-
   terminado número de muestras a ambos lados. */
#define PROF_MAX_MIN          5

// tolerancia en frecuencia para encontrar el leader.
#define  TOL1            0.2

// tolerancia en frecuencia para encontrar el sync pulse.
#define  TOL2            0.3

// tolerancia en frecuencia para coger los bits.
#define  TOL3            0.6

// mínimo número de muestras que debe tener el leader.
#define  TAM_LEADER_MIN  10000

#define MAX_ORDEN_FILTRO 150

//#define  LEE_WAV
#endif
