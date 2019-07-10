//-*- C++ -*-

#ifndef AUDIO_H
#define AUDIO_H

// class for audio input handling.
class Audio
{
private:

    int dsp;  // file handler.

public:

    // open audio dev
    Audio(char* fdsp, int* rate, int channels, int format, bool write);
    ~Audio();

    int read(void* buffer, int size);
    int write(void* buffer, int size);
};

#endif
