
#ifndef __AUDIO_H__
#define __AUDIO_H__

// clase para manejar la entrada/salida de audio.
class audio
{
 public:

 int  dsp;
 int  freq;

 audio(int canales, int frecuencia, int formato);
 ~audio();

 void lee(void* buffer, int tamanho);
 void escribe(void* buffer, int tamanho);
};

#endif
