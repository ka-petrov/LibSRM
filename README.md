# LibSRM

A C++ implementation of the statistical region merging [1] algorithm with Python bindings.

I wasn't happy with other Python implementations mainly because of the high execution time for large images so I made this one.

Currently only grayscale 8-bit images are supported.

The implementation is basically a C++ port of the code from ImageJ plugin [2].

---
## Usage tips

To build C++ lib and tests:
  > cd libsrm && cmake . && make

To use python bindings copy libsrm.so (or .dll for Windows) to ./pysrm. See usage example in srm_test.py.

---

[1] R. Nock, F. Nielsen (2004), "Statistical Region Merging", IEEE Trans. Pattern Anal. Mach. Intell. 26 (11): 1452-1458

[2] https://imagej.net/Statistical_Region_Merging ([on Github](https://github.com/fiji/Statistical_Region_Merging))
