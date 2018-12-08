#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Arduino.h"

extern float K11, K21, K22, K12, o1, o2;
extern const int sensor_pin, led_pin;


inline int saturation(int value);
inline int median(int v1, int v2, int v3);
inline float deadzone(float error, float epsilon);


class Controller{

public:
	Controller() : 
		b(5.8515),
		m(-0.9355),
		Raux(10000),
		C(1e-6){

		integral = 0;
		u = 0;
		error_keep = 0;	
		t = 0;
		old_value = 40;
		last_target = 80;
	}

	Controller(float b, float m, float Raux, float C) : 
		b(b),
		m(m),
		Raux(Raux),
		C(C){
	
		integral = 0;
		u = 0;
		error_keep = 0;	
		t = 0;
		old_value = 40;
		last_target = 80;
	}

	float compute_lux(int s){

		float v, R;
	
		v = s * 5.0/1.0230;
		R = (5-v/1000.0)/(v/1000.0/Raux);
		return pow(10, (log10(R)-b)/m );

	}
	void PID_control(float target){
	
		int s1, s2, s3;
		float v, ff, R, value;
		float Ltarget, error, L;
		const float epsilon = 0.7;
		const float KP=0.05;
		const float KI=0.00001;
		const float h=0.005;

		// Resets simulator if reference value changes
		if(target != last_target){
			old_value = last_target;
			last_target = target;
			t = 0;
		}

		// Median filter
		s1 = analogRead(sensor_pin);
		s2 = analogRead(sensor_pin);
		s3 = analogRead(sensor_pin);
		v = median(s1, s2, s3) * 5.0/1.0230;
		R = (5-v/1000.0)/(v/1000.0/Raux);
		L = pow(10, (log10(R)-b)/m );

	    //Feed-forward gain
		ff = (target-o1) / K11 * 255.0;

	    //Simulator
		Ltarget = target - (target - old_value)*exp(-t/((R+Raux)*C));

	    //Feed-back
		error = deadzone(Ltarget - L, epsilon);
		integral = integral + h/2.0*(error + error_keep);
		u += (integral*KI + error)*KP;

		//Actuation
		value = round(ff+u);
		analogWrite(led_pin, saturation(value));
    
		//Clean-up
#ifndef SUPRESS_LUX
    Serial.println(L);
#endif
		t += h;
	    
		//Anti windup, cancel integration
		if(value > 255){
			u = 255 - ff;
			integral = integral - h/2.0*(error+error_keep);  
		}else if(value<0){
			u = -ff;     
			integral = integral - h/2.0*(error+error_keep); 
		}
	    
		error_keep = error;

	}

private:

	// LDR parameters
	float b;
	float m;
	float Raux;
	float C;

	// PID control variables
	float old_value;
	float t;
	float integral;
	float error_keep;
	float u;
	float last_target;
};

// Auxiliary functions

inline float deadzone(float error, float epsilon){

	float out;
	if(error > epsilon){
		out = error-epsilon;  
	}else if(error < epsilon){
		out = error+epsilon;
	}else
		out = 0;

	return out;
}

inline int saturation(int value){

	int out;
  
	if(value > 255){
		out = 255;
	}else if(value < 0){
		out = 0;
	}else
		out = value;
    
	return out;
}

inline int median(int v1, int v2, int v3){
  
	float maximum = v1;

	maximum = (maximum < v2) ? v2 : maximum;
	maximum = (maximum < v3) ? v3 : maximum;

	if (maximum == v1)
		return max(v2, v3);
	else if(maximum == v2)
		return max(v1, v3);
	else
		return max(v1,v2);
  
}

#endif //CONTROLLER_H
