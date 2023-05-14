#!/bin/bash 
#echo "" >> salida.dat
i=500
while [ "$i" -le  12500 ]
do
	 ./Generador/generador $i $3
    ./$1 $2 $3 /dev/null >> $1TablaDatosCota_$2.dat
      i=$(( $i + 500 ))
done
      
