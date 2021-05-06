import pygame

WIDTH, HEIGHT = 100, 100
EVER = True

def init():
	pygame.init()
	window = pygame.display.set_mode((WIDTH, HEIGHT))

def getKey(keyName):
	ans = False
	for event in pygame.event.get():
		pass
	keyInput = pygame.key.get_pressed()
	myKey = getattr(pygame, 'K_{}'.format(keyName))
	
	if keyInput[myKey]:
		ans = True
	pygame.display.update()
	return ans

def main():
	if getKey('LEFT'):
		print("Tecla 'LEFT' fue presionada")
	if getKey('RIGHT'):
		print("Tecla 'RIGHT' fue presionada")

if __name__ == "__main__":
	init()
	while EVER:
		main()
	

