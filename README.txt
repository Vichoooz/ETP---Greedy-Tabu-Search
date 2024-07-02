Codigos para hacer funcionar el makefile: 

make : Inicia el programa y genera los 3 outputs.
make clean: Borra todo lo generado anteriormente.


El programa te solicitará ingresar el nombre de uno de los archivos de datos a leer. Los archivos disponibles son:

-TorontoE92
-St.Andrews83
-EdHEC92
-YorkMills82
-Trent92

Después, el programa te preguntará cuántas iteraciones deseas realizar con el algoritmo de búsqueda tabú. El tamaño predeterminado de la lista tabú es de 100.

Si deseas cambiar el tamaño de la lista tabú, debes editar el archivo fuente del programa. Dirígete a la línea 223 y modifica el valor de la constante TAMANO_LISTA_TABU según tus necesidades:

223: const int TAMANO_LISTA_TABU = 100;