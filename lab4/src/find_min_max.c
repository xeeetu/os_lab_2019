#include "find_min_max.h"

#include <limits.h>

struct MinMax GetMinMax(int *array, unsigned int begin, unsigned int end) {
  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;
  // your code here
  for (int it=begin; it<end; it++) {
      if (array[it]>min_max.max) min_max.max=array[it];
      if (array[it]<min_max.min) min_max.min=array[it];
  }
  return min_max;
}