#include <iostream>
#include <Windows.h>

#include "ConsoleEngine.h"

using namespace std;

int main() {
	ConsoleEngine game(250, 80);

	bool run = true;
	while (run) {
		try {
			game.camMovement();
			game.calcRays();
			game.writeConsole();
			Sleep(1);
		}
		catch (EndGame) {
			run = false;
		}
	}
	return 0;
}
