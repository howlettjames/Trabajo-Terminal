import RPi.GPIO as GPIO
from time import sleep 
import os

EVER = True

# Configura los pines segun el microprocesador Broadcom
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

class Motor():
	def __init__(self, ENA, IN1, IN2, ENB, IN3, IN4):
		# Definir nombre de las entradas del puente H del motor A
		self.ENA = ENA
		self.IN1 = IN1
		self.IN2 = IN2
		
		# Definir nombre de las entradas del puente H del motor B
		self.ENB = ENB
		self.IN3 = IN3
		self.IN4 = IN4
		
		# Configura los pines de salida del motor A
		GPIO.setup(self.ENA, GPIO.OUT)
		GPIO.setup(self.IN1, GPIO.OUT)
		GPIO.setup(self.IN2, GPIO.OUT)
		
		# Configura los pines de salida del motor A
		GPIO.setup(self.ENB, GPIO.OUT)
		GPIO.setup(self.IN3, GPIO.OUT)
		GPIO.setup(self.IN4, GPIO.OUT)
		
		# Define las salidas PWM para el motor A y B
		self.__FRECUENCIA = 40
		self.PWM_A = GPIO.PWM(self.ENA, self.__FRECUENCIA)
		self.PWM_B = GPIO.PWM(self.ENB, self.__FRECUENCIA)

		# Inicializa los PWM con un duty cicly de cero
		self.PWM_A.start(0)
		self.PWM_B.start(0)
		
		
	def move(self, velocidad = 0.6, giro = 0, tiempo = 0):
		''' 
			Permite el movimiento hacia delante del carro
			especificando la velocidad e inclinacion de avance
			
			Parametros
			----------
			velocidad : float
				Especifica en porcentaje la velocidad del carro
				E.g. 0.6  - 60% de la velocidad total (100) 
				Su rango va de [-1, 1] para direccion positiva y 
				negativa respectivamente
			
			giro : float
				Establece los grados de inclinacion del robot
				El intervalo de giro va de [-1, 1] y representan:
				-1 : Giro hacia la izquierda
				 0 : Moverse de forma recta
				 1 : Giro hacia la derecha
			
			tiempo : int
				Sirve para establecer cuanto tiempo avanzara
				en la direccion especificada el carro
		'''
		
		# Recuperamos la velocidad y giro real
		velocidad *= 100
		giro *= 100
		
		velocidad_izq = velocidad - giro
		velocidad_der = velocidad + giro
		
		# Verificamos que las velocidades no sobrepasen el intervalo
		if (velocidad_izq > 100):
			velocidad_izq = 100
		elif (velocidad_izq < -100):
			velocidad_izq = -100
		
		if (velocidad_der > 100):
			velocidad_der = 100
		elif (velocidad_der < -100):
			velocidad_der = -100
		
		self.PWM_A.ChangeDutyCycle(abs(velocidad_izq))
		self.PWM_B.ChangeDutyCycle(abs(velocidad_der))
		
		# Necesitamos cambiar la direccion porque la funcion
		# ChangeDutyCycle no entiende numeros negativos
		
		# Mover hacia la direccion el motor A
		if (velocidad_izq > 0):	
			GPIO.output(self.IN1, GPIO.HIGH)
			GPIO.output(self.IN2, GPIO.LOW)
		else:
			GPIO.output(self.IN1, GPIO.LOW)
			GPIO.output(self.IN2, GPIO.HIGH)
			
		# Mover hacia la direccion el motor B
		if (velocidad_der > 0):	
			GPIO.output(self.IN3, GPIO.HIGH)
			GPIO.output(self.IN4, GPIO.LOW)
		else:
			GPIO.output(self.IN3, GPIO.LOW)
			GPIO.output(self.IN4, GPIO.HIGH)
		
		sleep(tiempo)
	
	def stop(self, tiempo = 0):
		self.PWM_A.ChangeDutyCycle(0)
		self.PWM_B.ChangeDutyCycle(0)
		sleep(tiempo)
	
	def stopTotal(self):
		self.PWM_A.stop()
		self.PWM_B.stop()
		GPIO.cleanup()
		os.system('clear')
		print('Los motores se han parado')

def main():
	motores.move(0.80, 0 , 5)

if __name__ == "__main__":
	sleep(5)
	try:
		motores = Motor(18,23,24,17,27,22)
		while EVER:
			main()
			
	except KeyboardInterrupt:
		motores.stopTotal()
