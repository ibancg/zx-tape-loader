
#ifndef __CONFIG_H__
#define __CONFIG_H__

// archivo de configuración del loader para SPECTRUM.

#define TIPO_MUESTRA          int16_t
#define FORMATO_MUESTRA       AFMT_S16_LE

#define FRECUENCIA_SAMPLING   44100

#define PROF_MAX_MIN          5

// tolerancia para encontrar el leader.
#define  TOL1            0.2

// tolerancia para encontrar el sync pulse.
#define  TOL2            0.4

// tolerancia para coger los bits.
#define  TOL3            2.0

// mínimo número de muestras que debe tener el leader.
#define  TAM_LEADER_MIN  10000

#endif
