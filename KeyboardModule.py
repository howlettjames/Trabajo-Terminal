import pygame

def init():
    pygame.init()
    window = pygame.display.set_mode((100, 100))

def isKeyPressed(keyName):
    wasPressed = False
    for event in pygame.event.get():
        pass
    
    keyInput = pygame.key.get_pressed()
    
    myKey = getattr(pygame, 'K_{}'.format(keyName))
    if keyInput[myKey]:
        wasPressed = True

    pygame.display.update()
    return wasPressed

def main():
    if getKey('LEFT'):
        print('Key LEFT was pressed')
    if getKey('RIGHT'):
        print('Key RIGHT was pressed')

if __name__ == '__main__':
    init()
    while True:
        main()