import matplotlib.pyplot as plt
from skimage import io, img_as_ubyte
from skimage.exposure import equalize_hist

from pysrm import srm

if __name__ == '__main__':
    img = img_as_ubyte(equalize_hist(io.imread('test.tif')))
    avg_out, lbl_out = srm.segment(img, q=10)
    f, ax = plt.subplots(2, 2)
    ax = ax.ravel()
    ax[0].imshow(img, cmap='gray')
    ax[1].imshow(avg_out, cmap='gray')
    ax[2].imshow(lbl_out)
    plt.show()
