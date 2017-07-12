#include "srm.h"
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <ctime>

using namespace std::chrono;

void print_int(const int* img, int w, int h) {
    for (int j=h-1; j >= 0; j--) {
        printf("\n");
        for (int i = w - 1; i >= 0; i--) {
            printf("%d\t", img[i + w * j]);
        }
    }
    printf("\n");
}

void print_float(const float* img, int w, int h) {
    for (int j=h-1; j >= 0; j--) {
        printf("\n");
        for (int i = w - 1; i >= 0; i--) {
            printf("%.1f\t", img[i + w * j]);
        }
    }
    printf("\n");
}

int test_empty(float Q) {
    int w = 10;
    int h = 10;
    uchar* imdata = new uchar[w*h];
    for (int j=h-1; j >= 0; j--)
        for (int i=w-1; i >= 0; i--) {
            imdata[i+w*j] = 0;
        }

    int* lblOut = new int[w*h];
    float* avgOut = new float[w*h];
    srm_c(Q, imdata, w, h, true, true, avgOut, lblOut);

    print_int(lblOut, w, h);
    print_float(avgOut, w, h);
    return 0;
}

int test_primitives(float Q) {
    int w = 10;
    int h = 10;
    uchar* imdata = new uchar[w*h];
    for (int j=h-1; j >= 0; j--)
        for (int i=w-1; i >= 0; i--) {
            uchar val = 0;
            int ii = i - w/2;
            int jj = j - h/2;
            if (ii*ii + jj*jj < 4)
                val = 50;
            if (i*i + j*j < 4)
                val = 100;
            imdata[i+w*j] = val;
        }

    int* lblOut = new int[w*h];
    float* avgOut = new float[w*h];
    srm_c(Q, imdata, w, h, true, true, avgOut, lblOut);

    print_int(lblOut, w, h);
    print_float(avgOut, w, h);
    return 0;
}

int test_rand(float Q) {
    srand(10);
    int w = 10;
    int h = 10;
    uchar* imdata = new uchar[w*h];
    for (int j=h-1; j >= 0; j--)
        for (int i=w-1; i >= 0; i--) {
            imdata[i+w*j] = (uchar)((double)rand() / (double)RAND_MAX * 255);
        }

    int* lblOut = new int[w*h];
    float* avgOut = new float[w*h];
    srm_c(Q, imdata, w, h, true, true, avgOut, lblOut);

    print_int(lblOut, w, h);
    print_float(avgOut, w, h);
    return 0;
}

int test_performance(float Q) {
    srand(10);
    int w = 2048;
    int h = 2048;
    uchar* imdata = new uchar[w*h];
    for (int j=h-1; j >= 0; j--)
        for (int i=w-1; i >= 0; i--) {
            imdata[i+w*j] = (uchar)((double)rand() / (double)RAND_MAX * 255);
        }

    int* lblOut = new int[w*h];
    float* avgOut = new float[w*h];

    printf("Processing image [%d x %d]...", w, h);

    time_point<system_clock> start, end;
    start = system_clock::now();
    srm_c(Q, imdata, w, h, true, true, avgOut, lblOut);
    end = system_clock::now();
    duration<double> elapsed_seconds = end - start;

    printf("Elapsed time: %.4f s.", elapsed_seconds.count());
    return 0;
}

int main(int argc, char** argv) {
    test_empty(25);
    printf("\n");
    test_primitives(25);
    printf("\n");
    test_rand(15);
    printf("\n");
    test_performance(25);
    return 0;
}