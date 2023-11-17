#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

pid_t *pid_array=NULL;
int pnum = -1;

void sig_handler(int signum) {
  for (int i=0; i<pnum; i++) kill(pid_array[i], SIGKILL);
}

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  bool with_files = false;
  int timeout = 0;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {"timeout", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            // your code here
            // error handling
            if (seed <= 0) {
              printf("seed is a positive number\n");
              return 1;
            }
            break;
          case 1:
            array_size = atoi(optarg);
            // your code here
            // error handling
            if (array_size <= 0) {
              printf("array_size is a positive number\n");
              return 1;
            }
            break;
          case 2:
            pnum = atoi(optarg);
            // your code here
            // error handling
            if (pnum <= 0) {
              printf("pnum is a positive number\n");
              return 1;
            }            
            break;
          case 3:
            with_files = true;
            break;
          case 4:
            timeout = atoi(optarg);
            if (timeout <= 0) {
              printf("timeout is a positive number\n");
              return 1;
            } else {
              signal(SIGALRM, sig_handler);
            }
            break;
          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;

      case '?':
        printf("Unknown argument");
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  if (array_size<pnum) {
    printf("Error: pnum > array_size\n");
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;
  int *pipefd = malloc(sizeof(int)*pnum*2);
  for (int i=0; i<pnum; i++) {
    pipe(&pipefd[i*2]);
  }
  if (timeout>0) {
    alarm(timeout);
  }
  pid_array=malloc(sizeof(pid_t)*pnum);
  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  for (int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    if (child_pid >= 0) {
      pid_array[i]=child_pid;
      // successful fork
      active_child_processes += 1;
      if (child_pid == 0) {
        // child process

        // parallel somehow
        struct MinMax min_max_child;
        if (i+1<pnum) {
          min_max_child = GetMinMax(array, i*(array_size/pnum), (i+1)*(array_size/pnum));
//          printf("i=%d, st_ind=%d, fn_ind=%d, max=%d, min=%d\n", i, i*(array_size/pnum), (i+1)*(array_size/pnum), min_max_child.max, min_max_child.min);
        } else {
          min_max_child = GetMinMax(array, i*(array_size/pnum), array_size);
//          printf("i=%d, st_ind=%d, fn_ind=%d, max=%d, min=%d\n", i, i*(array_size/pnum), array_size, min_max_child.max, min_max_child.min);
        }        
        if (with_files) {
          // use files here
          FILE *fp_child;
          char filename_child[20];
          snprintf(filename_child, 20, "child_proc_result%d", i);
          fp_child=fopen(filename_child, "w+b");
          fwrite(&min_max_child, sizeof(struct MinMax), 1, fp_child);
          fclose(fp_child);
        } else {
          // use pipe here
          close(pipefd[i*2]);
          write(pipefd[i*2+1], &min_max_child, sizeof(struct MinMax));
          close(pipefd[i*2+1]);
        }
        if (timeout>0) {
          printf("Child process %d kill signal waiting...\n", getpid());
          pause();
        }
        return 0;
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }
  pid_t pid;
  int stat;
  while (active_child_processes > 0) {
    // your code here
    if ((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
      printf("child %d terminated\n", pid);
      active_child_processes -= 1;
    }
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;
    struct MinMax min_max_parent;
    if (with_files) {
      // read from files
      FILE *fp_parent;
      char filename_parent[20];
      snprintf(filename_parent, 20, "child_proc_result%d", i);
      fp_parent=fopen(filename_parent, "r+b");
      fread(&min_max_parent, sizeof(struct MinMax), 1, fp_parent);
      fclose(fp_parent);
    } else {
      // read from pipes
      close(pipefd[i*2+1]);
      read(pipefd[i*2], &min_max_parent, sizeof(struct MinMax));
      close(pipefd[i*2]);
    }
    min=min_max_parent.min;
    max=min_max_parent.max;
    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);
  free(pipefd);
  free(pid_array);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}