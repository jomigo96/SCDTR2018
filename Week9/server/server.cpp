#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
//#include <boost/asio.hpp>

#define DESKCOUNT 2

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::stringstream;

/* Struct for each desk considered
 * Contains the necessary stored variables
 * All cummulative variables are counted since the last restart */
typedef struct desk{
	int ID;
	double illuminance;
	double duty_cycle;
	double occupancy; // Boolean (type double to uniformize functions)
	double illuminance_lb; // Lower-bound
	double illuminance_bg; // Background
	double illuminance_ref; // Control reference
	double power;
	double time_acc;
	double energy_acc;
	double comfort_error_acc; // Accumulated comfort error
	double comfort_flicker_acc;
} desk;

/* Struct to assist the above struct in defining the identity of the message received */
enum MsgCode : uint8_t{
	calibration_request,
	data,
	cont,
	acknowledge,
	consensus_data,
	consensus_stop,
	sampling_time_data,
	none,
};

/* Message struct defined in the Arduino code
 * This should be the input from I2C */
typedef struct msg{
	MsgCode code;		// 1 byte
	uint8_t address;	// 1 byte
	uint8_t aux1;		// 1 byte
	uint8_t aux2;		// 1 byte
	float value[4];		// 4 x 4 = 16 bytes
}message_t;				//size fixed to 20 bytes

/* Main server class
 * Contains desk data in a vector of desk structs (defined in this file)
 * Contains valid subsets for input arguments */
class Server {
	private:
		vector<char> command_val = {'g','r','b','s'};
		vector<char> g_val = {'l','d','s','L','o','r','p','t','e','c','v'};
		vector<char> bs_val = {'l','d'};

		double g_desk(char, int);
		desk get_total();
	public:
		vector<desk> desks;
		desk desk1;

		void init_desks();
		string clmessage(string);
		void desk_input(message_t);
};

/* Initializes desk structs with arbitrary values
 * Mainly for testing */
void Server::init_desks(){
	desk new_desk;

	/* Create DESKCOUNT desks with the same initial values */
	for(int i = 0; i < DESKCOUNT; i++){
		new_desk.ID = i + 1;
		new_desk.illuminance = 500;
		new_desk.duty_cycle = 1;
		new_desk.occupancy = 0;
		new_desk.illuminance_lb = 100;
		new_desk.illuminance_bg = 50;
		new_desk.illuminance_ref = 400;
		new_desk.power = 3;
		new_desk.time_acc = 0;
		new_desk.energy_acc = 0;
		new_desk.comfort_error_acc = 0;
		new_desk.comfort_flicker_acc = 0;
		desks.push_back(new_desk);
	}
}

/* Receive string command, return string message
 * Currently returns only success of failure messages
 * To be fixed */
string Server::clmessage(string message_in){
	vector<string> args;
	vector<char> command = {'a','a'};
	string buff;
	stringstream message_stream(message_in);

	int command_desk;
	double res_val;

	// Grab separate arguments from input message as a vector of strings
	while(message_stream >> buff)
		args.push_back(buff);

	// Check for number of arguments
	if(args.size() > 3 || args.size() < 1)
		return "error";

	// Check that command is a string of one character and converts to char
	if(args[0].size() == 1){
		const char *c = args[0].c_str();
		command[0] = c[0];
	} else
		return "error - c";

	// Checks for commands with arguments
	if(args.size() > 1){
		// Check that first argument is a string of one character and converts to char
		if(args[1].size() == 1){
			const char *d = args[1].c_str();
			command[1] = d[0];
		} else
			return "error - arg";

		// Check if last argument is a valid desk
		// Use 0 as the equivalent of T (so all desks instead of just one)?
		command_desk = std::stoi(args[2], nullptr, 10);
		if(command_desk < 0 && command_desk > 2)
			return "error - desk";
	}

	// Check if arguments is valid (i.e. belong to a list of valid arguments)
	if(std::find(command_val.begin(), command_val.end(), command[0]) != command_val.end()){
		if(args.size() != 3){
			if(command[0] == 'r'){
				return "ack";
				// Enter value reset and calibration code possibly method
			} else
				return "error - arg nums";
		} else {
			//////////////////////////////////////////////
			// Enter desk ID validity (argument 2) here //
			//////////////////////////////////////////////
			switch(command[0]){
				case 'g':
					if(std::find(g_val.begin(), g_val.end(), command[1]) != g_val.end()){
						res_val = g_desk(command[1], command_desk - 1);
						
						// Format output string for g commands
						std::ostringstream output;
						output << command[1] << ' ' << command_desk << ' ' << res_val;
						return output.str();
					}
					break;
				case 'b':
					return "b <x> <i>";
					// Enter last minute buffer code or method
					break;
				case 's':
					return "ack";
					// Enter stop stream code or method
					break;
			}
		}
	}
	else
		return "error - c";
	
	return "error - unknown";
}

/* Method to obtain and return the data corresponding to any
 * g commands provided by client */
double Server::g_desk(char arg, int desk){
	switch(arg){
		case 'l':
			return desks[desk].illuminance;
		case 'd':
			return desks[desk].duty_cycle;
		case 's':
			return desks[desk].occupancy;
		case 'L':
			return desks[desk].illuminance_lb;
		case 'o':
			return desks[desk].illuminance_bg;
		case 'r':
			return desks[desk].illuminance_ref;
		case 'p':
			return desks[desk].power;
		case 't':
			return desks[desk].time_acc;
		case 'e':
			return desks[desk].energy_acc;
		case 'c':
			return desks[desk].comfort_error_acc;
		case 'v':
			return desks[desk].comfort_flicker_acc;
		default:
			return 0;
	}
}

/* Method to return a desk struct with the totals of all saved desk structs */
desk Server::get_total(){
	desk desk_total;
	for(auto i : desks){
		desk_total.power += i.power;
		desk_total.energy_acc += i.energy_acc;
		desk_total.comfort_error_acc += i.comfort_error_acc;
		desk_total.comfort_flicker_acc += i.comfort_flicker_acc;
	}

	return desk_total;
}

/* Method to parse an input message from an Arduino desk source */
void Server::desk_input(message_t message_in){
	//cout << "Hello!" << endl;
}

int main(void){
	string message_in("g l 2");

	Server server1;

	server1.init_desks();
	cout << server1.clmessage(message_in) << endl;

	return 0;
}
