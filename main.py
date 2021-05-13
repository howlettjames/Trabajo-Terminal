from MotorModule import Motor
import CameraModule
from LaneDetectionModule import getLaneCurve
import cv2

####################################
motor = Motor(2, 3, 4, 17, 22, 27)
imgCounter = 0
firstTime = 1
####################################

def main():
    global imgCounter
    global firstTime 
    
    ########################## PDI
    img = CameraModule.getImage()
    curveVal, imgResult = getLaneCurve(img, 1)
    # imgResult = cv2.resize(imgResult, (1920, 1440))
    print(f"Curve: {curveVal}")
    
    ######################### IMG WRITING
    cv2.imwrite(f'images/result{imgCounter}.jpg', imgResult)
    #cv2.imwrite(f'/var/www/html/images/warp{imgCounter}.jpg', imgWarp)

    if(firstTime > 0):
        cv2.imwrite(f'images/result{imgCounter}.jpg', imgResult)
        #cv2.imwrite(f'/var/www/html/images/warp{imgCounter}.jpg', imgWarp)
        firstTime -= 1
        imgCounter += 1
        cv2.waitKey(1)
    ######################### MOTOR
    else:
        sen = 1.0  # SENSITIVITY
        #maxVAl = 0.5 # MAX SPEED
        dutyCicle = 0.45
        time = 0.3

        #if curveVal > maxVAl: curveVal = maxVAl
        #if curveVal < -maxVAl: curveVal = -maxVAl
        
        if curveVal < 0: #Izquierda
            if curveVal <= -0.75:
                print("Error al detectar curva izquierda")
                motor.stop(5)
            elif curveVal <= -0.10:
                sen = 6
                print("Curva izquierda")
                #motor.stop(1)
            elif curveVal <= -0.07:
                sen = 3
                print("Alinear izquierda")
                #motor.stop(0.2)
            if curveVal > -0.07:
                curveVal = 0
        else: # Derecha
            if curveVal >= 0.75:
                print("Error al detectar curva derecha")
                #motor.stop(5)
            elif curveVal >= 0.10:
                sen = 6
                print("Curva derecha")
                #motor.stop(1)
            elif curveVal >= 0.07:
                sen = 3
                print("Alinear derecha")
                #motor.stop(0.2)
            if curveVal < 0.07: curveVal = 0

        if (curveVal > 0):
            print(f"{imgCounter} DERECHA {dutyCicle}, {-curveVal * sen}, {time}")
        elif (curveVal < 0):
            print(f"{imgCounter} IZQUIERDA {dutyCicle}, {-curveVal * sen}, {time}")
        else:
            print(f"{imgCounter} RECTO {dutyCicle}, {-curveVal * sen}, {time}")

        motor.move(dutyCicle, curveVal * sen, time)
        imgCounter += 1
        cv2.waitKey(1)
        motor.stop(0.01)
    
if __name__ == '__main__':
    while True:
        main()