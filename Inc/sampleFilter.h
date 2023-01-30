#ifndef SAMPLEFILTER_H_
#define SAMPLEFILTER_H_

/*

FIR filter designed with
 http://t-filter.appspot.com

sampling frequency: 22000 Hz

fixed point precision: 16 bits

* 0 Hz - 400 Hz
  gain = 1
  desired ripple = 5 dB
  actual ripple = n/a

* 500 Hz - 1000 Hz
  gain = 0
  desired attenuation = -40 dB
  actual attenuation = n/a

*/

#define SAMPLEFILTER_TAP_NUM 31

typedef struct {
  uint16_t history[SAMPLEFILTER_TAP_NUM];
  unsigned int last_index;
} sampleFilter;

void sampleFilter_init(sampleFilter* f);
void sampleFilter_put(sampleFilter* f, uint16_t input);
uint16_t sampleFilter_get(sampleFilter* f);

#endif