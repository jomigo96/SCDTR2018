#ifndef DATA_STRUCTURES_HPP
#define DATA_STRUCTURES_HPP

typedef struct desk{
	double illuminance; // Current illuminance in LUX
	double duty_cycle; // Current LED applied duty_cycle, from 0 to 1
	int occupancy; // ocupancy state, 0-empty 1-occupied
	double illuminance_lb; // Current illuminance lower bound
	double illuminance_bg; // Background measured at the last calibration
	double illuminance_ref; // Illuminance control reference
	double power; // Instantaneous power consumption
	uint32_t time_acc; // Timestamp of the last restart
	double energy_acc; // Accumulated energy
	double comfort_error_acc; // Accumulated comfort error
	double comfort_flicker_acc; // Accumulated flicker error

	uint32_t comf_err_samples; // Samples where there was confort error
	uint32_t samples;
	double l_prev; // Illuminance in the i-1 previous sample
	double l_pprev; // Illuminance in the i-2 previous sample
} desk_t;

enum MsgCode : uint8_t{

	calibration_request,
	data,
	cont,
	acknowledge,
	consensus_data,
	consensus_stop,
	consensus_new,
	sampling_time_data,
	none,

};

typedef struct msg{
	MsgCode code; // 1 byte
	uint8_t address; // 1 byte
	uint8_t aux1; // 1 byte
	uint8_t aux2; // 1 byte
	float value[4]; // 4 x 4 = 16 bytes
}message_t; //size fixed to 20 bytes

#endif
