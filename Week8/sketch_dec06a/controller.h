#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Arduino.h"

extern float K11, K12, o1;
extern const float c1;
extern const int sensor_pin, led_pin;


inline int saturation(int value);
inline int median(int v1, int v2, int v3);
inline float deadzone(float error, float epsilon);


class Controller{

public:
	Controller() : Controller(5.8515, -0.9355, 10000, 1e-6){}

	Controller(float b, float m, float Raux, float C) : 
		b(b),
		m(m),
		Raux(Raux),
		C(C),
		rho(0.07){
	
		integral = 0;
		u = 0;
		error_keep = 0;	
		t = 0;
		old_value = 40;
		last_target = 80;
		d1 = 0;
		d2 = 0;
		d1_av = 0;
		d2_av = 0;
		y1 = 0;
		y2 = 0;
		n = 0;
		l_bound = 80;
	}

	float compute_lux(int s){

		float v, R;
	
		v = s * 5.0/1.0230;
		R = (5-v/1000.0)/(v/1000.0/Raux);
		return pow(10, (log10(R)-b)/m );

	}
	void PID_control(const float &target, int &dimming, float &L){
	
		int s1, s2, s3;
		float v, ff, R, value, Ltarget;
		float error;
		const float epsilon = 0.7;
		const float KP=0.05;
		const float KI=0.00001;
		const float h=0.005;

		
#ifdef TIMING
		long t1, t2;
		t1 = micros();
#endif

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
    
#ifdef TIMING
		t2 = micros();
		Serial.print("PID time taken (micros): ");
		Serial.println(t2-t1);
#endif

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

		dimming = round(saturation(value)/255.0*100.0); // Dimming, 0-100
	}

	void consensus_iteration(){
		
		const float k11 = K11/100;
		const float k12 = K12/100;
		float z1, z2;
		float cost_best = 1e20;
		float d1_best = -1;
		float d2_best = -1;
		float d1_sol, d2_sol;
		float cost;
		float n;

		z1 = rho*d1_av-y1-c1;
		z2 = rho*d2_av-y2;

		// Unconstrained
		d1_sol = 1/rho*z1;
		d2_sol = 1/rho*z2;
		if(this->check_feasibility(d1_sol, d2_sol)){
			cost = this->evaluate_cost(d1_sol, d2_sol);
			if(cost < cost_best){
				d1 = d1_sol;
				d2 = d2_sol;
				return;
			}
		}	

		// constrained to linear boundary
		n = pow(k11,2)+pow(k12,2);
		d1_sol = 1/rho*z1 - k11/n * (o1-l_bound+(z1*k11+z2*k12)/rho);
		d2_sol = 1/rho*z2 - k12/n * (o1-l_bound+(z1*k11+z2*k12)/rho);
		if(this->check_feasibility(d1_sol, d2_sol)){
			cost = this->evaluate_cost(d1_sol, d2_sol);
			if(cost < cost_best){
				d1_best = d1_sol;
				d2_best = d2_sol;
				cost_best = cost;
			}
		}

		// constrained to 0 boundary
		d1_sol = 0; 
		d2_sol = z2/rho;
		if(this->check_feasibility(d1_sol, d2_sol)){
			cost = this->evaluate_cost(d1_sol, d2_sol);
			if(cost < cost_best){
				d1_best = d1_sol;
				d2_best = d2_sol;
				cost_best = cost;
			}
		}

		// constrained to 100 boundary
		d1_sol = 100;
		d2_sol = z2/rho;
		if(this->check_feasibility(d1_sol, d2_sol)){
			cost = this->evaluate_cost(d1_sol, d2_sol);
			if(cost < cost_best){
				d1_best = d1_sol;
				d2_best = d2_sol;
				cost_best = cost;
			}
		}

		// constrained to linear and 0 boundary
		d1_sol = 0;
		d2_sol = (o1-l_bound)/k12;
		if(this->check_feasibility(d1_sol, d2_sol)){
			cost = this->evaluate_cost(d1_sol, d2_sol);
			if(cost < cost_best){
				d1_best = d1_sol;
				d2_best = d2_sol;
				cost_best = cost;
			}
		}

		// constrained to linear and 100 boundary
		d1_sol = 100;
		d2_sol = (o1-l_bound+100*k11);
		if(this->check_feasibility(d1_sol, d2_sol)){
			cost = this->evaluate_cost(d1_sol, d2_sol);
			if(cost < cost_best){
				d1_best = d1_sol;
				d2_best = d2_sol;
				cost_best = cost;
			}
		}

		d1 = d1_best;
		d2 = d2_best;
	}

	void get_dimmings(float &dd1, float &dd2){
	
		dd1 = d1/100.0;
		dd2 = d2/100.0;
	}

	void set_lower_bound(const float& lower_bound){
		l_bound = lower_bound;
	}


	void update(const float& d1o, const float& d2o){
	
		d1_av = (d1+d1o*100)/2.0;
		d2_av = (d2+d2o*100)/2.0;
		y1 = y1 + rho*(d1-d1_av);
		y2 = y2 + rho*(d2-d2_av);
	}

private:

	bool check_feasibility(const float& d1_s, const float& d2_s){
		const float tol = 0.01;

		if(d1_s < 0)
			return false;
		if(d2_s > 100)
			return false;
		return (d1_s*K11/100 + d2_s*K12/100 > l_bound-o1-tol);
	}

	float evaluate_cost(const float& dd1, const float& dd2){
		return c1*dd1 + y1*(dd1-d1_av) + y2*(dd2-d2_av) + rho/2.0*(pow(dd1-d1_av, 2)+pow(dd2-d2_av,2));
	}

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

	// Consensus variables
	float d1, d2;
	float d1_av, d2_av;
	float y1, y2;
	float n;
	float l_bound;
	const float rho;
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
