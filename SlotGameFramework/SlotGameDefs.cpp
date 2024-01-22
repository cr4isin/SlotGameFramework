
#include "SlotGame.h"


// ==================================== Grid Dimensions ==================================== 
int SlotGame::numReels = 5;
int SlotGame::numRows = 4;


// ==================================== Reels ==================================== 
vector<vector<int>> SlotGame::baseReels =
{
	{BB, BB, GG, JJ, AA, GG, JJ, BB, GG, JJ, HH, AA, GG, GG, EE, HH, JJ, FG, HH, AA, EE, CC, DD, DD, EE, AA, JJ, GG, JJ, CC, FF, FF, AA, GG, CC, EE, EE, HH, JJ, LL, HH, AA, GG, DD, DD, CC, DD, FF, HH, JJ, AA, GG, HH, FF, FF, HH, AA, JJ, GG, FG, HH, JJ, GG, BB, HH, FF, FF, KK, JJ},
	{WC},
	{WC},
	{WC},
	{EE, CC, BB, AA, AA, EE, CC, BB, CC, FF, FF, FF, DD, AA, CC, DD, BB, FF, AA, FF, EE, DD, DD, AA, CC, DD, EE, AA, FF, DD, CC, DD, CC, FG, DD, GG, CC, CC, AA, EE, DD, CC, KK, EE, EE, DD, AA, FF, EE, KK, DD, CC, EE, EE, DD, CC, EE, EE, JJ, AA, LL, DD, CC, EE, DD, FG, EE, AA, HH},
};

vector<vector<int>> SlotGame::baseReelWeights =
{
	{1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 1, 1, 1, 1, 1},
	{1},
	{1},
	{1},
	{3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

// ==================================== Symbol Combo Info ==================================== 
map<int, set<int>> SlotGame::symbolSubstitutions =
{ // Symbols not listed here will be set to only map to themselves: {X, {X}}
	{BLANK, {}},
	{WC, {AA, BB, CC, DD, EE, FF, GG, HH, JJ, KK, LL}},
};

map<int, int> SlotGame::symbolMultipliers =
{ // Symbols not included will be set to a multiplier of 1: {X, 1}

};

map<int, vector<double>> SlotGame::paytable =
{ // Symbols not listed here will be set to have zero pays: {X, {0,0,0,0,0}}
	{AA,{0,25,125,250,800}},
	{BB,{0,20,100,200,500}},
	{CC,{0,15,75,150,400}},
	{DD,{0,10,50,100,300}},
	{EE,{0,10,50,100,300}},
	{FF,{0,0,25,50,150}},
	{GG,{0,0,25,50,150}},
	{HH,{0,0,15,25,100}},
	{JJ,{0,0,15,25,100}},
	{KK,{0,0,10,15,75}},
	{LL,{0,0,10,15,75}},
};

map<int, string> SlotGame::symbolStrings =
{
	{BLANK, "--"},
	{WC,"WC"},
	{AA,"AA"},
	{BB,"BB"},
	{CC,"CC"},
	{DD,"DD"},
	{EE,"EE"},
	{FF,"FF"},
	{GG,"GG"},
	{HH,"HH"},
	{JJ,"JJ"},
	{KK,"KK"},
	{LL,"LL"},
	{FG,"FG"},
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