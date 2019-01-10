library(ggplot2)
R = 100 #Ohm

measurements <- data.frame(
	c(465, 608, 104, 250), # Extra datapoint - 3
	c(213, 263, 53, 123), # Extra datapoint - 34
	c(268, 328, 88, 169)) # Extra datapoint - 51

names(measurements) <- c("luminosity", "voltage-ldr1", "voltage-ldr2")
# luminosity in LUX, voltages in mV

convert_Ohm <- function(v){
	return ((5-v/1000)/((v/1000)/R))
}

measurements["Rldr1"] <- lapply(measurements["voltage-ldr1"], convert_Ohm)
measurements["Rldr2"] <- lapply(measurements["voltage-ldr2"], convert_Ohm)
# values in Ohm

# log log regression
mod1 <- lm(log10(Rldr1) ~ log10(luminosity), data = measurements)
mod2 <- lm(log10(Rldr2) ~ log10(luminosity), data = measurements)

print("LDR1")
print(mod1$coefficients)
print("LDR2")
print(mod2$coefficients)

measurements["lum-calc1"] <- lapply(measurements["Rldr1"], function(r){return (10^((log10(r)-mod1$coefficients[1])/mod1$coefficients[2]))})

measurements["lum-calc2"] <- lapply(measurements["Rldr2"], function(r){return (10^((log10(r)-mod2$coefficients[1])/mod2$coefficients[2]))})

print(measurements)

png(filename = "graph_ldr1.png")
ggplot(data=measurements, aes(x=log10(Rldr1), y=log10(luminosity)))+geom_point()
dev.off()

png(filename = "graph_ldr2.png")
ggplot(data=measurements, aes(x=log10(Rldr2), y=log10(luminosity)))+geom_point()
dev.off()

# Script output:
# [1] "LDR1"
#       (Intercept) log10(luminosity) 
#         5.8514615        -0.9355354 
# [1] "LDR2"
#       (Intercept) log10(luminosity) 
#         5.3060498        -0.7723981 
#   luminosity voltage-ldr1 voltage-ldr2    Rldr1    Rldr2 lum-calc1 lum-calc2
# 1        465          213          268 2247.418 1765.672  469.9254  463.3455
# 2        608          263          328 1801.141 1424.390  595.3736  611.8894
# 3        104           53           88 9333.962 5581.818  102.5737  104.4105
# 4        250          123          169 3965.041 2858.580  256.1387  248.3178

