import cv2
import numpy as np
import utileria as util

EVER = True
curveList = []
avgVal = 10

def obtenerCarril(img, display = 2):
    
    imgResult = img.copy()
    imgCopy = img.copy()
    hT, wT = img.shape[:2]
    
    # Obtenemos los valores de segmentacion
    #lowerWhite, upperWhite = util.getValTrackbarsHSV()
    #lowerWhite = np.array([0,0,0])
    #upperWhite = np.array([179,255,105])
    
    # Segmentamos la imagen del camino
    imgThres = cv2.bitwise_not(util.thresholding(img))
    
    # Obtenemos los puntos de la perspectiva transformada
    #initializeTrackbarVals = [112, 80, 18, 203]
    #util.initializeTrackbarsTP(initializeTrackbarVals)
    #points = util.getValTrackbarsTP()
    widthTop = 69
    heightTop = 91
    widthBottom = 15
    heightBottom = 204
    points = np.float32([(widthTop, heightTop), (wT - widthTop, heightTop),
                        (widthBottom, heightBottom), (wT - widthBottom, heightBottom)])
    
	# Transformamos la perspectiva del camino
    imgWarp = util.warpImage(imgThres, points, wT, hT)
    
    # Dibujamos los puntos del poligono usado para
    # la transformacion de perspectiva del camino
    imgWarpPoints = util.drawPointsTP(imgCopy, points)
    
    # Obtenemos el punto medio central para una region del camino
    middlePoint,imgHist = util.getHistogram(imgWarp, minPer = 0.5, display=True, region = 4)
    
    # Obtenemos el punto medio central de la imagen completa
    curveAveragePoint,imgHist = util.getHistogram(imgWarp, minPer = 0.9, display=True)
    
    # Determinamos el valor de la curvatura de nuestro camino
    realCurve = curveAveragePoint - middlePoint
    
    # Reducimos la oscilacion del ruido al momento de detectar curvas
    # para evitar valores muy altos o muy pequenios para obtener
    # transiciones suaves
    curveList.append(realCurve)
    if (len(curveList) > avgVal):
        curveList.pop(0)
    curve = int(sum(curveList) / len(curveList))    
    
        
    # Paso 06
    # Visualizamos el trayecto del robot
    if display != 0:
        # Obtenemos la region ubicada a partir del
        # cambio de perspectiva en la imagen original
        imgInvWarp = util.warpImage(imgWarp, points, wT, hT,inv = True)
        imgInvWarp = cv2.cvtColor(imgInvWarp,cv2.COLOR_GRAY2BGR)
        imgInvWarp[0:hT//3,0:wT] = 0,0,0
        
        imgLaneColor = np.zeros_like(img)
        imgLaneColor[:] = 0, 255, 0
        imgLaneColor = cv2.bitwise_and(imgInvWarp, imgLaneColor)

        imgResult = cv2.addWeighted(imgResult,1,imgLaneColor,1,0)
        midY = 450
        cv2.putText(imgResult,str(curve),(wT//2-80,85),cv2.FONT_HERSHEY_COMPLEX,2,(255,0,255),3)
        cv2.line(imgResult,(wT//2,midY),(wT//2+(curve*3),midY),(255,0,255),5)
        cv2.line(imgResult, ((wT // 2 + (curve * 3)), midY-25), (wT // 2 + (curve * 3), midY+25), (0, 255, 0), 5)
        for x in range(-30, 30):
            w = wT // 20
            cv2.line(imgResult, (w * x + int(curve//50 ), midY-10),
                                (w * x + int(curve//50 ), midY+10), (0, 0, 255), 2)
    
    # Es una normalizacion de los datos para poder 
    # detectar las curvas izquierdas o derechas
    curve /= 100
    if (curve > 1):
        crurve = 1
    if (curve < -1):
        curve = -1
        
    if display == 2:
       imgStacked = util.stackImages(0.7,([img,imgWarpPoints,imgWarp],[imgHist,imgLaneColor,imgResult]))
       cv2.imshow('ImageStack',imgStacked)
    elif display == 1:
        cv2.imshow('Threshold', imgThres)
        #cv2.imshow('Warp', imgWarp)
        #cv2.imshow('Warp Points', imgWarpPoints)
        cv2.imshow('Histrograma', imgHist)
        cv2.imshow('Lane Color', imgLaneColor)
        cv2.imshow('Img Result', imgResult)
    
    return curve


if __name__ == '__main__':
    cap = cv2.VideoCapture('images/vid1.h264')
    #initializeTrackbarVals = [121, 71, 25, 215]
    #util.initializeTrackbarsHSV()
    #util.initializeTrackbarsTP(initializeTrackbarVals)
    frameCounter = 0
    
    while EVER:
        frameCounter += 1
        if cap.get(cv2.CAP_PROP_FRAME_COUNT) == frameCounter:
            cap.set(cv2.CAP_PROP_POS_FRAMES,0)
            frameCounter=0
        
        success, img = cap.read()
        
        try:
            img = cv2.resize(img, (480, 240))
        except:
            break
        
        curve = obtenerCarril(img, display=1)        
        print(curve)
        cv2.imshow('Video', img)
        cv2.waitKey(1)
