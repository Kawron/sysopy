#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>
#include <pthread.h>

#define MAX_LINE_LEN 256

int width;
int height;
int num_of_threads;
int** image;
int** negative;

void copy_image() {
    negative = calloc(height, sizeof(int*));
    for (int i = 0; i < height; i++) {
        negative[i] = calloc(width, sizeof(int));
    }
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            negative[i][j] = image[i][j];
        }
    }
}

void load_image(char* filename) {
    FILE* f = fopen(filename, "r");
    if (f == NULL) {
        fprintf(stderr, "Couldn't open a file\n");
        exit(1);
    }
    char buf[MAX_LINE_LEN];
    int flag = 1;
    char* splited;
    while (flag && fgets(buf, MAX_LINE_LEN, f)) {
        if (buf[0] == '#') {
            printf("LOAD_IMAGE: found comment\n");
            continue;
        }
        if (buf[0] == 'P') {
            printf("LOAD_IMAGE: got P2 or sth\n");
        }
        else {
            splited = strtok(buf, " ");
            width = atoi(splited);
            splited = strtok(NULL, " ");
            height = atoi(splited);
            printf("TEST: %d\n", width);
            printf("TEST: %d\n", height);
            fgets(buf, MAX_LINE_LEN, f);
            flag = 0;
        }
    }
    image = calloc(height, sizeof(int*));
    for (int i = 0; i < height; i++) {
        image[i] = calloc(width, sizeof(int));
    }
    int val;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fscanf(f, "%d", &val);
            image[i][j] = val;
        }
    }
}

void save_image(char* out_filename) {
    FILE* f = fopen(out_filename, "w");
    if (f == NULL) {
        fprintf(stderr, "Cannot open file to save image\n");
        exit(1);
    }
    fprintf(f, "P2\n");
    fprintf(f, "%d %d\n", width, height);
    fprintf(f, "255\n");
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fprintf(f, "%d ", negative[i][j]);
        }
        fprintf(f, "\n");
    }
    fclose(f);
}

void print_image() {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            printf("%d ", image[i][j]);
        }
        printf("\n");
    }
}

void numbers_negative(void* arg) {
    // left inclusive, right exclusive
    int idx = *((int*) arg);
    int lower_limit = 256 / num_of_threads * idx;
    int upper_limit;
    if (idx == num_of_threads-1) {
        upper_limit = 256;
    }
    else {
        upper_limit = 256 / num_of_threads * (idx+1);
    }

    int val;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            val = image[i][j];
            if (val >= lower_limit && val < upper_limit) {
                negative[i][j] = 255 - val;
            }
        }
    }
}

void block_negative(void* arg) {
    int idx = *((int*) arg);

    int x1 = idx * ceil(width/num_of_threads);
    int x2;
    if (idx == num_of_threads - 1) {
        x2 = width;
    }
    else {
        x2 = (idx+1) * ceil(width/num_of_threads);
    }
    int val;
    for (int i = 0; i < height; i++) {
        for (int j = x1; j < x2; j++) {
            val = image[i][j];
            negative[i][j] = 255 - val;
        }
    }
}

int main(int argc, char** argv) {

    if (argc < 5) {
        fprintf(stderr, "Program need 4 argumetns\n");
        exit(1);
    }

    num_of_threads = atoi(argv[1]);
    char* mode = argv[2];
    char* filename = atoi(argv[3]);
    char* out_filename = atoi(argv[4]);

    load_image(filename);
    copy_image();

    pthread_t* threads = calloc(num_of_threads, sizeof(pthread_t));
    int* idxs = calloc(num_of_threads, sizeof(int));

    for (int i = 0; i < num_of_threads; i++) {
        idxs[i] = i;
        if (strcmp(mode, "numbers") == 0 ) {
            pthread_create(&threads[i], NULL, &numbers_negative, &idxs[i]);
        }
        else if (strcmp(mode, "block") == 0) {
            pthread_create(&threads[i], NULL, &block_negative, &idxs[i]);
        }
        else {
            fprintf(stderr, "wrong mode\n");
            exit(1);
        }
    }
    save_image(out_filename);
}
