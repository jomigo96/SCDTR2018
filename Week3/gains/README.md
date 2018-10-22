# Ganhos e constantes de tempo

Usa-se o data4 para calcular o ganho e a constante de tempo. Correr com
```
Rscript gains.R
```

## output
```
  led final-value    gain-i tau-i
1   0    1.279038        NA    NA
2  50   55.524735 43.411321 33612
3 100   98.210044  1.768762 17008
4 150  135.046597  1.375079 11336
5 200  168.184207  1.245379  5664
[1] "Gain: [lux] / [led_power]"
[1] 0.02926147
[1] "Time constant: [s]"
[1] 0.011336
```

### Problemas

+ O primeiro ganho não é válido porque o valor inicial é aproximadamente 0.
+ A constante de tempo também não parece ser constante, fica mais rápido quando a variação é menor.
