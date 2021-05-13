import cv2 
import numpy as np
import utils

def getLaneCurve(img):
    imgCopy = img.copy()
    imgResult = img.copy()

    # STEP 1 Threshold
    imgThres = utils.thresholding(img)

    # STEP 2 Warp - Or "Region of interest"; we're interested in knowing how much curve
    # is present at the moment not far away
    heightT, widthT, channels = img.shape
    points = utils.valTrackbars()
    #points = np.float32([(102, 80), (480 - 102, 80), (20 , 214), (480 - 20, 214)])
    imgWarp = utils.warpImg(imgThres, points, widthT, heightT)
    imgWarpPoints = utils.drawPoints(imgCopy, points)

    cv2.imshow('Capture', img)
    # cv2.imshow('Thres', imgThres)
    cv2.imshow('Warp', imgWarp)
    cv2.imshow('Warp Points', imgWarpPoints)
    
    return None

if __name__ == '__main__':
    cap = cv2.VideoCapture(0)
    initialTrackbarValues = [0, 0, 0, 240]
    utils.initializeTrackbars(initialTrackbarValues)

    while True:
        success, img = cap.read()
        img = cv2.resize(img, (480, 240))

        getLaneCurve(img)

        # Commented cause already shown in stack images
        #cv2.imshow('Vid', img)
        cv2.waitKey(1)