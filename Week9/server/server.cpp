#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include <sstream>
//#include <boost/asio.hpp>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::stringstream;

typedef struct desk{
	// All cummulative variables counted since last restart
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

class Server {
	private:
		vector<desk> desks;
		int check_args(vector<string>);
		void init_desks(); // Placeholder with example initial values
		double g_desk(char, int);
	public:
		string clmessage(string);
};

void Server::init_desks(){
	desks[0].illuminance = 500;
	desks[0].duty_cycle = 1;
	desks[0].occupancy = 0;
	desks[0].illuminance_lb = 100;
	desks[0].illuminance_bg = 50;
	desks[0].illuminance_ref = 400;
	desks[0].power = 3;
	desks[0].time_acc = 0;
	desks[0].energy_acc = 0;
	desks[0].comfort_error_acc = 0;
	desks[0].comfort_flicker_acc = 0;

	desks[1].illuminance = 500;
	desks[1].duty_cycle = 1;
	desks[1].occupancy = 0;
	desks[1].illuminance_lb = 100;
	desks[1].illuminance_bg = 50;
	desks[1].illuminance_ref = 400;
	desks[1].power = 3;
	desks[1].time_acc = 0;
	desks[1].energy_acc = 0;
	desks[1].comfort_error_acc = 0;
	desks[1].comfort_flicker_acc = 0;
}

int Server::check_args(vector<string> args){
	if(args.size() > 3 || args.size() < 1)
		return 1;
	return 0;
}

string Server::clmessage(string message_in){
	vector<string> args;
	string buff;
	stringstream message_stream(message_in);

	while(message_stream >> buff)
		args.push_back(buff);

	if(check_args(args) == 1){
		cout << "Error processing arguments." << endl;
		return "Error processing arguments.";
	}

	return "Success returning arguments!";
}

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
			return 0; // Should other function handle unavailable args?
	}
}

int main(void){
	string message_in("g l 500");

	Server command1;

	cout << command1.clmessage(message_in) << endl;

	return 0;
}
