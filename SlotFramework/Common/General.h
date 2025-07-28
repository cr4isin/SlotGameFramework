#pragma once

// General list of common assets that need to be included

#include <iostream>
#include <string>
#include <sstream>
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
#include <regex>
#include "ANSI.h"
#include "SlotTools.h"

using namespace std;

struct Combo
{
	double pay = 0;
	double basePay = 0;
	double multiplier = 1;
	int bonusCode = 0;
	int progressive = 0;
	int length = 0;
	bool set = false;
};

void PrintPauseMessage(string message, bool clearAfter = false);

void SpawnProcesses(string programName, int numTimes, int delayInSeconds = 0);

string FormatDouble(double value, int precision = -1);

string FormatInt(int value, int width = 1, char fill = ' ');

string FormatString(string value, int width = 1, char fill = ' ', bool alignLeft = true);

string FormatTime(long long milliseconds);

void WriteHistogramToFile(string filename, map<double, long long> hist); 

void WriteStringToFile(string filename, string text);