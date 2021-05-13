import cv2 
import numpy as np
import utils

curveList = []
averageValue = 10

# 0 will not display anything, 1 will display just the result and 2 the complete pipeline
def getLaneCurve(img, display = 2):
    imgCopy = img.copy()
    imgResult = img.copy()

    # STEP 1 Threshold
    imgThres = utils.thresholding(img)

    # STEP 2 Warp - Or "Region of interest"; we're interested in knowing how much curve is present at the moment not far away
    heightT, widthT, channels = img.shape
    # points = np.float32([(widthTop, heightTop), (widthT - widthTop, heightTop), (widthBottom, heightBottom), (widthT-widthBottom, heightBottom)])
    # points = np.float32([(42, 120), (widthT - 42, 120), (0 , 214), (widthT - 0, 214)])
    points = np.float32([(0, 150), (widthT - 0, 150), (0 , 240), (widthT - 0, 240)])
    imgWarp = utils.warpImg(imgThres, points, widthT, heightT)
    imgWarpPoints = utils.drawPoints(imgCopy, points)

    # STEP 3 Histogram
    # Middle point of path
    middlePoint, imgHist1 = utils.getHistogram(imgWarp, display = True, minPercentage = 0.5, region = 4) 
    # Middle point of all image
    curveAveragePoint, imgHist2 = utils.getHistogram(imgWarp, display = True, minPercentage = 0.9) 
    curveRaw = curveAveragePoint - middlePoint

    # STEP 4 Averaging To smooth if there's noise
    curveList.append(curveRaw)
    if len(curveList) > averageValue:
        curveList.pop(0)

    #curve = int(sum(curveList)/len(curveList))
    curve = curveRaw / 100
    if curve > 1: curve = 1
    elif curve < -1: curve = -1

    # STEP 5 Display
    if display == 1 or display == 2:
       #imgInvWarp = utils.warpImg(imgWarp, points, widthT, heightT, inverse = True)
       imgInvWarp = cv2.cvtColor(imgThres, cv2.COLOR_GRAY2BGR)
       imgInvWarp[0:heightT // 3, 0:widthT] = 0, 0, 0
       imgLaneColor = np.zeros_like(img)
       imgLaneColor[:] = 0, 255, 0
       imgLaneColor = cv2.bitwise_and(imgInvWarp, imgLaneColor)
       imgResult = cv2.addWeighted(imgResult, 1, imgLaneColor, 1, 0)
       midY = 450   
       cv2.putText(imgResult, str(curve), (widthT // 2 - 80, 85), cv2.FONT_HERSHEY_COMPLEX, 2, (255, 0, 255), 3)
       cv2.line(imgResult, (widthT // 2, midY), (int(widthT // 2 + (curve * 3)), midY), (255, 0, 255), 5)
       cv2.line(imgResult, (int(widthT // 2 + (curve * 3)), midY - 25), (int(widthT // 2 + (curve * 3)), midY + 25), (0, 255, 0), 5)
       for x in range(-30, 30):
           w = widthT // 20
           cv2.line(imgResult, (w * x + int(curve // 50), midY - 10), (w * x + int(curve // 50), midY + 10), (0, 0, 255), 2)
       #fps = cv2.getTickFrequency() / (cv2.getTickCount() - timer);
       #cv2.putText(imgResult, 'FPS '+str(int(fps)), (20, 40), cv2.FONT_HERSHEY_SIMPLEX, 1, (230,50,50), 3);
    
    if display == 2:
        imgStacked = utils.stackImages(0.8, ([img, imgWarp, imgInvWarp], [imgHist2, imgLaneColor, imgResult]))
        cv2.imshow('ImageStack', imgStacked)
        return curve, imgStacked
    elif display == 1:
        imgStacked = utils.stackImages(0.8, ([img, imgWarpPoints, imgWarp], [imgHist2, imgLaneColor, imgResult]))
        return curve, imgStacked

    return curve

if __name__ == '__main__':
    #cap = cv2.VideoCapture('vid1.mp4')
    #initialTrackbarValues = [102, 80, 20, 214]
    #utils.initializeTrackbars(initialTrackbarValues)

    frameCounter = 0
    while True:
        frameCounter += 1
        if cap.get(cv2.CAP_PROP_FRAME_COUNT) == frameCounter:
            cap.set(cv2.CAP_PROP_POS_FRAMES, 0)
            frameCounter = 0

        success, img = cap.read()
        img = cv2.resize(img, (480, 240))

        curve = getLaneCurve(img, display = 2)
        print(curve)

        # Commented cause already shown in stack images
        #cv2.imshow('Vid', img)
        cv2.waitKey(1)