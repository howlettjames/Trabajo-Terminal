import numpy as np
import cv2
from time import time
from Navegacion import obtenerCarril

EVER = True

velocidad = 0.85
tiempo = 0.1

def getImage(display = False, size = (480,240)):
    # Obtenemos la imagen
    cap = cv2.VideoCapture(0)
    
    # Capturar frame-por-frame
    _, img = cap.read()
    
    img = cv2.resize(img, size)
    
    if (display):
        cv2.imshow('Imagen', img)
    
    return img

if __name__ == '__main__':
    while EVER:
        start_time_img = time()
        
        img = getImage(display = True)
        
        end_time_img = time()
        final_time_img = end_time_img -  start_time_img
        
        print("Nueva foto\t\t" + str(final_time_img))
        
        start_time_carril = time()
        curveVal = obtenerCarril(img, 2)
        end_time_carril = time()
        final_time_carril = end_time_carril - start_time_carril
        
        print(f"curveVal: {curveVal}")
        
        prevCurve=curveVal
                
        sen = 1.3
        maxVal = 0.3
        
        if (curveVal > maxVal): curveVal = maxVal
        if (curveVal < -maxVal):    curveVal = -maxVal
        
        if curveVal > 0:
            sen = 1.6
            if (curveVal < 0.15):   curveVal = 0
        else:
            if (curveVal > -0.15):  curveVal = 0
        
        str_movimiento = ""
        if (curveVal == 0):
            str_movimiento = f"RECTO: {prevCurve} | {curveVal} - move({velocidad}, {curveVal * sen}, {tiempo})"
            
        elif (curveVal < 0):
            str_movimiento = f"IZQUIERDO: {prevCurve} | {curveVal} - move({velocidad}, {curveVal * sen}, {tiempo})"
        else:
            str_movimiento = f"DERECHO: {prevCurve} | {curveVal} - move({velocidad}, {curveVal * sen}, {tiempo})"
    
        print(str_movimiento)
        
        print(f"\t\tgiro: {-curveVal} * {sen}")
        print(f"\t\tmove(0.85, {-curveVal * sen}, 0.1)")
        
        with open("curveVal.txt", "a") as f:
            f.write("" + str(curveVal) + "\n")
        
        with open("timeImg.txt", "a") as f:
            f.write("" + str(final_time_img) + "\n")
            
        with open("timeCarril.txt", "a") as f:
            f.write("" + str(final_time_carril) + "\n")    
        
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    
    # When everything done, release the capture
    cap.release()
    cv2.destroyAllWindows()


