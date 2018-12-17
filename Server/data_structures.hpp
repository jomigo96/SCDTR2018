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
	uint32_t time_acc;
	double energy_acc;
	double comfort_error_acc; // Accumulated comfort error
	double comfort_flicker_acc;
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
