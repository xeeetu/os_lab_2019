#ifndef SUM_ARR_H
#define SUM_ARR_H

struct SumArgs {
  int *array;
  int begin;
  int end;
};

int Sum(const struct SumArgs *args);

void *ThreadSum(void *args);

#endif