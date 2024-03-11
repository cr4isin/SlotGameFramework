
#include "SlotGame.h"

// ==================================== Grid Dimensions & Symbol Combo Info ==================================== 
int SlotGame::numReels = 5;
int SlotGame::numRows = 3;

map<int, set<int>> SlotGame::symbolSubstitutions =
{ // Symbols not listed here will be set to only map to themselves: {X, {X}}
	{BLANK, {}},
	{WILD, {WILD, S01, S02, S03, S04, S05, S06, S07, S08, S09}},
	{BONUS, {}}, // BONUS does not contribute to line pays
};

map<int, int> SlotGame::symbolMultipliers =
{ // Symbols not included will be set to a multiplier of 1: {X, 1}

};

map<int, vector<double>> SlotGame::paytable =
{ // Symbols not listed here will be set to have zero pays: {X, {0,0,0,0,0}}
	{WILD, {0, 0, 100, 750, 2500}},
	{S01, {0, 0, 50, 250, 750}},
	{S02, {0, 0, 50, 250, 750}},
	{S03, {0, 0, 25, 100, 500}},
	{S04, {0, 0, 25, 100, 400}},
	{S05, {0, 0, 25, 75, 250}},
	{S06, {0, 0, 5, 25, 125}},
	{S07, {0, 0, 5, 25, 100}},
	{S08, {0, 0, 5, 20, 100}},
	{S09, {0, 0, 5, 20, 100}},
	{BONUS, {0, 0, 0, 1, 5, 10}}, // Indexing on BONUS is different because it is used as a scatter pay
};

// ==================================== Symbol Print Info ==================================== 
map<int, string> SlotGame::symbolStrings =
{ // These are how the symbols will print to the console during Free Play
	{BLANK, "--"},
	{WILD, "WILD"},
	{S01, "S01"},
	{S02, "S02"},
	{S03, "S03"},
	{S04, "S04"},
	{S05, "S05"},
	{S06, "S06"},
	{S07, "S07"},
	{S08, "S08"},
	{S09, "S09"},
	{BONUS, "BONUS"},
};

map<int, Colors> SlotGame::symbolColors =
{ // Available Colors are: BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE. Start with t for text color, b for background color
	{BLANK,tBLACK},
	{WILD,bRED},
	{S01,tCYAN},
	{S02,tBLUE},
	{S03,tBLUE},
	{S04,tGREEN},
	{S05,tGREEN},
	{S06,tYELLOW},
	{S07,tYELLOW},
	{S08,tWHITE},
	{S09,tWHITE},
	{BONUS,bYELLOW},
};

// ==================================== Lines ==================================== 
vector<vector<int>> SlotGame::lines =
{
	{1,1,1,1,1},
	{0,0,0,0,0},
	{2,2,2,2,2},
	{0,1,2,1,0},
	{2,1,0,1,2},
	{1,0,1,2,1},
	{1,2,1,0,1},
	{0,0,1,2,2},
	{2,2,1,0,0},
	{0,1,0,1,0},
	{2,1,2,1,2},
	{1,0,0,0,1},
	{1,2,2,2,1},
	{0,1,1,1,0},
	{2,1,1,1,2},
	{1,1,0,1,1},
	{1,1,2,1,1},
	{0,2,0,2,0},
	{2,0,2,0,2},
	{2,0,1,0,2},
	{0,2,1,2,0},
	{0,2,2,2,0},
	{2,0,0,0,2},
	{1,0,2,0,1},
	{1,2,0,2,1},
	{0,0,2,0,0},
	{2,2,0,2,2},
	{1,0,1,0,1},
	{1,2,1,2,1},
	{2,2,2,1,0},
	{0,0,0,1,2},
	{2,1,0,0,0},
	{0,1,2,2,2},
	{1,1,1,0,1},
	{1,1,1,2,1},
	{1,0,1,1,1},
	{1,2,1,1,1},
	{2,1,1,1,0},
	{0,1,1,1,2},
	{1,2,2,1,0},
};

