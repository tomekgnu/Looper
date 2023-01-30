#include "sampleFilter.h"

static uint16_t filter_taps[SAMPLEFILTER_TAP_NUM] = {
  -3268991577817963,
  -1385567241803360,
  -211482748761554,
  502174962082707,
  644245650074103,
  1974943759986952,
  3148377028085962,
  5669778136057553,
  5090759316529329,
  37569640290593,
  -6825553764527247,
  -11756011451797710,
  -13147047429094748,
  -4089510581656187,
  8757808103986937,
  29717016396781548,
  8757808103986937,
  -4089510581656187,
  -13147047429094748,
  -11756011451797710,
  -6825553764527247,
  37569640290593,
  5090759316529329,
  5669778136057553,
  3148377028085962,
  1974943759986952,
  644245650074103,
  502174962082707,
  -211482748761554,
  -1385567241803360,
  -3268991577817963
};

void sampleFilter_init(sampleFilter* f) {
  int i;
  for(i = 0; i < SAMPLEFILTER_TAP_NUM; ++i)
    f->history[i] = 0;
  f->last_index = 0;
}

void sampleFilter_put(sampleFilter* f, uint16_t input) {
  f->history[f->last_index++] = input;
  if(f->last_index == SAMPLEFILTER_TAP_NUM)
    f->last_index = 0;
}

uint16_t sampleFilter_get(sampleFilter* f) {
  uint32_t acc = 0;
  int index = f->last_index, i;
  for(i = 0; i < SAMPLEFILTER_TAP_NUM; ++i) {
    index = index != 0 ? index-1 : SAMPLEFILTER_TAP_NUM-1;
    acc += (uint32_t)f->history[index] * filter_taps[i];
  };
  return acc >> 16;
}