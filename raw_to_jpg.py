import cv2 
import numpy as np

cc = 0

fd = open('/home/kolmogorov/april_test/Acquisizione02.raw', 'rb')
while(1):
    rows = 1024
    cols = 1280
    f = np.fromfile(fd, dtype=np.uint8,count=rows*cols)
    im = f.reshape((rows, cols)) #notice row, column format
    cv2.imwrite("/home/kolmogorov/april_test/bari2/img_" + str(cc) + ".jpg", im)
    # print(im)
    # while(1):
    #     cv2.imshow('', im)
    #     k = cv2.waitKey(33)
    #     if k==27:    # Esc key to stop
    #         break
    #     elif k==-1:  # normally -1 returned,so don't print it
    #         continue
    cc = cc +1
    print(cc)
# cv2.destroyAllWindows()
fd.close()
