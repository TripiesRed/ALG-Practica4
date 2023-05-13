#!/bin/bash 
#echo "" >> salida.dat
i=200
while [ "$i" -le  5000 ]
do
	 ./Generador/generador $i $3
    ./$1 $2 $3 /dev/null >> $1TablaDatosCota_$2.dat
      i=$(( $i + 200 ))
done
      
