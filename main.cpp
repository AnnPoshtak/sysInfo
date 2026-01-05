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
	long long  total, av;
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
	st key,kb;
	long long value;
	long long total=0, availabel=0;

	while (file >> key >> value >> kb){
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

		RAMData ram = getRAMUsage();
		double used = 100.0 *  (ram.total - ram.av) / ram.total;

		cout << "\rLoad CPU usage: " << fixed << setprecision(1) << percentage << "%" << "||| RAM: "<< fixed << setprecision(1) << used << "%" <<flush;
		prev = curr;
	}
	return 0;
}
