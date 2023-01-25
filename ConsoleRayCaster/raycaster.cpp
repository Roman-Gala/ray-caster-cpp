#include <iostream>
#include <Windows.h>

#include "ConsoleEngine.h"

using namespace std;

int main() {
	ConsoleEngine game(250, 100);

	int x = 1000000;
	while (x) {
		game.camRotation();
		game.calcRays();
		game.writeConsole();
		Sleep(5);
	}
	return 0;
}
