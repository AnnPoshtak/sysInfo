#include <iostream>
#include <fstream>
#include <string>

using namespace std;
using st = string;

int main(){
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
	return 0;
}
