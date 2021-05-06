from Motor import Motor
import Keypress as kp

EVER = True

# =================================
motores = Motor(18,23,24,17,27,22)
velocidad = 1
giro = 0.5
tiempo = 0.1
# =================================

kp.init()

def main():
	if (kp.getKey('UP')):
		print('Avanza')
		motores.move(0.85, 0 , tiempo)
	elif (kp.getKey('DOWN')):
		print('Retrocede')
		motores.move(-0.85, 0, tiempo)
	elif (kp.getKey('LEFT')):
		print('Gira izquierda')
		motores.move(0.85, 0.3 , tiempo)
	elif (kp.getKey('RIGHT')):
		print('Gira derecha')
		motores.move(0.85, -0.3, tiempo)
	else:
		motores.stop(0.1)

def mainCarroGrande():
	if (kp.getKey('UP')):
		print('Avanza')
		motores.move(0.85, 0 , tiempo)
	elif (kp.getKey('DOWN')):
		print('Retrocede')
		motores.move(-0.85, 0, tiempo)
	elif (kp.getKey('LEFT')):
		print('Gira izquierda')
		motores.move(0.85, -0.3 , tiempo)
	elif (kp.getKey('RIGHT')):
		print('Gira derecha')
		motores.move(0.85, 0.3, tiempo)
	else:
		motores.stop(0.1)

if __name__ == "__main__":
	isCarroGrande = False
	while EVER:
		if (isCarroGrande):
			mainCarroGrande()
		else:
			main()
		
