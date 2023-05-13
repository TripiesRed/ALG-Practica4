#!/bin/bash 

#Script que permite generar un caso dados dos argumentos:
# numero_puntos = Tamaño del problema a resolver
# <nombre_archivo> = Nombre del archivo donde se almacene el caso
#
#Así pues, además de generar el problema, este script también se encarga de hacer:
# 	1) make all (Para crear los archivos objetos)
# 	2) Resolver el problema/caso con los algoritmos Dyv y Específico



if (( $# > 4 || $# < 3 )); then echo "./init nombre_archivo numero_camareros fichero_salida <numero_comensales>"; exit

elif (( $# == 4 ))
	then
	make all
	./Generador/datagen $2 $1
	./Voraz/voraz $3 $1 $4
	./Especifico/especifico $1
	
else (( $# == 3 ))
	make all
	./Voraz/voraz $1 $2 $4
	./Especifico/especifico $1
	
fi

