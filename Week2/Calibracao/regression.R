# R = 100 Ohm

measurements <- data.frame(
	c(465, 608, 3, 104, 250),
	c(213, 263, 34, 53, 123),
	c(268, 328, 51, 88, 169))

names(measurements) <- c("luminosity", "voltage-ldr1", "voltage-ldr2")
# luminosity in LUX, voltages in mV

convert_Ohm <- function(v){
	return (5-v/1000)/((v/1000)/100)
}

measurements["Rldr1"] <- lapply(measurements["voltage-ldr1"], convert_Ohm)
measurements["Rldr2"] <- lapply(measurements["voltage-ldr2"], convert_Ohm)
# values in Ohm

print(measurements)

# log log regression
mod1 <- lm(log10(luminosity) ~ log10(Rldr1), data = measurements)
mod2 <- lm(log10(luminosity) ~ log10(Rldr2), data = measurements)

print("LDR1")
print(mod1$coefficients)
print("LDR2")
print(mod2$coefficients)
