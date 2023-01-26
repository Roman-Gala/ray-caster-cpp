#include <iostream>
#include <Windows.h>

#include "ConsoleEngine.h"

using namespace std;

int main() {

	// Create ConsoleEngine object
	ConsoleEngine sim(250, 80);

	// Run the engine
	sim.runEngine();

	return 0;
}
