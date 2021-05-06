import cv2
import numpy as np

frameWidth  = 360
frameHeight = 240

def thresholding(img):
    imgHSV = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    lowerWhite = np.array([0,0,0])
    upperWhite = np.array([179,255,105])
    maskWhite = cv2.inRange(imgHSV, lowerWhite, upperWhite)
    return maskWhite

def warpImage(img, points, width, height, inv = False):
    puntos_origen = np.float32(points)
    puntos_final  = np.float32([(0, 0), (width, 0), (0, height), (width, height)])
    
    if (inv):
        matriz = cv2.getPerspectiveTransform(puntos_final, puntos_origen)
    else:
        matriz = cv2.getPerspectiveTransform(puntos_origen, puntos_final)
    
    imgWarp = cv2.warpPerspective(img, matriz, (width, height))
    return imgWarp

# Esto lo hace para una pequenia porcion o region delimitada de la imagen
def getHistogram(img, minPer=0.1, display= False, region=1):
    
    height, width = img.shape[:2]
    
    if region == 1:
        histValues = np.sum(img,axis=0)
    else:
        histValues = np.sum(img[height//region: , :],axis=0)
    
    maxValue = np.max(histValues)
    minValue = minPer*maxValue
    
    indexArray = np.where(histValues >= minValue)
    basePoint = int(np.average(indexArray))
    
    if display:
        imgHist = np.zeros((height,width,3), np.uint8)
        for x, intensity in enumerate(histValues):
            cv2.line(imgHist, (x,height), (x, height-intensity//255//region), (255,0,255), 1)
            cv2.circle(imgHist, (basePoint, height), 20, (0, 255, 255), cv2.FILLED)
        return basePoint, imgHist
    
    return basePoint

# Esto lo hace para toda la imagen
def getHistogramV1(img, minPer=0.1, display= False):
    
    histValues = np.sum(img,axis=0)
    
    maxValue = np.max(histValues)
    minValue = minPer*maxValue
    
    indexArray = np.where(histValues >= minValue)
    basePoint = int(np.average(indexArray))
    
    if display:
        imgHist = np.zeros((img.shape[0],img.shape[1],3), np.uint8)
        for x, intensity in enumerate(histValues):
            cv2.line(imgHist, (x,img.shape[0]), (x, img.shape[0]-intensity//255), (255,0,255), 1)
            cv2.circle(imgHist, (basePoint, img.shape[0]), 20, (0, 255, 255), cv2.FILLED)
        return basePoint, imgHist
    
    return basePoint
    
def stackImages(scale,imgArray):
    rows = len(imgArray)
    cols = len(imgArray[0])
    rowsAvailable = isinstance(imgArray[0], list)
    width = imgArray[0][0].shape[1]
    height = imgArray[0][0].shape[0]
    if rowsAvailable:
        for x in range ( 0, rows):
            for y in range(0, cols):
                if imgArray[x][y].shape[:2] == imgArray[0][0].shape [:2]:
                    imgArray[x][y] = cv2.resize(imgArray[x][y], (0, 0), None, scale, scale)
                else:
                    imgArray[x][y] = cv2.resize(imgArray[x][y], (imgArray[0][0].shape[1], imgArray[0][0].shape[0]), None, scale, scale)
                if len(imgArray[x][y].shape) == 2: imgArray[x][y]= cv2.cvtColor( imgArray[x][y], cv2.COLOR_GRAY2BGR)
        imageBlank = np.zeros((height, width, 3), np.uint8)
        hor = [imageBlank]*rows
        hor_con = [imageBlank]*rows
        for x in range(0, rows):
            hor[x] = np.hstack(imgArray[x])
        ver = np.vstack(hor)
    else:
        for x in range(0, rows):
            if imgArray[x].shape[:2] == imgArray[0].shape[:2]:
                imgArray[x] = cv2.resize(imgArray[x], (0, 0), None, scale, scale)
            else:
                imgArray[x] = cv2.resize(imgArray[x], (imgArray[0].shape[1], imgArray[0].shape[0]), None,scale, scale)
            if len(imgArray[x].shape) == 2: imgArray[x] = cv2.cvtColor(imgArray[x], cv2.COLOR_GRAY2BGR)
        hor= np.hstack(imgArray)
        ver = hor
    return ver

def empty(a):
	pass

# ===================================================
#	   Funciones para ajuste automatico de HSV
# ===================================================
def initializeTrackbarsHSV(wT = 480, hT = 240):
	cv2.namedWindow("HSV")
	cv2.resizeWindow("HSV", frameWidth, frameHeight)
	cv2.createTrackbar("HUE Min", "HSV", 0, 179, empty)
	cv2.createTrackbar("HUE Max", "HSV", 179, 179, empty)
	cv2.createTrackbar("SAT Min", "HSV", 0, 255, empty)
	cv2.createTrackbar("SAT Max", "HSV", 255, 255, empty)
	cv2.createTrackbar("VALUE Min", "HSV", 0, 255, empty)
	cv2.createTrackbar("VALUE Max", "HSV", 255, 255, empty)

def getValTrackbarsHSV(wT = 480, hT = 240):
	h_min = cv2.getTrackbarPos("HUE Min", "HSV")
	h_max = cv2.getTrackbarPos("HUE Max", "HSV")
	s_min = cv2.getTrackbarPos("SAT Min", "HSV")
	s_max = cv2.getTrackbarPos("SAT Max", "HSV")
	v_min = cv2.getTrackbarPos("VALUE Min", "HSV")
	v_max = cv2.getTrackbarPos("VALUE Max", "HSV")
	
	lower = np.array([h_min, s_min, v_min])
	upper = np.array([h_max, s_max, v_max])
	
	return lower, upper

# ===================================================
#	Funciones para ajuste automatico de perspectiva
# ===================================================
def initializeTrackbarsTP(initialTrackbarVals, wT = 480, hT = 240):
	cv2.namedWindow("Trackbars")
	cv2.resizeWindow("Trackbars", 360, 240)
	cv2.createTrackbar("Width  Top", "Trackbars", initialTrackbarVals[0], wT//2, empty)
	cv2.createTrackbar("Height Top", "Trackbars", initialTrackbarVals[1], hT, empty)
	cv2.createTrackbar("Width  Bottom", "Trackbars", initialTrackbarVals[2], wT//2, empty)
	cv2.createTrackbar("Height Bottom", "Trackbars", initialTrackbarVals[3], hT, empty)

def getValTrackbarsTP(wT = 480, hT = 240):
	widthTop     = cv2.getTrackbarPos("Width  Top", "Trackbars")
	heightTop    = cv2.getTrackbarPos("Height Top", "Trackbars")
	widthBottom  = cv2.getTrackbarPos("Width  Bottom", "Trackbars")
	heightBottom = cv2.getTrackbarPos("Height Bottom", "Trackbars")
	points = np.float32([(widthTop, heightTop), (wT - widthTop, heightTop),
						 (widthBottom, heightBottom), (wT - widthBottom, heightBottom)])
	return points
	
def drawPointsTP(img, points):
	for x in range(4):
		origen = (int(points[x][0]), int(points[x][1]))
		cv2.circle(img, origen , 15, (0, 0, 255), cv2.FILLED)
	return img

