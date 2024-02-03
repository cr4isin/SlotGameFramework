
#include "SlotGame.h"


// ==================================== Grid Dimensions ==================================== 
int SlotGame::numReels = 5;
int SlotGame::numRows = 3;
int SlotGame::numFreeGames[6] = { 0,0,0,10,15,20 };

// ==================================== Reels ==================================== 
vector<vector<int>> SlotGame::baseReels =
{
	{S07, S08, S08, S02, S06, S05, S09, S02, S03, S07, S10, BONUS, S09, S10, S06, S05, S08, S01, S07, S09, S05, S03, S07, S04, S09, S05, S03, S10, S04, S09, S08, S01, S10, S07, S05, S05, S06, S08, S06, S02, S09, S06, S03, S08, BONUS, S04, S04, S07, S10, S01, S06, S10, S07, S05, S08, S06, S04, S02, S09, S03, S04, S10},
	{WILD}, //{S09, S05, S06, S07, S09, S08, WILD, S09, S08, S04, S04, S05, S09, S07, S01, S08, S06, S03, S06, S02, BONUS, S08, S07, S08, S06, S03, S10, S09, S07, S10, S06, S02, S05, S10, S07, WILD, S10, S07, S02, S05, S06, S10, S01, S05, S04, S10, S03, S04, S02, S04, BONUS, S09, S10, S05, S01, S05, S08, S03, S07, S06, S04, S08, S09, S03},
	{WILD}, //{S07, S02, S10, S05, S01, S02, S08, S10, BONUS, S09, S07, S02, S06, S06, S04, S08, S07, WILD, S08, S07, S09, S10, S03, S07, S04, S10, S05, S03, S01, S03, S09, S06, S05, S02, S04, S04, S05, S09, BONUS, S06, S07, S03, S08, S05, S03, S06, S08, S09, S04, S06, S05, S05, S07, S10, S09, WILD, S10, S09, S08, S06, S01, S04, S08, S10},
	{WILD}, //{S02, S06, S06, S03, S07, S01, S05, S02, S10, S08, WILD, S10, S08, S04, S10, S07, S07, S01, S06, S05, S07, S09, BONUS, S10, S09, S08, S03, S04, S05, S08, S02, S07, S06, S10, S09, S07, WILD, S09, S07, S03, S02, S08, S06, S03, S09, S06, S01, S06, S08, S09, BONUS, S05, S04, S05, S04, S03, S08, S04, S09, S10, S04, S05, S05, S10},
	{S06, S02, S07, S09, S01, S08, S09, S02, S10, S07, S05, S02, S03, BONUS, S08, S07, S04, S06, S04, S03, S07, S03, S08, S09, S10, S01, S06, S09, S05, S06, S04, S04, S10, S08, S02, S10, S06, S07, S10, S01, S08, S09, S05, S09, S10, S06, S04, S05, BONUS, S05, S04, S05, S05, S08, S07, S09, S08, S03, S10, S07, S06, S03},
};

vector<vector<int>> SlotGame::baseReelWeights =
{
	{  },
	{  },
	{  },
	{  },
	{  },
};

vector<vector<int>> SlotGame::freeReels =
{
	{S03, S04, S08, S10, S09, S06, S08, S05, S10, S02, S07, S10, S04, S03, S10, S07, S10, S10, S01, S08, S07, S07, S05, S06, S09, S09, S08, S08, S05, S09, S07, S08, S06, S09, S06, S10, S08, S08, S03, S10, S08, S05, S08, S09, S02, S09, S07, S06, S04, S06, S10, S09, S10, S06, S04, S09, S05, S05, S07, S05, S09, S05},
	{WILD}, //{S08, S07, S02, S04, S06, S10, S03, S09, S08, WILD, S09, S08, S06, S10, S04, S06, S10, S02, S09, S07, S03, S05, S01, S09, S07, S05, S10, S04, S09, S08, S05, S02, S05, S07, S04, S10, S01, S09, S05, S06, S07, S03, S05, S10, S07, WILD, S10, S07, S06, S05, S10, S08, S06, S04, S09, S09, S05, S08, S06, S07, S03, S07, S04, S08, S02, S06, S03, S10, S01, S08, S09, S08},
	{WILD}, //{S04, S06, S07, S09, S05, S03, S07, S06, S04, S10, S10, S09, S05, S08, S07, WILD, S08, S07, S08, S07, S10, S03, S06, S04, S09, S01, S06, S06, S10, S02, S03, S06, S05, S07, S04, S05, S07, S05, S05, S09, S08, S03, S07, S08, S01, S10, S02, S04, S06, S08, S09, S07, S06, S10, S09, WILD, S10, S09, S02, S08, S02, S10, S08, S08, S03, S05, S04, S09, S09, S01, S06, S05, S10},
	{WILD}, //{S06, S08, S07, S03, S04, S08, S10, S08, WILD, S10, S08, S05, S07, S02, S08, S05, S07, S04, S09, S01, S06, S08, S08, S10, S10, S09, S09, S03, S02, S10, S01, S06, S07, S03, S07, S10, S06, S02, S06, S06, S09, S05, S04, S08, S06, S01, S06, S03, S08, S04, S10, S09, S05, S07, S07, S10, S03, S09, S05, S05, S09, S07, WILD, S09, S07, S04, S10, S05, S02, S04, S09, S05},
	{S04, S03, S08, S07, S10, S05, S08, S08, S07, S09, S06, S06, S02, S07, S04, S09, S05, S06, S08, S10, S09, S10, S07, S09, S08, S05, S09, S08, S09, S08, S10, S01, S03, S10, S08, S06, S06, S08, S07, S05, S02, S05, S10, S03, S04, S09, S05, S08, S10, S10, S06, S10, S07, S04, S09, S09, S05, S09, S07, S06, S10},
};

vector<vector<int>> SlotGame::freeReelWeights =
{
	{  },
	{  },
	{  },
	{  },
	{  },
};

// ==================================== Symbol Combo Info ==================================== 
map<int, set<int>> SlotGame::symbolSubstitutions =
{ // Symbols not listed here will be set to only map to themselves: {X, {X}}
	{BLANK, {}},
	{WILD, {S01, S02, S03, S04, S05, S06, S07, S08, S09, S10}},
	{BONUS, {}},
};

map<int, int> SlotGame::symbolMultipliers =
{ // Symbols not included will be set to a multiplier of 1: {X, 1}

};

map<int, vector<double>> SlotGame::paytable =
{ // Symbols not listed here will be set to have zero pays: {X, {0,0,0,0,0}}
	{S01,{0,0,40,80,800}},
	{S02,{0,0,30,60,400}},
	{S03,{0,0,20,40,300}},
	{S04,{0,0,10,20,200}},
	{S05,{0,0,6,10,100}},
	{S06,{0,0,6,8,50}},
	{S07,{0,0,4,6,40}},
	{S08,{0,0,4,6,30}},
	{S09,{0,0,2,4,20}},
	{S10,{0,0,2,4,10}},
};

map<int, string> SlotGame::symbolStrings =
{
	{BLANK, "--"},
	{WILD,"WILD"},
	{S01,"S01"},
	{S02,"S02"},
	{S03,"S03"},
	{S04,"S04"},
	{S05,"S05"},
	{S06,"S06"},
	{S07,"S07"},
	{S08,"S08"},
	{S09,"S09"},
	{S10,"S10"},
	{BONUS,"BONUS"},
};


// ==================================== Lines ==================================== 
vector<vector<int>> SlotGame::CustomLines =
{
	{0,0,0,0,0},
	{1,1,1,1,1},
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