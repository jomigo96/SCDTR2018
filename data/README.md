# Things to evaluate

## Independant mode
+ Computation delays
+ Comunication times (serial)
+ Jitter
+ Flicker (change pwm)
+ Closed loop characteristics, dampning, overshoot, ....
+ Improvements with feed-forward only or feed-back only
+ Evaluate simulator accuracy
+ Anti-windup

## Cooperative mode
+ Communication times (I2C)
+ Compare controllers (independant vs distributed)
+ Performance metrics
+ Recalculate jitter with the added communication overhead

# Files

+ `no_pwm_changes.csv` - Driven LED at constant 200/255 duty cycle to check for noise.
+ `with_pwm_change.csv` - Changed pwm and repeated experiment
+ `computation_time.csv` - Some computation times. Average is 1393 microseconds, std is 6.258 microseconds
+ `nyquist.csv` - data on the nyquist critical point
+ `jitter_single.csv` - time which control starts, single controller mode
+ `jitter_single_processed.csv` - processed to have only time difference
+ `feedback.csv` - Performance of pure feedback control
+ `feedforward.csv` - pure feedforward
+ `feedforward_feedback.csv` - feedback+feedforward
+ `windup.csv` - no anti-windup, a constant reference of 150 is held while a large outside disturbance incides. the box is closed and the output takes a long time to get to 150.
+ `antiwindup.csv` - same experiment, with anti-windup in place
+ `simulator.csv` - has Lmeasured - Lsimulator on some step changes

## Notes

+ Time spent in serial comms is on average 384 microseconds, for 6 bytes, the value in LUX.
+ Jitter: mean of 5ms and variance of 0.3248 microseconds squared
## Niquist critical point: Kp = 600, oscilation frequency: 20Hz
