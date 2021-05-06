### Servidor montado en Apache para captura, procesamiento y visualizacion de la imagen

El servidor utilizado para el proyecto es Apache Tomcat.
Para ejecutar el servidor es necesario realizar los siguientes pasos:
1. Colocar la carpeta del `Servidor` en la ruta de: `/var/www/html`.
2. Entrar a la ruta especificada a traves de la shell.
3. Compilar cada uno de los archivos ejecutando el siguiente comando:
	`> make`
	`>./principal`
4. Para limpiar la carpeta de todos los ejecutables, se proporciona el comando `make clean`, el cual permite eliminar los archivos .o

En este punto el servidor se encuentra corriendo.
