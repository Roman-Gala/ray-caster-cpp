#pragma once
#ifndef CONSOLE_ENGINE
#define CONSOLE_ENGINE

#include <iostream>
#include <Windows.h>

using namespace std;

#define MAP_SIZE 20
#define KEY_W 0x57
#define KEY_S 0x53
#define KEY_Q 0x51
#define KEY_A 0x41
#define KEY_D 0x44

class EndGame : public exception {
public:
	static const char* what() { return "Quit."; }
};

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

	// Fading
	static const char fade[21];

public:
	// Constructors, destructor
	ConsoleEngine() : ConsoleEngine(100, 30) {}
	ConsoleEngine(int width, int height);
	~ConsoleEngine() { delete screenBuffer; }

	// Run the engine
	void runEngine();

	// Renderin and calculation
	void writeConsole();
	void calcRays();

	// Init
	void initMap();
	void initMap(wstring loadMap) { initMap(); }               // Not implemented

	// Movement
	void camMovement();
	void moveFor(float (&moVec)[2]);
	void moveBack(float (&moVec)[2]);
	void moveLeft(float (&moVec)[2]);
	void moveRight(float (&moVec)[2]);
};

// --- Fading ---
const char ConsoleEngine::fade[21] = { '@', '@', '@', '&', 'W', '0', '$', '%', '8', 'B', '#',
'm', 'a', 'o', 'c', ':', '"', '\'', '\'', '.', '.' };


// ========================== DEFINITIONS ==============================

// --- Constructors ---

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
	SetConsoleTitle(L"Ray Caster");
}

// --- Run the engine ---

void ConsoleEngine::runEngine() {
	bool run = true;
	while (run) {
		try {
			camMovement();
			calcRays();
			writeConsole();
			Sleep(1);
		}
		catch (EndGame exc) {
			cout << exc.what();
			run = false;
		}
	}
}

// --- Rendering and calculation ---

void ConsoleEngine::writeConsole() {
	//screenBuffer[screenWidth * screenHeight - 1] = '\0';         // Escape character at the end
	WriteConsoleOutputCharacter(consoleHandle, screenBuffer, screenWidth * screenHeight, { 0,0 }, &bytesWritten);
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
			test[0] = (camX + testVect[0] * distance);
			test[1] = (camY + testVect[1] * distance);

			if (map[MAP_SIZE * test[1] + test[0]] == 'O') {
				hit = true;
			}
		}

		if (hit) {
			// Compensate for fisheye effect
			distance = distance * cosf(fabs(camAng - rayAng));
			// Calculate the perspective difference
			perspective = (int)(screenHeight / 2.0f - screenHeight / (float)distance * 2.0f);
			// Fill buffer column
			for (int j = 0; j < screenHeight; j++) {
				if (j < perspective || j > screenHeight - perspective) {
					screenBuffer[j * screenWidth + i] = ' ';
				}
				else {
					screenBuffer[j * screenWidth + i] = fade[(int)distance];
				}
			}
		}
		else {
			// Fill buffer column with spaces
			for (int j = 0; j < screenHeight; j++) {
				screenBuffer[j * screenWidth + i] = ' ';
			}
		}
	}
}

// --- Movement ---

void ConsoleEngine::camMovement() {
	// Check key input and boundries
	float moVec[2];
	if (GetAsyncKeyState(VK_LEFT)) {
		camAng -= 0.02f;
	}
	if (GetAsyncKeyState(VK_RIGHT)) {
		camAng += 0.02f;
	}
	if (GetAsyncKeyState(KEY_W)) {
		moveFor(moVec);
	}
	if (GetAsyncKeyState(KEY_S)) {
		moveBack(moVec);
	}
	if (GetAsyncKeyState(KEY_A)) {
		moveLeft(moVec);
	}
	if (GetAsyncKeyState(KEY_D)) {
		moveRight(moVec);
	}
	if (GetAsyncKeyState(KEY_Q)) {
		throw EndGame();
	}
}

void ConsoleEngine::moveFor(float (&moVec)[2]) {
	moVec[0] = sinf(camAng) * 0.04f;
	moVec[1] = cosf(camAng) * 0.04f;
	if (map[(int)round(camY + moVec[1]) * MAP_SIZE + (int)round(camX + moVec[0])] != 'O') {
		camX += moVec[0];
		camY += moVec[1];
	}
}

void ConsoleEngine::moveBack(float(&moVec)[2]) {
	moVec[0] = sinf(camAng) * 0.04f;
	moVec[1] = cosf(camAng) * 0.04f;
	if (map[(int)round(camY - moVec[1]) * MAP_SIZE + (int)round(camX - moVec[0])] != 'O') {
		camX -= moVec[0];
		camY -= moVec[1];
	}
}

void ConsoleEngine::moveLeft(float(&moVec)[2]) {
	moVec[0] = cosf(camAng) * 0.04f;
	moVec[1] = -sinf(camAng) * 0.04f;
	if (map[(int)round(camY - moVec[1]) * MAP_SIZE + (int)round(camX - moVec[0])] != 'O') {
		camX -= moVec[0];
		camY -= moVec[1];
	}
}

void ConsoleEngine::moveRight(float(&moVec)[2]) {
	moVec[0] = cosf(camAng) * 0.04f;
	moVec[1] = -sinf(camAng) * 0.04f;
	if (map[(int)round(camY + moVec[1]) * MAP_SIZE + (int)round(camX + moVec[0])] != 'O') {
		camX += moVec[0];
		camY += moVec[1];
	}
}

// --- Init ---

void ConsoleEngine::initMap() {
	map += L"OOOOOOOOOOOOOOOOOOOO";
	map += L"O                  O";
	map += L"O                  O";
	map += L"O                  O";
	map += L"O                  O";
	map += L"OOOOOOO     OOOOOOOO";
	map += L"O                  O";
	map += L"O                  O";
	map += L"O                  O";
	map += L"O                  O";
	map += L"O           O      O";
	map += L"O                  O";
	map += L"O                  O";
	map += L"O    O             O";
	map += L"O                  O";
	map += L"O           O      O";
	map += L"O                  O";
	map += L"O                  O";
	map += L"O                  O";
	map += L"OOOOOOOOOOOOOOOOOOOO";
}

#endif