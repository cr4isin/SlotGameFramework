#pragma once

// General list of common assets that need to be included

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <random>
#include <Windows.h>
#include <thread>
#include <chrono>
#include <iomanip>

using namespace std;

enum Colors {
	tBLACK = 30,
	tRED,
	tGREEN,
	tYELLOW,
	tBLUE,
	tMAGENTA,
	tCYAN,
	tWHITE,
	bBLACK = 40,
	bRED,
	bGREEN,
	bYELLOW,
	bBLUE,
	bMAGENTA,
	bCYAN,
	bWHITE,
};

void PrintVec(vector<int> input);

vector<int> ChangeBase(int input, int base, int length = 0);

void SpawnProcesses(const char* programName, int numTimes, int delayInSeconds = 0);