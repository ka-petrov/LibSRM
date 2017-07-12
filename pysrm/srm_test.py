import matplotlib.pyplot as plt
from skimage import io, img_as_ubyte
from skimage.exposure import equalize_hist

from pysrm import srm

if __name__ == '__main__':
    img = img_as_ubyte(equalize_hist(io.imread('test.tif')))
    avg_out, lbl_out = srm.segment(img, q=15)
    plt.figure()
    plt.imshow(avg_out, cmap='gray')
    plt.show()
    plt.figure()
    plt.imshow(lbl_out)
    plt.show()
