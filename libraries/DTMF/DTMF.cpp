#include "Arduino.h"
#include "DTMF.h"

float SAMPLING_RATE;
float TARGET;
int N;
float coeff[8];
float Q1[8];
float Q2[8];
float cosine;
int testData[160];

const int dtmf_tones[8] = {
  697,
  770,
  852,
  941,
 1209,
 1336,
 1477,
 1633
};

const unsigned char dtmf_map[16] = {
  0x11,
  0x21,
  0x41,
  0x12,
  0x22,
  0x42,
  0x14,
  0x24,
  0x44,
  0x28,
  0x81,
  0x82,
  0x84,
  0x88,
  0x18,
  0x48
};

const char dtmf_char[16] = {
  '1',
  '2',
  '3',
  '4',
  '5',
  '6',
  '7',
  '8',
  '9',
  '0',
  'A',
  'B',
  'C',
  'D',
  '*',
  '#',
};

DTMF::DTMF(float BLOCK)
{
#if F_CPU == 16000000L
DTMF(BLOCK, 8928.0);
#else
DTMF(BLOCK, 4400.0);
#endif
}

DTMF::DTMF(float BLOCK,float SAMPLING_FREQ)
{
  DDRD |= B00010000;
  PORTD &= B11101111;
    
  SAMPLING_RATE=SAMPLING_FREQ;
  N=BLOCK;

  float	omega;
  // Calculate the coefficient for each DTMF tone
  for(int i = 0;i < 8;i++) {
    omega = (2.0 * PI * dtmf_tones[i]) / SAMPLING_RATE;
// DTMF detection doesn't need the phase.
// Computation of the magnitudes (which DTMF does need) does not
// require the value of the sin.
// not needed    sine = sin(omega);
    coeff[i] = 2.0 * cos(omega);
  }
  ResetDTMF();
}


/* Call this routine before every "block" (size=N) of samples. */
void DTMF::ResetDTMF(void)
{
  for(int i=0; i<8 ; i++) {
    Q2[i] = 0;
    Q1[i] = 0;
  }
}


/* Call this routine for every sample. */
void DTMF::ProcessSample(int sample,int adc_centre)
{
  float Q0;
  for(int i=0;i < 8;i++) {
    Q0 = coeff[i] * Q1[i] - Q2[i] + (float) (sample - adc_centre);
    Q2[i] = Q1[i];
    Q1[i] = Q0;
  }
}

/* Sample some test data. */
void DTMF::sample(int sensorPin)
{
const unsigned char f_counter = 0x10;

  for (int index = 0; index < N; index++)
  {
    testData[index] = analogRead(sensorPin);
    PORTD ^= f_counter;    
  }
}

// return the magnitudes of the 8 DTMF frequencies
void DTMF::detect(float dtmf_mag[],int adc_centre)
{
  int index;
  float d_tmp;

  /* Process the samples. */
  for (index = 0; index < N; index++)
  {
    ProcessSample(testData[index],adc_centre);
  }
  // Calculate the magnitude of each tone.
  for(int i=0;i < 8;i++) {
  	// This is the equivalent of sqrt(real*real + imag*imag)
  	d_tmp = Q1[i]*Q1[i];
  	d_tmp += Q2[i]*Q2[i];
  	d_tmp -= coeff[i]*Q1[i]*Q2[i];

    dtmf_mag[i] = sqrt(d_tmp);
  }
  ResetDTMF();
}


char last_dtmf = 0;
char DTMF::button(float mags[],float magnitude)
{
  int bit = 1;
  int j;
  int dtmf = 0;



  for(int i=0;i<8;i++) {
    if(mags[i] > magnitude) {
      dtmf |= bit;
    }
    bit <<= 1;
  }
  for(j=0;j<16;j++) {
    if(dtmf_map[j] == dtmf)break;
  }
  if(j < 16) {
    if(dtmf_char[j] == last_dtmf)return((char) 0);
    last_dtmf = dtmf_char[j];
    return(dtmf_char[j]);
  }
  last_dtmf = 0;
  return((char) 0);
}
