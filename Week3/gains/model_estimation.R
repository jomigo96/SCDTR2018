# import data
data <- read.csv(file = "data4.csv", head = TRUE, sep = ",")

# convert voltage to LUX
m <- -0.9355
b <- 5.8515
Raux <- 10000
convert <- function(v){

	R <- ((5-v/1000)/(v/1000/Raux))
	L <- 10^((log10(R)-b)/m)
	return (L)
}

data["lum"] <- lapply(data["voltage"], convert)

# splice steps
set1 <- subset(data, led == 0)
set2 <- subset(data, led == 50)
set3 <- subset(data, led == 100)
set4 <- subset(data, led == 150)
set5 <- subset(data, led == 200)

z <- nrow(set1)
avg1 <- mean( set1[(z%/%2):z , "lum"] )
z <- nrow(set2)
avg2 <- mean( set2[(z%/%2):z , "lum"] )
z <- nrow(set3)
avg3 <- mean( set3[(z%/%2):z , "lum"] )
z <- nrow(set4)
avg4 <- mean( set4[(z%/%2):z , "lum"] )
z <- nrow(set5)
avg5 <- mean( set5[(z%/%2):z , "lum"] )

target <- 0.63*(avg2 - avg1) + avg1
z <- which( abs(set2["lum"] - target) == min(abs(set2["lum"] - target)) )
tau2 <- set2[z, "time"] - set2[1, "time"]

target <- 0.63*(avg3 - avg2) + avg2
z <- which( abs(set3["lum"] - target) == min(abs(set3["lum"] - target)) )
tau3 <- set3[z, "time"] - set3[1, "time"]

target <- 0.63*(avg4 - avg3) + avg3
z <- which( abs(set4["lum"] - target) == min(abs(set4["lum"] - target)) )
tau4 <- set4[z, "time"] - set4[1, "time"]

target <- 0.63*(avg5 - avg4) + avg4
z <- which( abs(set5["lum"] - target) == min(abs(set5["lum"] - target)) )
tau5 <- set5[z, "time"] - set5[1, "time"]

out <- data.frame(
				  c(0 , 50, 100, 150, 200),
				  c(avg1, avg2, avg3, avg4, avg5),
				  c(NA, avg2/50, avg3/100, avg4/150, avg5/200),
				  c(NA, tau2, tau3, tau4, tau5))
names(out) <- c("led", "final-value","gain-i","tau-i")

print(out)


# first gain is unusable since the initial value is close to 0
gain <- mean(out[3:5, "gain-i"])
print("Gain: [lux] / [led_power]")
print(gain)

# using the same data for the time-constant
tau <- mean(out[3:5, "tau-i"])/10^6
print("Time constant: [s]")
print(tau)

# Script output:
#   led final-value    gain-i tau-i
# 1   0    1.279038        NA    NA
# 2  50   55.524735 1.1104947 33612
# 3 100   98.210044 0.9821004 17008
# 4 150  135.046597 0.9003106 11336
# 5 200  168.184207 0.8409210  5664
# [1] "Gain: [lux] / [led_power]"
# [1] 0.9077774
# [1] "Time constant: [s]"
# [1] 0.011336

