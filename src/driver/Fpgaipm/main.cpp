#include <iostream>
//#include <pthread.h>
//#include <semaphore.h>
#include "Fpgaipm.h"
using namespace std;

int main(int argc, char **argv) {

	Fpgaipm fpga;
	cout << fpga.init() << endl;
	cout << fpga.fpga_openTransaction() << endl;
	cout << fpga.fpga_openTransaction() << endl;

	cout << fpga.fpga_closeTransaction() << endl;
	return 0;
}
