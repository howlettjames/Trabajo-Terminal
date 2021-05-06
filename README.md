# Proyecto de Robot Autonomo Movil Terrestre utilizando Procesamiento Digital de Imagenes


- `utileria.py`: Contiene las funciones necesarias del procesamiento digital de imagenes, tales como: Segmentacion, ajuste de perspectiva, obtencion del histograma, asi como funciones para un ajuste automatico de segmentacion por HSV, usando trackbars.
- `Navegacion.py`: Contiene la funcion principal, `obtenerCarril` para llevar a cabo todas las transformaciones necesarios con el fin de obtener el valor de curvatura del camino.
- `Camara.py`: Almacena una funcion para la obtencion de la imagen utilizando la camara conectada a la Raspberry
- `Motor.py`: Alberga la clase encargada de inicializar los GPIOs de la Raspberry para la manipulacion de los motores.
	> Nota: La asignacion de pines puede variar.
- `Keypress.py`: Integra la funcion para la deteccion de teclas presionadas.
- `MotorKeyboard.py`: Contiene el programa que permite manipular los motores a partir de las teclas de: LEFT, RIGHT, UP and DOWN.

