//-*- C++ -*-

#include <memory.h>
#include "filter.h"

#define max(a,b) (((a)<(b))?(b):(a))

Filter::Filter()
{
  N = 0;
  a = b = s = NULL;
}

// given each polynomial order and coefs, with optional initial status.
Filter::Filter(unsigned int Na, unsigned int Nb, FLT* a, FLT* b, FLT* s)
{
  Filter::N = max(Na, Nb);

  Filter::a = new FLT[N + 1];
  Filter::b = new FLT[N + 1];
  Filter::s = new FLT[N + 1];

  for (unsigned int i = 0; i < N + 1; i++)
    Filter::a[i] = Filter::b[i] = Filter::s[i] = 0.0;

  memcpy(Filter::a, a, (Na + 1)*sizeof(FLT));
  memcpy(Filter::b, b, (Nb + 1)*sizeof(FLT));
  if (s) memcpy(Filter::s, s, (N + 1)*sizeof(FLT));

  for (unsigned int i = 0; i < N + 1; i++) {
    Filter::a[i] /= a[0]; // polynomial normalization.
    Filter::b[i] /= a[0];
  }

}

// allows to change coefs of filter, maintaining the polynomial order.
void Filter::update(unsigned int Na, unsigned int Nb, FLT* a, FLT* b)
{
  memcpy(Filter::a, a, (Na + 1)*sizeof(FLT));
  memcpy(Filter::b, b, (Nb + 1)*sizeof(FLT));

  for (unsigned int i = 0; i < N + 1; i++) {
    Filter::a[i] /= a[0]; // normalization.
    Filter::b[i] /= a[0];
  }
}

// Digital Filter Implementation II, in & out overlapables.
void Filter::filter(unsigned int n, FLT* in, FLT* out)
{
  FLT                    w, y;
  register unsigned int  i;
  register int           j;
  
  for(i = 0; i < n; i++){
    
    w = in[i];
    y = 0.0;
    
    for (j = N - 1; j >= 0; j--) {
      w -= a[j + 1]*s[j];
      y += b[j + 1]*s[j];
      s[j + 1] = s[j];
    }
    
    y += w*b[0];
    s[0] = w;
    
    out[i] = y;
  }
}

FLT Filter::filter(FLT in) // single sample filtering
{
  FLT result;

  filter(1, &in, &result);
  return result;
}

Filter::~Filter()
{
  delete [] a;
  delete [] b;
  delete [] s;
}
