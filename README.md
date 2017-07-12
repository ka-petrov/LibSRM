# LibSRM

A C++ implementation of the statistical region merging [1] algorithm with Python bindings.

I wasn't particularly happy with other Python implementations mainly because of the high execution time for large images so I made this one.

Currently only grayscale 8-bit images are supported. Multi-channel support is planned.

The implementation is inspired by the ImageJ plugin [2], a substantial part of the code is borrowed from there and ported to C++.

---

[1] R. Nock, F. Nielsen (2004), "Statistical Region Merging", IEEE Trans. Pattern Anal. Mach. Intell. 26 (11): 1452-1458

[2] https://imagej.net/Statistical_Region_Merging