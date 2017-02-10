ZX Tape Loader
==============

This program uses implements a PSK demodulator, intended to digitalize old Spectrum tapes by
loading .wav files and converting them to .tzx files.


Compilation
===========

```
make clean
make
```

Usage
=====

Read from the audio input device (OSS) in real time:

```
loader out_file.tzx
```

or from a WAV file in batch mode:

```
loader out_file.tzx [in_file.wav]
```

Example:

```
> ./loader output.tzx Manic_Miner.wav

WAV sample rate: 44100
Decoding... (press CTRL-C to abort)
1: Found carrier


 2692: found PILOT f=1632.57 Hz
 108862: found SYNC1 f=3477.45 Hz
 108866: found SYNC2 f=6122.26 Hz
 getting data
109086: max_tol = 0.479337
109152: max_tol = 0.482011
109185: max_tol = 0.484447
109537: max_tol = 0.485421
109746: max_tol = 0.485626
 111175: warning: bit error, assuming end, tol=-2.89

 19 bytes read (152 bits) , f0=3859.22 Hz, f1=2022.03 Hz, checksum=0 ok
111188: Found carrier
111395: Lost Carrier
133195: Found carrier


     Name : ManicMiner
     Type : Program (autoexecutable at line 10)
     Size : 69 bytes



 135893: found PILOT f=1633.98 Hz
 176656: found SYNC1 f=3477.45 Hz
 176660: found SYNC2 f=6122.26 Hz
 getting data
176847: max_tol = 0.487975
 184909: warning: bit error, assuming end, tol=-2.91

 71 bytes read (568 bits) , f0=3956.49 Hz, f1=2017.47 Hz, checksum=FF fail!
185100: Lost Carrier
206929: Found carrier


 209627: found PILOT f=1633.98 Hz
 315784: found SYNC1 f=3477.45 Hz
 315788: found SYNC2 f=6122.26 Hz
 getting data
 317874: warning: bit error, assuming end, tol=-0.87

 19 bytes read (152 bits) , f0=3984.58 Hz, f1=1976.89 Hz, checksum=0 ok
317886: Found carrier
318097: Lost Carrier
339897: Found carrier


     Name : mmm
     Type : Machine code bytes (starting in 22784)
     Size : 256 bytes



 342595: found PILOT f=1633.98 Hz
 383358: found SYNC1 f=3477.45 Hz
 383362: found SYNC2 f=6122.26 Hz
 getting data
 414052: warning: bit error, assuming end, tol=-3.01

 258 bytes read (2064 bits) , f0=3971.61 Hz, f1=2018.04 Hz, checksum=0 ok
414243: Lost Carrier
436071: Found carrier


 438769: found PILOT f=1633.98 Hz
 544926: found SYNC1 f=3477.45 Hz
 544930: found SYNC2 f=6122.26 Hz
 getting data
 546952: warning: bit error, assuming end, tol=-0.92

 19 bytes read (152 bits) , f0=3986.73 Hz, f1=1967.02 Hz, checksum=0 ok
546965: Found carrier
547171: Lost Carrier
568973: Found carrier


     Name : mm1
     Type : Machine code bytes (starting in 32768)
     Size : 32768 bytes



 571671: found PILOT f=1633.98 Hz
 612434: found SYNC1 f=3477.45 Hz
 612438: found SYNC2 f=6122.26 Hz
 getting data
612625: max_tol = 0.487975
612724: max_tol = 0.488425
  1k read
  2k read
  3k read
  4k read
  5k read
  6k read
  7k read
  8k read
  9k read
  10k read
  11k read
  12k read
  13k read
  14k read
  15k read
  16k read
  17k read
  18k read
  19k read
  20k read
  21k read
  22k read
  23k read
  24k read
  25k read
  26k read
  27k read
  28k read
  29k read
  30k read
  31k read
  32k read
 4250975: warning: bit error, assuming end, tol=-3.11

 32770 bytes read (262160 bits) , f0=3985.85 Hz, f1=2021.12 Hz, checksum=0 ok
4251165: Lost Carrier
END of file
4295029 samples read
```

