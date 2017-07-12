#ifndef LIBSRM_LIBRARY_H
#define LIBSRM_LIBRARY_H

typedef unsigned char uchar;

void runSrm(float Q, const uchar* imdata, int w, int h,
            bool showAvg, bool showLbl, float* avgOut, int* lblOut);

extern "C" {
    void srm_c(float Q, const uchar* imdata, int w, int h,
               bool showAvg, bool showLbl, float* avgOut, int* lblOut) {
        runSrm(Q, imdata, w, h, showAvg, showLbl, avgOut, lblOut);
    }
}

#endif