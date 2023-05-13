#!/bin/bash 
#echo "" >> salida.dat
i=1000
while [ "$i" -le  25000 ]
do
	 ./Generador/generador $i $3
    ./$1 $2 $3 /dev/null >> $1TablaDatosCota_$2.dat
      i=$(( $i + 1000 ))
done
      
