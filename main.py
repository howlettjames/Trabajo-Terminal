import Keypress as kp
import Camara
from Motor import Motor
from Navegacion import obtenerCarril
import cv2
from time import sleep

####################################
motor = Motor(18,23,24,17,27,22)
velocidad = 0.72
tiempo = 0.14
####################################

imgCounter = 0


#kp.init()

def guardarImg(image, leyenda, counter):      
    # font
    font = cv2.FONT_HERSHEY_SIMPLEX
    
    # org
    org = (50, 50)
    
    # fontScale
    fontScale = 1
    
    # Blue color in BGR
    color = (255, 0, 0)
      
    # Line thickness of 2 px
    thickness = 2
        
    # Using cv2.putText() method
    image = cv2.putText(image, leyenda, org, font, 
                       fontScale, color, thickness, cv2.LINE_AA)
    
    name_img = "images/Image" + str(counter) + ".png"
    
    cv2.imwrite(name_img, image)
    
def main(isCarroGrande = False):
    
    global imgCounter
    
    img = Camara.getImage(display = True)
    curveVal = obtenerCarril(img, 2)
    
    sen = 1.3
    maxVal = 0.3
    
    prevCurve = curveVal
    
    if (curveVal > maxVal): curveVal = maxVal
    if (curveVal < -maxVal):    curveVal = -maxVal
    
    if curveVal > 0:
        sen = 1.5
        if (curveVal < 0.10):   curveVal = 0
    else:
        if (curveVal > -0.08):  curveVal = 0
    
    str_movimiento = ""
    
    if (curveVal == 0):
        str_movimiento = f"RECTO: {prevCurve} | {curveVal} - move({velocidad}, {curveVal * sen}, {tiempo})"
        
    elif (curveVal < 0):
        str_movimiento = f"IZQUIERDO: {prevCurve} | {curveVal} - move({velocidad}, {curveVal * sen}, {tiempo})"
    else:
        str_movimiento = f"DERECHO: {prevCurve} | {curveVal} - move({velocidad}, {curveVal * sen}, {tiempo})"
    
    print(str_movimiento)
    
    guardarImg(img, str_movimiento, imgCounter)
     
    motor.move(velocidad, curveVal * sen, tiempo)
    
    imgCounter += 1
    


def moverCarro():
    if kp.getKey('UP'):
        motor.move(0.8, 0, 0.1)
    elif kp.getKey('DOWN'):
        motor.move(-0.8, 0, 0.1)
    elif kp.getKey('LEFT'):
        motor.move(0.8, -0.3, 0.1)
    elif kp.getKey('RIGHT'):
        motor.move(0.8, 0.3, 0.1)
    else:
        motor.stop(0.1)

if __name__ == '__main__':
    sleep(7)
    while True:
        #moverCarro()
        main() 
        
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
