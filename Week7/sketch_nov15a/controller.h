#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Arduino.h"


class Controller{

public:
		Controller() : 
			b(5.8515),
			m(-0.9355),
			Raux(100000),
			C(1e-6){}

		Controller(float b, float m, float Raux, float C) : 
			b(b),
			m(m),
			Raux(Raux),
			C(C){}

		float compute_lux(int s){

			float v, R;
		
			v = s * 5.0/1.0230;
			R = (5-v/1000.0)/(v/1000.0/Raux);
			return pow(10, (log10(R)-b)/m );

		}

private:

	float b;
	float m;
	float Raux;
	float C;
};

#endif
