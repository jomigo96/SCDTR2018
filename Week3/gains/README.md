# Ganhos e constantes de tempo

Usa-se o data4 para calcular o ganho e a constante de tempo. Correr com
```
Rscript model_estimation.R
```

## output
```
  led final-value    gain-i tau-i
1   0    1.279038        NA    NA
2  50   55.524735 1.1104947 33612
3 100   98.210044 0.9821004 17008
4 150  135.046597 0.9003106 11336
5 200  168.184207 0.8409210  5664
[1] "Gain: [lux] / [led_power]"
[1] 0.9077774
[1] "Time constant: [s]"
[1] 0.011336
```

### Problemas

+ O primeiro ganho não é válido porque o valor inicial é aproximadamente 0.
+ A constante de tempo também não parece ser constante, fica mais rápido quando a variação é menor.
