//-*- C++ -*-

#ifndef __FILTER_H__
#define __FILTER_H__

#include <fcntl.h>
#include <stdlib.h>

#include "defs.h"

/*
  digital filtering implementation.
 */

class Filter {

 private:

 public:

  FLT*  a;
  FLT*  b;  // coefs
  FLT*  s;  // status

  unsigned int N;

  void filter(unsigned int n, FLT* in, FLT* out); // vector filtering
  FLT  filter(FLT in);                            // sample filtering

  Filter();
  Filter(unsigned int Na, unsigned int Nb, FLT* a, FLT* b, FLT* s = NULL);
  void update(unsigned int Na, unsigned int Nb, FLT* a, FLT* b);
  ~Filter();
};

#endif
