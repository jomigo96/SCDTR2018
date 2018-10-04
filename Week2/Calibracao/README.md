# Script para achar a característica dos LDRs

Script em R para calcular o declive e interceção da característica dos LDRs numa escala log-log.

## Uso

Correr com 
```
Rscript regression.R
```
(Se tiver R instalado)


O output é 
```
  luminosity voltage ldr1 voltage ldr2  Rldr1  Rldr2
1        465          213          268 212995 267995
2        608          263          328 262995 327995
3          3           34           51  33995  50995
4        104           53           88  52995  87995
5        250          123          169 122995 168995
[1] "LDR1"
 (Intercept) log10(Rldr1) 
   -8.948050     2.195107 
[1] "LDR2"
 (Intercept) log10(Rldr2) 
  -11.220151     2.572978 
```


### LDRs

O LDR1 é aquele com os cabos mais tortos, e o 2 é o outro.
