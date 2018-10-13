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
[1] "LDR1"
      (Intercept) log10(luminosity)
        5.8514615        -0.9355354
[1] "LDR2"
      (Intercept) log10(luminosity)
        5.3060498        -0.7723981
  luminosity voltage-ldr1 voltage-ldr2    Rldr1    Rldr2 lum-calc1 lum-calc2
1        465          213          268 2247.418 1765.672  469.9254  463.3455
2        608          263          328 1801.141 1424.390  595.3736  611.8894
3        104           53           88 9333.962 5581.818  102.5737  104.4105
4        250          123          169 3965.041 2858.580  256.1387  248.3178
```

![alt-text][reg1]

![alt-text][reg2]

### Formulas

A luminosidade de cada LDR é então:

![alt-text][ldr1]

![alt-text][ldr2]

Com a resistência determinada por:

![alt-text][r]


### LDRs

O LDR1 é aquele com os cabos mais tortos, e o 2 é o outro.

[ldr1]: l1.png
[ldr2]: l2.png
[r]: r.png
[reg1]: graph_ldr1.png
[reg2]: graph_ldr2.png
