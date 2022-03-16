#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>

clock_t clock_begin, clock_end;
struct tms start, end;;

int main(int argc, char* argv[]) {
  int i = 1;
  int j;
  int counter = 0;
  char** array;
  char create_table_name[] = "create_table: ";
  char wc_files_name[] = "wc_files: ";
  char remove_block_name[] = "remove_block: ";
  int (*ptr_count)(char**, int);
  void (*ptr_create_table)(int);
  void (*ptr_remove_block)(int);
  void (*ptr_free_memory)();


  #ifdef LIB_DYNAMIC
    void* handle = dlopen ("../zad1/libzad1shared.so", RTLD_LAZY);
    if (handle == NULL) {
      printf("Cannot load library\n");
      exit(1);
    }
    ptr_create_table = dlsym(handle, "create_table");
    ptr_count = dlsym(handle, "count");
    ptr_remove_block = dlsym(handle, "remove_block");
    ptr_free_memory = dlsym(handle, "free_memory");

  #endif

  while (i < argc) {
    // printf("%s\n", argv[i]);
    if (strcmp(argv[i], "create_table") == 0) {
      //funckja is number
      if (is_number(argv[i+1]) == 0) {
        fprintf(stderr, "Argument of create_table must be a number");
        exit(1);
      }
      int size = atoi(argv[i+1]);
      start_time();
      #ifdef LIB_DYNAMIC
        printf("test\n");
        (*ptr_create_table)(size);
      #else
        create_table(size);
      #endif
      stop_time();
      time_result(create_table_name);
      i += 2;
    }

    else if (strcmp(argv[i], "wc_files") == 0) {
      // sprawdzic czy koniec
      j = i;
      counter = 0;
      while (j < argc) {
        j++;
        if (j < argc && strcmp(argv[j], "wc_files") != 0 && strcmp(argv[j], "create_table") != 0 && strcmp(argv[j], "remove_block") != 0) {
            counter += 1;
        }
        else break;
      }
      if (counter == 0) exit(1);
      array = calloc(counter, sizeof(char*));
      j = i;
      for (int k = 0; k < counter; k++) {
        j++;
        array[k] = argv[j];
      }

      start_time();
      #ifdef LIB_DYNAMIC
        int res = (*ptr_count)(array, counter);
      #else
        int res = count(array, counter);
      #endif
      stop_time();
      time_result(wc_files_name);

      for (int k = 0; i < counter; k++) {
        free(array[k]);
      }
      free(array);

      i += counter + 1;
    }

    else if (strcmp(argv[i], "remove_block") == 0) {
      if (is_number(argv[i+1]) == 0) {
        fprintf(stderr, "Argument of remove_block must be a number");
        exit(1);
      }
      int index = atoi(argv[i+1]);
      start_time();
      #ifdef LIB_DYNAMIC
        (*ptr_remove_block)(index);
      #else
        remove_block(index);
      #endif
      stop_time();
      time_result(remove_block_name);
      i += 2;
    }
  }
  if (i < argc) fprintf(stderr, "Wrong Argument\n");
  #ifdef LIB_DYNAMIC
    (*ptr_free_memory)();
    dlclose (handle);
  #else
    free_memory();
  #endif
  return 0;
}

int is_number(char* word) {
  size_t size = strlen(word);
  // if (word[0] == '0' && size > 1) return 0;
  for (int i = 0; i < size; i++) {
    if (!isdigit(word[i])) {
      return 0;
    }
  }
  return 1;
}

void  start_time() {
  clock_begin = times(&start);
}

void stop_time() {
  clock_end = times(&end);
}

void time_result(char procces_name[]) {
  int64_t clock_tck = sysconf(_SC_CLK_TCK);

  double real_time = (double)(clock_end - clock_begin) / clock_tck;

  double user_main = (double)(end.tms_utime - start.tms_utime) / clock_tck;
  double user_child = (double)(end.tms_cutime - start.tms_cutime) / clock_tck;
  double user_time = user_main + user_child;

  double sys_main = (double)(end.tms_stime - start.tms_stime) / clock_tck;
  double sys_child = (double)(end.tms_cstime - start.tms_cstime) / clock_tck;
  double sys_time = sys_main + sys_child;

  char buffer[255];
  snprintf(buffer, sizeof(buffer), "Real time: %.6fs, User Time: %.6fs, System time: %.6fs\n",
  real_time, user_time, sys_time);
  printf("%s %s\n", procces_name, buffer);

  // FILE* file = fopen("raport2.txt", "a");
  // fputs(procces_name, file);
  // fputs(buffer, file);
  // fclose(file);
}
