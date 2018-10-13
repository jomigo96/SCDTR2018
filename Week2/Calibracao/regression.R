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
