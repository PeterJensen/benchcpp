// Author: Peter Jensen

#ifndef _AVERAGEFLOAT32X4_H
#define _AVERAGEFLOAT32X4_H

#include <stdio.h>
#include <stdint.h>
#include <xmmintrin.h>
#include <math.h>
#include "base.h"

class AverageFloat32x4 : public Base::Benchmark {
 public:
  AverageFloat32x4() :
    Base::Benchmark(
      new Base::Configuration(
        string("AverageFloat32x4"),
        initArray,
        cleanup,
        simdAverage,
        average,
        1000)) {}

  static const uint32_t length = 10000;
  static float a[length];

  static bool sanityCheck() {
    return fabs(simdAverageKernel() - nonSimdAverageKernel()) < 0.0001;
  }

  static bool initArray() {
    for (uint32_t i = 0; i < length; ++i) {
      a[i] = 0.1f;
    }
    // Check that the two kernel functions yields the same result.
    return sanityCheck();
  }

  static bool cleanup() {
    return sanityCheck();
  };

  static float simdAverageKernel() {
    __m128 sumx4 = _mm_set_ps1(0.0);
    for (uint32_t j = 0, l = length; j < l; j = j + 4) {
      sumx4 = _mm_add_ps(sumx4, _mm_loadu_ps(&(a[j])));
    }
    return (sumx4.m128_f32[0] + sumx4.m128_f32[1] + sumx4.m128_f32[2] + sumx4.m128_f32[3])/length;
  }

  static float nonSimdAverageKernel() {
    float sum = 0.0;
    #pragma loop(no_vector)
    #pragma nounroll
    for (uint32_t j = 0, l = length; j < l; ++j) {
      sum += a[j];
    }
    return sum/length;
  }

  static uint64_t simdAverage(uint64_t n) {
    float val;
    for (uint64_t i = 0; i < n; ++i) {
      val = simdAverageKernel();
    }
    return (uint64_t)val;
  };

  static uint64_t average(uint64_t n) {
    float val;
    for (uint64_t i = 0; i < n; ++i) {
      val = nonSimdAverageKernel();
    }
    return (uint64_t)val;
  };

};

float AverageFloat32x4::a[AverageFloat32x4::length];

#endif
