
#include "SlotGame.h"


// ==================================== Grid Dimensions ==================================== 
int SlotGame::numReels = 5;
int SlotGame::numRows = 3;
int SlotGame::numFreeGames[6] = { 0,0,0,10,15,20 };

// ==================================== Reels ==================================== 
vector<vector<int>> SlotGame::baseReels =
{
	{Plum, Bell, Boat, Cherry, Melon, Boat, Orange, Bell, Melon, Car, Orange, Melon, Cherry, Diamond, Melon, Cherry, Boat, Orange, Car, Plum, Boat, Glass, Orange, Lobster, Plum, Melon, Lobster, Cherry, Bell, Orange, Melon, Bell, Cherry, Lobster, Plum, Car, Bell, Coin, Lobster, Boat, Cherry, Melon, Lobster, Glass, Plum, Bell, Boat, Cherry, Melon, Coin, Orange, Bell, Melon, Car, Orange, Melon, Cherry, Diamond, Melon, Cherry, Boat, Orange, Car, Plum, Cherry, Glass, Orange, Lobster, Plum, Melon, Lobster, Cherry, Bell, Orange, Melon, Bell, Cherry, Lobster, Plum, Car, Bell, Plum, Lobster, Boat, Cherry, Melon, Lobster, Glass},
	{Orange, Bell, Diamond, Orange, Bell, Coin, Car, Orange, Lobster, Melon, Orange, Plum, Bell, Car, Orange, Glass, Melon, Plum, Car, Glass, Orange, Lobster, Melon, Plum, Glass, Bell, Boat, Orange, Bell, Boat, Orange, Bell, Coin, Car, Orange, Lobster, Melon, Orange, Plum, Bell, Cherry, Orange, Glass, Melon, Orange, Car, Glass, Orange, Lobster, Boat, Orange, Glass, Bell, Cherry},
	{Melon, Lobster, Plum, Diamond, Lobster, Plum, Glass, Bell, Boat, Plum, Car, Boat, Glass, Lobster, Melon, Glass, Lobster, Plum, Diamond, Lobster, Plum, Orange, Boat, Car, Bell, Cherry, Boat, Melon, Cherry, Lobster, Car, Bell, Plum, Melon, Lobster, Orange, Melon, Lobster, Orange, Boat, Glass, Melon, Lobster, Car, Orange, Lobster, Plum, Orange, Bell, Boat, Cherry, Car, Glass, Boat, Cherry, Coin, Lobster, Bell, Plum, Cherry, Glass, Melon, Orange, Cherry, Car, Bell, Cherry, Lobster, Plum, Diamond, Lobster, Plum, Bell, Cherry, Melon, Lobster, Boat, Melon, Lobster, Orange, Plum},
	{Orange, Bell, Diamond, Orange, Bell, Coin, Car, Orange, Lobster, Boat, Orange, Plum, Bell, Car, Orange, Glass, Melon, Plum, Car, Glass, Orange, Lobster, Boat, Orange, Glass, Bell, Cherry, Orange, Bell, Car, Orange, Bell, Coin, Car, Orange, Lobster, Melon, Orange, Plum, Bell, Cherry, Orange, Glass, Melon, Orange, Car, Glass, Orange, Lobster, Melon, Orange, Glass, Bell, Boat, Orange, Bell, Diamond, Orange, Bell, Coin, Car, Orange, Lobster, Boat, Orange, Plum, Bell, Boat, Orange, Glass, Melon, Orange, Car, Glass, Cherry, Lobster, Boat, Orange, Glass, Bell, Cherry, Melon, Bell, Car, Orange, Bell, Coin, Car, Orange, Lobster, Melon, Orange, Plum, Bell, Cherry, Orange, Glass, Melon, Orange, Car, Glass, Orange, Lobster, Melon, Orange, Glass, Bell, Boat},
	{Plum, Bell, Boat, Cherry, Melon, Boat, Orange, Bell, Melon, Car, Orange, Melon, Cherry, Diamond, Melon, Cherry, Boat, Orange, Car, Plum, Cherry, Glass, Orange, Lobster, Plum, Melon, Lobster, Cherry, Bell, Orange, Melon, Bell, Cherry, Lobster, Plum, Car, Bell, Coin, Lobster, Boat, Cherry, Melon, Lobster, Glass, Plum, Bell, Boat, Cherry, Melon, Coin, Orange, Bell, Melon, Car, Orange, Melon, Cherry, Diamond, Melon, Cherry, Boat, Orange, Car, Plum, Cherry, Glass, Orange, Lobster, Plum, Melon, Lobster, Cherry, Bell, Orange, Melon, Bell, Cherry, Lobster, Plum, Car, Bell, Plum, Lobster, Boat, Cherry, Melon, Lobster, Glass},
};

