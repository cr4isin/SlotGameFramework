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

void SpawnProcesses(string programName, int numTimes, int delayInSeconds = 0);

string FormatDouble(double value, int precision = -1);

string FormatInt(int value, int width = 1, char fill = ' ');

string FormatString(string value, int width = 1, char fill = ' ', bool alignLeft = true);

string FormatTime(long long milliseconds);

int GetMedian(const map<int, int>& hist);

int RandInt(int total);

set<int> SetIntersect(set<int> seta, set<int> setb);