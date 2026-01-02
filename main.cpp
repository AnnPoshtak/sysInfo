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

int main(){
	CPUData prev = getState();
	ifstream file("/proc/cpuinfo");

	if (!file.is_open()){
		cerr << "Could not open /proc/cpuinfo";
		return 1;
	}



	st line;
	const st findProcName = "model name";
	st procName = "NOT FOUND";
	while (getline(file, line)){
		if (line.starts_with(findProcName)){
			procName = line;
			break;
		}
	}

	cout << procName << "\n";

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
	}
	return 0;






	return 0;
}
