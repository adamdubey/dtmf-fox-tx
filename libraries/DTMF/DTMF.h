/*
Decode DTMF tones.

The Goertzel algorithm is long standing so see http://en.wikipedia.org/wiki/Goertzel_algorithm 
for a full description. It is often used in DTMF tone detection as an alternative to the 
Fast Fourier Transform because it is quick with low overheard because it is only searching for a single 
frequency rather than showing the occurrence of all frequencies.
*/

#ifndef DTMF_h
#define DTMF_h

#include "Arduino.h"

// library interface description
class DTMF
{
  // user-accessible "public" interface
  public:
    DTMF(float,float);
    DTMF(float);
void sample(int);
void detect(float dtmf_mag[],int adc_centre);
char button(float mags[],float magnitude);
  // library-accessible "private" interface
  private:

void ProcessSample(int,int);
void ResetDTMF(void);

};

#endif

