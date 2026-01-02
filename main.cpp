#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <iomanip>

using namespace std;
using st = string;


struct CPUData {
	long long total, idle;
};

struct RAMData {
	long total, av;
};

CPUData getState() {
	ifstream file("/proc/stat");
	st cpuLabel;
	long long user, nice, system, idle, iowait, irq, softirq, steal;

	if (!(file >> cpuLabel >> user >> nice >> system >>  idle >> iowait >> irq >> softirq >> steal)){
		return {0, 0};
	}

	long long total = user + nice +system + idle + iowait + irq + softirq + steal;
	return {total, idle};
}

st getProcName(){
	ifstream file("/proc/cpuinfo");

	if (!file.is_open()){
		cerr << "Could not open /proc/cpuinfo";
		return "error";
	}

	st line;
	const st find = "model name";
	st procName = "NOT FOUND";
	while (getline(file, line)){
		if (line.find(find) != st::npos){
			procName = "";
			bool stat = false;
			for (char c: line){
				if (c == ':'){
					stat = true;
				}
				if (stat){
					procName+=c;
				}
			}
		}
	}
	return procName;
}

RAMData getRAMUsage(){
	ifstream file("/proc/meminfo");
	st key;
	long long value;
	long long total=0, availabel=0;

	while (file >> key >> value){
		if (key == "MemTotal:") total = value;
		else if (key == "MemAvailable:"){
			availabel = value;
			break;
		}
	}

	return {total, availabel};
}

int main(){
	CPUData prev = getState();
	const st procName = getProcName();
	cout << "Proc name" << procName << "\n";

	while (true){
		usleep(500000);

		CPUData curr = getState();
		
		long long diffTotal = curr.total - prev.total;
		long long diffIdle = curr.idle - prev.idle;

		double percentage = 0.0;
		if (diffTotal > 0) {
			percentage = 100.0 * (diffTotal - diffIdle)/diffTotal;
		}

		cout << "\rLoad CPU usage: " << fixed << setprecision(1) << percentage << "%  " << flush;
		prev = curr;

		RAMData ram = getRAMUsage();
		double used = (ram.total - ram.av) / 1024.0 / 1024.0;
		double total = ram.total / 1024.0 / 1024.0;
		cout << "\n";
		cout << "\rLoad RAM usage: " << fixed << setprecision(1) << used << " / " << total << " GB" << flush;
	}
	return 0;
}
