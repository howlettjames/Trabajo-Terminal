import RPi.GPIO as GPIO
from time import sleep
import KeyboardModule as kp
 
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
 
kp.init()

class Motor():
    def __init__(self, EnaA, In1A, In2A, EnaB, In1B, In2B):
        self.EnaA = EnaA
        self.In1A = In1A
        self.In2A = In2A
        self.EnaB = EnaB
        self.In1B = In1B
        self.In2B = In2B
        
        GPIO.setup(self.EnaA,GPIO.OUT)
        GPIO.setup(self.In1A,GPIO.OUT)
        GPIO.setup(self.In2A,GPIO.OUT)
        GPIO.setup(self.EnaB,GPIO.OUT)
        GPIO.setup(self.In1B,GPIO.OUT)
        GPIO.setup(self.In2B,GPIO.OUT)
        
        self.pwmA = GPIO.PWM(self.EnaA, 20)
        self.pwmA.start(0)
        self.pwmB = GPIO.PWM(self.EnaB, 20)
        self.pwmB.start(0)

    def move(self, speed = 50, turn = 0, time = 0):
        # Normalizing the values
        speed *= 100
        turn *= 100

        leftSpeed = speed - turn
        rightSpeed = speed + turn

        if leftSpeed > 0:
            GPIO.output(self.In1A,GPIO.HIGH)
            GPIO.output(self.In2A,GPIO.LOW)
        else:
            GPIO.output(self.In1A,GPIO.LOW)
            GPIO.output(self.In2A,GPIO.HIGH)

        if rightSpeed > 0:
            GPIO.output(self.In1B,GPIO.HIGH)
            GPIO.output(self.In2B,GPIO.LOW)
        else:
            GPIO.output(self.In1B,GPIO.LOW)
            GPIO.output(self.In2B,GPIO.HIGH)

        if leftSpeed > 100: 
            leftSpeed = 100
        elif leftSpeed < -100:
            leftSpeed = -100
        if rightSpeed > 100: 
            rightSpeed = 100
        elif rightSpeed < -100: 
            rightSpeed = -100

        self.pwmA.ChangeDutyCycle(abs(leftSpeed))
        self.pwmB.ChangeDutyCycle(abs(rightSpeed))
        
        sleep(time)

    def moveB(self, speed = 50, time = 0):
        self.pwmA.ChangeDutyCycle(speed)
        GPIO.output(self.In1A,GPIO.LOW)
        GPIO.output(self.In2A,GPIO.HIGH)
        self.pwmB.ChangeDutyCycle(speed)
        GPIO.output(self.In1B,GPIO.LOW)
        GPIO.output(self.In2B,GPIO.HIGH)
        sleep(time)

    def stop(self, time = 0):
        self.pwmA.ChangeDutyCycle(0)
        self.pwmB.ChangeDutyCycle(0)
        sleep(time)

    def cleanGPIOS(self):
        self.pwmA.ChangeDutyCycle(0)
        self.pwmB.ChangeDutyCycle(0)

        GPIO.output(self.In1A,GPIO.LOW)
        GPIO.output(self.In2A,GPIO.LOW)
        GPIO.output(self.In1B,GPIO.LOW)
        GPIO.output(self.In2B,GPIO.LOW)
        
        GPIO.setup(self.EnaA,GPIO.IN)
        GPIO.setup(self.In1A,GPIO.IN)
        GPIO.setup(self.In1B,GPIO.IN)
        GPIO.setup(self.EnaB,GPIO.IN)
        GPIO.setup(self.In2A,GPIO.IN)
        GPIO.setup(self.In2B,GPIO.IN)

if __name__ == '__main__':
    motor = Motor(2, 3, 4, 17, 22, 27)

    try:
        while True:
            if kp.isKeyPressed('UP'):
                motor.move(0.8, 0, 0.1)
            elif kp.isKeyPressed('DOWN'):
                motor.move(-0.8, 0, 0.1)
            elif kp.isKeyPressed('LEFT'):
                motor.move(0.8, 0.8, 0.1)
            elif kp.isKeyPressed('RIGHT'):
                motor.move(0.8, -0.8, 0.1)
            else:
                motor.stop(0.1)        
    except KeyboardInterrupt:
        print("Cleaning GPIOS")
    except:
        print("Cleaning GPIOS")
    finally:
        print("Cleaning GPIOS 1")
        motor.cleanGPIOS()
        