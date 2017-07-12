import os
import numpy as np
from ctypes import cdll, c_float, c_int32, c_ubyte, c_bool, POINTER
libpath = os.path.join(os.path.dirname(__file__), 'libsrm.so')
srm_lib = cdll.LoadLibrary(libpath)
srm_fn = srm_lib.srm_c


def segment(img, q=25, get_labels=True, get_average=True):
    """
    Performs image segmentation using the SRM algorithm 
    :param img: input image (ndarray)
    :param q: complexity of the assumed distributions
              (affects the number of unique colors)
    :param get_labels: compute and return the labeled image
    :param get_average: compute and return the image with region's average colors
    :return: tuple of output images (ndarray, ndarray)
    """
    if not img.dtype == np.ubyte:
        min = img.min()
        max = img.max()
        img = (img - min) / (max - min) * 255
        img = img.astype(np.ubyte)
    c_q = c_float(q)
    c_img = img.ctypes.data_as(POINTER(c_ubyte))
    c_w = c_int32(img.shape[1])
    c_h = c_int32(img.shape[0])
    avg_out = np.zeros(img.shape, dtype=np.float32)
    lbl_out = np.zeros(img.shape, dtype=np.int32)
    c_avg_out = avg_out.ctypes.data_as(POINTER(c_float))
    c_lbl_out = lbl_out.ctypes.data_as(POINTER(c_int32))
    srm_fn(c_q, c_img, c_w, c_h, c_bool(get_average), c_bool(get_labels),
           c_avg_out, c_lbl_out)
    return avg_out, lbl_out
