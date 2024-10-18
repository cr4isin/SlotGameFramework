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
#include "tinyxml2.h"

using namespace std;

enum Colors {
	tLAVENDER = 141,
	tPURPLE = 5,
	tPINK = 198,
	tRED = 1,
	tORANGE = 202,
	tBROWN = 94,
	tGOLD = 3,
	tYELLOW = 226,
	tLIME = 118,
	tGREEN = 2,
	tTEAL = 36,
	tCYAN = 6,
	tBLUE = 4,
	tWHITE = 7,
	tGRAY = 8,
	tBLACK = 0,
	bLAVENDER = 397,
	bPURPLE = 261,
	bPINK = 454,
	bRED = 257,
	bORANGE = 458,
	bBROWN = 350,
	bGOLD = 259,
	bYELLOW = 482,
	bLIME = 374,
	bGREEN = 258,
	bTEAL = 292,
	bCYAN = 262,
	bBLUE = 260,
	bWHITE = 263,
	bGRAY = 264,
	bBLACK = 256,
};

void PrintVec(vector<int> input);

vector<int> ChangeBase(int input, int base, int length = 0, bool reversed = false);

void SpawnProcesses(string programName, int numTimes, int delayInSeconds = 0);

string FormatDouble(double value, int precision = -1);

string FormatInt(int value, int width = 1, char fill = ' ');

string FormatString(string value, int width = 1, char fill = ' ', bool alignLeft = true);

string FormatTime(long long milliseconds);

int GetMedian(const map<int, int>& hist);

int RandInt(int total);

set<int> SetIntersect(set<int> seta, set<int> setb);