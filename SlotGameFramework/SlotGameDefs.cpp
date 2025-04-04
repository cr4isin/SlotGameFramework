
#include "SlotGame.h"

// ==================================== Grid Dimensions & Symbol Combos ==================================== 
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

// ==================================== Symbol Printing ==================================== 
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
{ // Available Colors: LAVENDER, PURPLE, PINK, RED, ORANGE, BROWN, GOLD, YELLOW, LIME, GREEN, TEAL, CYAN, BLUE, WHITE, GRAY, BLACK
  // Start with a 't' for text color and a 'b' for background color
	{BLANK,tGRAY},
	{WILD,bRED},
	{S01,tCYAN},
	{S02,tPINK},
	{S03,tLIME},
	{S04,tTEAL},
	{S05,tORANGE},
	{S06,tGREEN},
	{S07,tBLUE},
	{S08,tLAVENDER},
	{S09,tWHITE},
	{BONUS,bGOLD},
};

// ==================================== Lines ==================================== 
vector<vector<int>> SlotGame::lines =
{
	{1,1,1,1,1},// 1
	{0,0,0,0,0},// 2
	{2,2,2,2,2},// 3
	{0,1,2,1,0},// 4
	{2,1,0,1,2},// 5
	{1,0,1,2,1},// 6
	{1,2,1,0,1},// 7
	{0,0,1,2,2},// 8
	{2,2,1,0,0},// 9
	{0,1,0,1,0},// 10
	{2,1,2,1,2},// 11
	{1,0,0,0,1},// 12
	{1,2,2,2,1},// 13
	{0,1,1,1,0},// 14
	{2,1,1,1,2},// 15
	{1,1,0,1,1},// 16
	{1,1,2,1,1},// 17
	{0,2,0,2,0},// 18
	{2,0,2,0,2},// 19
	{2,0,1,0,2},// 20
	{0,2,1,2,0},// 21
	{0,2,2,2,0},// 22
	{2,0,0,0,2},// 23
	{1,0,2,0,1},// 24
	{1,2,0,2,1},// 25
	{0,0,2,0,0},// 26
	{2,2,0,2,2},// 27
	{1,0,1,0,1},// 28
	{1,2,1,2,1},// 29
	{2,2,2,1,0},// 30
	{0,0,0,1,2},// 31
	{2,1,0,0,0},// 32
	{0,1,2,2,2},// 33
	{1,1,1,0,1},// 34
	{1,1,1,2,1},// 35
	{1,0,1,1,1},// 36
	{1,2,1,1,1},// 37
	{2,1,1,1,0},// 38
	{0,1,1,1,2},// 39
	{1,2,2,1,0},// 40
};