vector<vector<int>> SlotGame::baseReelWeights =
{
	{ BLANK },
	{ BLANK },
	{ BLANK },
	{ BLANK },
	{ BLANK },
};

vector<vector<int>> SlotGame::freeReels =
{
	{ BLANK },
	{ BLANK },
	{ BLANK },
	{ BLANK },
	{ BLANK },
};

vector<vector<int>> SlotGame::freeReelWeights =
{
	{ BLANK },
	{ BLANK },
	{ BLANK },
	{ BLANK },
	{ BLANK },
};

// ==================================== Symbol Combo Info ==================================== 
map<int, set<int>> SlotGame::symbolSubstitutions =
{ // Symbols not listed here will be set to only map to themselves: {X, {X}}
	{BLANK, {}},
	{Diamond, {Diamond, Boat, Car, Glass, Lobster, Bell, Plum, Melon, Orange, Cherry}},
	{Coin, {}},
};

map<int, int> SlotGame::symbolMultipliers =
{ // Symbols not included will be set to a multiplier of 1: {X, 1}
	{Diamond, 2},
};

map<int, vector<double>> SlotGame::paytable =
{ // Symbols not listed here will be set to have zero pays: {X, {0,0,0,0,0}}
	{Diamond,{0, 0, 40, 100, 2000}},
	{Boat,{0, 0, 20, 60, 750}},
	{Car,{0, 0, 16, 50, 500}},
	{Glass,{0, 0, 12, 40, 500}},
	{Lobster,{0, 0, 8, 25, 250}},
	{Bell,{0, 0, 8, 25, 250}},
	{Plum,{0, 0, 5, 15, 150}},
	{Melon,{0, 0, 5, 15, 150}},
	{Orange,{0, 0, 5, 10, 100}},
	{Cherry,{0, 0, 5, 10, 100}},
	{Coin, {0, 0, 2, 5, 20, 200}},
};

map<int, string> SlotGame::symbolStrings =
{
	{BLANK, "--"},
	{Diamond,"Diamond"},
	{Boat,"Boat"},
	{Car,"Car"},
	{Glass,"Glass"},
	{Lobster,"Lobster"},
	{Bell,"Bell"},
	{Plum,"Plum"},
	{Melon,"Melon"},
	{Orange,"Orange"},
	{Cherry,"Cherry"},
	{Coin,"Coin"},
};


// ==================================== Lines ==================================== 
vector<vector<int>> SlotGame::CustomLines =
{
	{1,1,1,1,1},
	{0,0,0,0,0},
	{2,2,2,2,2},
	{0,1,2,1,0},
	{2,1,0,1,2},
	{1,0,0,0,1},
	{1,2,2,2,1},
	{0,0,1,2,2},
	{2,2,1,0,0},
	{1,0,1,0,1},
	{1,2,1,2,1},
	{0,1,1,1,2},
	{2,1,1,1,0},
	{1,1,0,1,2},
	{1,1,2,1,0},
	{0,1,0,1,0},
	{2,1,2,1,2},
	{0,0,2,0,0},
	{2,2,0,2,2},
	{1,0,2,0,1},
	{1,2,0,2,1},
	{0,2,0,2,0},
	{2,0,2,0,2},
	{0,2,2,2,0},
	{2,0,0,0,2},
	{0,2,1,2,0},
	{2,0,1,0,2},
	{1,1,2,1,1},
	{1,1,0,1,1},
	{0,2,0,1,1}
};