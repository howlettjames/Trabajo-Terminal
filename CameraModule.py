import numpy as np
import cv2

EVER = True

def getImage(display = False, size = (480,240)):
    cap = cv2.VideoCapture(0)
    # Capturar frame-por-frame
    _, img = cap.read()
    
    img = cv2.resize(img, size)
    
    if (display):
        cv2.imshow('Imagen', img)
    
    return img

if __name__ == '__main__':
    while EVER:
        img = getImage(display = True)
        
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    
    # When everything done, release the capture
    cap.release()
    cv2.destroyAllWindows()
