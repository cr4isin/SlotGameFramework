#pragma once

// General list of common assets that need to be included

#include <iomanip>
#include <regex>
#include "SlotTools.h"
#include "PrintTools.h"

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