// ==================================== Reels ==================================== 
vector<vector<int>> SlotGame::baseReels =
{
	{S07, S02, S03, S06, S05, S08, S01, S04, S06, S09, S08, BONUS, S09, S08, S06, S07, BONUS, S09, S04, S03, S08, S09, S02, S03, S06, S01, S03, S05, WILD, S08, S04, S07, WILD, S08, S09, S02, S08, S05, S06, S09},
	{S06, S05, S09, S07, S03, S09, S04, S05, S07, S09, S01, S06, S07, S09, S06, WILD, S08, S07, S09, S04, S01, S07, S09, S06, S04, S07, BONUS, S05, S09, S02, S07, S09, S04, S07, S08, S09, S07, S02, S01, S03, S08},
	{S06, S09, S07, S05, BONUS, S08, S03, S06, S07, S03, S06, S04, S08, S02, S09, S06, S08, S07, S05, S09, S08, WILD, S07, S06, S03, S01, S06, BONUS, S04, S08, S06, S02, S08, S07, S05, S01, S08, S07, S05, S06, S09, S08, S07, S06, S08, S05},
	{S07, S06, S02, S03, S08, BONUS, S06, S04, S02, S05, S03, S06, S09, WILD, S01, S08, S02, S05, S07, S08, S05, S07, S09, S04, S03, S06, S07, S09, WILD, S04, S08, WILD, S09, S04, S08, WILD, S05, S08, S02},
	{WILD, S04, S01, WILD, S06, S05, S09, S04, WILD, S07, S03, S09, S07, S04, S03, S09, S08, S02, S07, S09, S02, S04, S06, S01, S05, S08, S09, S05, S07, S06, WILD, S04, S03, BONUS, S06, S07, S08, S02, S05, S07, S06},
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
	{S08, S06, S07, S08, S06, S04, S08, S06, S07, WILD, WILD, WILD, S08, S04, S07, S05, BONUS, S08, S06, S07, S01, S09, S08, S07, S05, S06, S08, BONUS, S07, S08, S03, S05, S07, S06, S08, BONUS, S05, S06, S07, S08, S03, S04, S08, S02, S04, S06, S03, S08, S06, S03, S07, S08, S06, S05, S08, S07, S02, S08, S03, S01, S07, S08, S04, S06, S02, S08, S05, S07, BONUS, S06, S08, S01, S07, S08, S06, BONUS, S08, S05, S03, S07, S08, S05, S01, S08, S07, S04, S08, S06, S05, S08, S07, S06, S03, S08, S07, S04, S06, BONUS, S08, S06, BONUS, S07, S03, S08, S07, S06, S02, S08, S07, S02, S08, S03, S02, S08, S07, S04, S08, S07, S06, S03, BONUS, S08, S07, BONUS, S05, S08, S03, S05, S02, S06, S08, S02, S09, S06, S05, S08, S06, S01, S07, S08, S06, S04, S08, S01, S04, S06, S08, S03, S07, S08, BONUS, S03, S08, S06, S05, S08, S06, S03, S08, S09, BONUS, S07, S08, S05, S02, S06, S04, S08, BONUS, S07, S08, S03, S05, S04},
	{S09, S05, S07, S04, S09, S07, S01, S04, S07, S09, S02, S03, S09, S04, S07, S09, BONUS, S07, S09, S05, S01, S07, S09, WILD, WILD, WILD, S03, S07, S09, S01, S04, S03, S01, S07, S09, S04, S06, S02, S07, S09, S01, S07, S08, S09, S03, S07, S09, S05, S04, S02, S07, S09, S05, S03, S07, S09, S04, S03, S09, S02, S07, S09, S02, S07, S09, S02, S03, S07, S09, S01, S04, S07, BONUS, S03, S09, BONUS, S03, S09, S08, S04, S09, S07, BONUS, S09, S03, S07, S04, S09, S03, S07, S05, S06, S09, S03, S07, S04, S09, S02, S03, S07, BONUS, S09, S04, S07, BONUS, S09, S07, S02, S03, S07, S04, S03, S09, S05, S07, S09, BONUS, S07, S03, S09, S07, S04, S06, S05, S09, S04, S05, S09, S08, S07, S09, BONUS, S03, S09, S07, BONUS, S03, S07, S09, S02, S07, S09, S05, S07},
	{S06, S05, S04, S08, S09, S06, S05, WILD, WILD, WILD, S09, S06, S08, BONUS, S06, S08, BONUS, S06, S04, S09, BONUS, S05, S08, S06, S09, S08, BONUS, S04, S08, S06, S02, S08, S09, S06, S08, BONUS, S09, S06, S05, S08, S02, S06, BONUS, S08, S04, BONUS, S06, S08, S02, S06, S08, S07, S03, S01, S06, S07, S02, S08, S01, S06, S02, S08, S09, S05, BONUS, S08, S06, S09, S08, S05, S06, S09, S08, S02, S07, S09, S06, S08, S05, BONUS, S04, S08, S06, S05, S09, S06, S08, S05, S06, S01, S08, S05, S06, BONUS, S09, S06, S01, S05, S08, S04, S01, S08, S05, S07, S01, S08, S06, S07, S05, S08, S03, BONUS, S06, S05, S08, BONUS, S05, S08, S07, S03},
	{S04, S09, S08, S06, S04, S08, S09, S04, S01, S09, S07, WILD, WILD, WILD, WILD, S06, S09, S03, BONUS, S08, S04, S02, S03, S08, S04, S09, S02, S08, S07, S09, S03, S05, S07, S02, S09, S05, S08, BONUS, S05, S09, S08, S07, S09, S02, S03, S06, S08, S07, S09, S04, S02, S07, S01, S09, S02, S08, S07, S09, S02, S07, S01, S08, S09, S02, S06, S09, S08, S07, S06, S09, S03, S08, S09, S06, S07, S09, S02, S06, S07, S09, S02, S04, S08, S06, S04, S09, S06, BONUS, S08, S09, S06, S07, S05, S09, S08, S06, S09, S08, S02, S05, S03, S08, S09, S07, S05, S06, S09, S08, BONUS, S07, S05, S09, S06, S04, S02, S05, S09, S08, S04, BONUS, S08, S03, S06, S07, S08, S03, S09, S08, S05, S04, S09, S08, S04, S09, S03, BONUS, S09, S08, S06},
	{S09, S08, S06, S05, S09, S01, S06, S09, S03, S05, S07, BONUS, S05, S09, S02, S07, S09, S04, WILD, WILD, WILD, WILD, S07, S09, S08, S01, S04, S09, BONUS, S07, S03, S06, S09, S07, S06, S09, S05, S07, S09, S04, S06, S01, S07, S05, S03, BONUS, S09, S06, S03, S02, S09, S04, S02, S07, S06, S09, BONUS, S08, S03, S06, S09, S01, S08, S03, S02, S09, S04, S05, BONUS, S04, S09, S05, S02, S03, S07, S01, S09, S05, S04, S09, S05, S06, S09, S03, BONUS, S05, S09, S08, S04, S05, S09, BONUS, S05, S04, S01, S09, S06, S04, S07, S09, S03, S04, S07, S09, S04, S06, S09, S08, BONUS, S05, S08, S09, S07, BONUS, S09, S04, S08, BONUS, S07, S06, S09, S05, S02, S07, S09, S08, BONUS, S06, S04, S09, S02, S06, S09, S02, S04, S05, S09, BONUS, S07, S09, S06, S02},
};

vector<vector<int>> SlotGame::freeReelWeights =
{
	{ BLANK },
	{ BLANK },
	{ BLANK },
	{ BLANK },
	{ BLANK },
};

// ==================================== Weight and Value Tables ==================================== 
int SlotGame::numFreeGames[6] = { 0,0,0,5,10,15 };

vector<long long> SlotGame::freeSpinWildWeights = { 1, 1, 2, 4, 1, 2, 3, 2, 3, 2 };

vector<double> SlotGame::freeSpinWildValues = { 3, 5, 9, 17, 6, 10, 18, 12, 20, 24 };
