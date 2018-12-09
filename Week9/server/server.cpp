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

/* Main server class
 * Contains desk data in a vector of desk structs (defined in this file)
 * Contains valid subsets for input arguments */
class Server {
	private:
		vector<char> command_val = {'g','r','b','s'};
		vector<char> g_val = {'l','d','s','L','o','r','p','t','e','c','v'};

		double g_desk(char, int);
	public:
		vector<desk> desks;
		desk desk1;

		void init_desks(); // Placeholder with example initial values
		string clmessage(string);
};

/* Initializes desk structs with arbitrary values
 * Mainly for testing */
void Server::init_desks(){
	desk new_desk;

	for(int i = 0; i < DESKCOUNT; i++){
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
		return "Error processing arguments.";

	// Check size of first two arguments and convert them to chars
	if(args[0].size() == 1){
		const char *c = args[0].c_str();
		command[0] = c[0];
	} else
		return "Error processing arguments.";
	if(args[1].size() == 1){
		const char *d = args[1].c_str();
		command[1] = d[0];
	} else
		return "Error processing arguments.";

	// Check if last argument is a valid desk
	command_desk = std::stoi(args[2], nullptr, 10);
	if(command_desk <= 0 && command_desk > 2)
		return "Invalid desk ID.";

	// Check if first argument is valid (i.e. belongs to a list of valid arguments)
	if(std::find(command_val.begin(), command_val.end(), command[0]) != command_val.end()){
		switch(command[0]){
			case 'g':
				if(std::find(g_val.begin(), g_val.end(), command[1]) != g_val.end()){
					res_val = g_desk(command[1], command_desk - 1);
				}
		}
	}
	else
		return "Error processing arguments.";


	return "Success returning arguments!";
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

int main(void){
	string message_in("g l 2");

	Server server1;

	server1.init_desks();
	cout << server1.clmessage(message_in) << endl;

	return 0;
}
