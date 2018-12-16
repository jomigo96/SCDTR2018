#ifndef DATA_STRUCTURES_HPP
#define DATA_STRUCTURES_HPP

typedef struct desk{
	double illuminance;
	double duty_cycle;
	int occupancy; // Boolean (type double to uniformize functions)
	double illuminance_lb; // Lower-bound
	double illuminance_bg; // Background
	double illuminance_ref; // Control reference
	double power;
	double time_acc;
	double energy_acc;
	double comfort_error_acc; // Accumulated comfort error
	double comfort_flicker_acc;
} desk_t;



#endif
