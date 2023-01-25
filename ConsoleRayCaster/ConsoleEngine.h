#pragma once
#ifndef CONSOLE_ENGINE
#define CONSOLE_ENGINE

#include <iostream>
#include <Windows.h>

using namespace std;

#define MAP_SIZE 20

class ConsoleEngine {
protected:
	// Screen
	wchar_t* screenBuffer;                     // Buffer array
	int screenWidth;                           // Console render width
	int screenHeight;                          // Console render height
	HANDLE consoleHandle;                      // Handle for the console
	DWORD bytesWritten;                        // Written bytes

	// Camera
	float camX;                                // Camera X coordinate
	float camY;                                // Camery Y coordinate
	float camAng;                              // Camera angle
	float camFov;                              // Camera field of view
	float renderDistance;

	// Map
	wstring map;

public:
	ConsoleEngine() : ConsoleEngine(100, 30) {}
	ConsoleEngine(int width, int height);

	void writeConsole();
	void calcRays();
	void initMap();
	void camRotation();
};

ConsoleEngine::ConsoleEngine(int width, int height) {
	// Init members
	screenWidth = width;
	screenHeight = height;
	camX = 2.0f;
	camY = 2.0f;
	camAng = 0.0f;
	camFov = 0.9f;
	renderDistance = 20.0f;

	initMap();

	// Create screen buffer
	screenBuffer = new wchar_t[screenWidth * screenHeight];
	consoleHandle = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(consoleHandle);
	bytesWritten = 0;
}

void ConsoleEngine::writeConsole() {
	screenBuffer[screenWidth * screenHeight - 1] = '\0';         // Escape character at the end
	WriteConsoleOutputCharacter(consoleHandle, screenBuffer, screenWidth * screenHeight, { 0,0 }, &bytesWritten);
}

void ConsoleEngine::camRotation() {
	float moVec[2];
	if (GetAsyncKeyState((unsigned short)'A') & 0x8000) {
		camAng -= 0.02f;
	}
	if (GetAsyncKeyState((unsigned short)'D') & 0x8000) {
		camAng += 0.02f;
	}
	if (GetAsyncKeyState((unsigned short)'W') & 0x8000) {
		moVec[0] = sinf(camAng) * 0.04f;
		moVec[1] = cosf(camAng) * 0.04f;
		camX += moVec[0];
		camY += moVec[1];
	}
	if (GetAsyncKeyState((unsigned short)'S') & 0x8000) {
		moVec[0] = sinf(camAng) * 0.04f;
		moVec[1] = cosf(camAng) * 0.04f;
		camX -= moVec[0];
		camY -= moVec[1];
	}
}

void ConsoleEngine::calcRays() {
	float rayAng;
	float distance;
	int test[2];                // [0] - x, [1] - y
	float testVect[2];          // [0] - x, [1] - y
	bool hit;
	int perspective;

	for (int i = 0; i < screenWidth; i++) {
		rayAng = camAng - camFov / 2 + (float)i / (float)screenWidth * camFov;
		testVect[0] = sinf(rayAng);
		testVect[1] = cosf(rayAng);
		distance = 0;
		hit = false;

		while (!hit && distance <= renderDistance) {
			distance += 0.01f;
			test[0] = (int)(camX + testVect[0] * distance);
			test[1] = (int)(camY + testVect[1] * distance);

			if (test[0] < 0 || test[0] >= MAP_SIZE || test[1] < 0 || test[1] >= MAP_SIZE) {
				hit = true;
				distance = renderDistance;
			}
			else {
				if (map[MAP_SIZE * test[1] + test[0]] == 'O') {
					hit = true;
				}
			}
		}

		distance = distance * cosf(fabs(camAng - rayAng));
		perspective = (int)(screenHeight / 2.0f - screenHeight / (float)distance);
		for (int j = 0; j < screenHeight; j++) {
			if (j < perspective || j > screenHeight - perspective) {
				screenBuffer[j * screenWidth + i] = ' ';
			}
			else {
				if (distance < 2) {
					screenBuffer[j * screenWidth + i] = '@';
				}
				else if (distance < 3) {
					screenBuffer[j * screenWidth + i] = '&';
				}
				else if (distance < 4) {
					screenBuffer[j * screenWidth + i] = 'W';
				}
				else if (distance < 5) {
					screenBuffer[j * screenWidth + i] = '0';//W
				}
				else if (distance < 6) {
					screenBuffer[j * screenWidth + i] = '$';//0
				}
				else if (distance < 7) {
					screenBuffer[j * screenWidth + i] = '%';//#
				}
				else if (distance < 8) {
					screenBuffer[j * screenWidth + i] = '8';//B
				}
				else if (distance < 9) {
					screenBuffer[j * screenWidth + i] = 'B';//8
				}
				else if (distance < 10) {
					screenBuffer[j * screenWidth + i] = '#';//%
				}
				else if (distance < 11) {
					screenBuffer[j * screenWidth + i] = 'm';
				}
				else if (distance < 12) {
					screenBuffer[j * screenWidth + i] = 'a';
				}
				else if (distance < 13) {
					screenBuffer[j * screenWidth + i] = 'o';
				}
				else if (distance < 14) {
					screenBuffer[j * screenWidth + i] = 'c';
				}
				else if (distance < 15) {
					screenBuffer[j * screenWidth + i] = ':';
				}
				else if (distance < 16) {
					screenBuffer[j * screenWidth + i] = '"';
				}
				else if (distance < 17) {
					screenBuffer[j * screenWidth + i] = '\'';
				}
				else if (distance < 18) {
					screenBuffer[j * screenWidth + i] = '\'';
				}
				else {
					screenBuffer[j * screenWidth + i] = '\.';
				}
			}
		}
	}

}

void ConsoleEngine::initMap() {
	map += L"OOOOOOOOOOOOOOOOOOOO";
	map += L"O                  O";
	map += L"O                  O";
	map += L"O                  O";
	map += L"O                  O";
	map += L"OOOOOOO     OOOOOOOO";
	map += L"O     O            O";
	map += L"O     O            O";
	map += L"O     O            O";
	map += L"O     O            O";
	map += L"O     OOOOOOOO     O";
	map += L"O            O     O";
	map += L"OOOOOOOOOOOOOO     O";
	map += L"O                  O";
	map += L"O                  O";
	map += L"O                  O";
	map += L"O                  O";
	map += L"O                  O";
	map += L"O                  O";
	map += L"OOOOOOOOOOOOOOOOOOOO";
}

#endif