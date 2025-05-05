#pragma once

#include "SlotGame.h"

class GameTemplate : public SlotGame 
{
public:
	// ======== Functions ======== 
	GameTemplate(string mathXMLFileName, int baseBet, int betMult = 1, int totalBet = 0, bool useCurrentDirectory = false);
	double PlayGame() override;
	double PlayBonus(int bonusCode) override;

	// ======== Variables ======== 
	int numReels = 5;
	int numRows = 3;
	
	// Grids
	

	// Symbol Sets
	

	// Combos


	// Reels


	// Weight and Value Tables


	// ======== Symbol Properties ======== 
	map<string, set<string>> symbolSubstitutions =
	{ // Leave the entire map blank if you want to use the XML wild definitions. Default value is that the symbol only maps to itself.
		//{"WILD", {"WILD", "S01", "S02", "S03", "S04", "S05", "S06", "S07", "S08", "S09"}},
	};

	map<string, int> symbolMultipliers =
	{ // Default value is 1.
		// {"WILD_2X", 2},
	};

	map<string, ColorStyle> symbolColors =
	{ // Available Colors: LAVENDER, PURPLE, PINK, RED, ORANGE, BROWN, GOLD, YELLOW, LIME, GREEN, TEAL, CYAN, BLUE, WHITE, GRAY, BLACK
	  // Start with a 't' for text color and a 'b' for background color. Effects are BOLD, DIM, ITALIC, UNDERLINE, BLINK, INVERSE, HIDDEN, STRIKETHROUGH
		{"BLANK",tGRAY | eHIDDEN},
		{"WILD",bRED},
		{"S01",tCYAN},
		{"S02",tPINK},
		{"S03",tLIME},
		{"S04",tTEAL},
		{"S05",tORANGE},
		{"S06",tGREEN},
		{"S07",tBLUE},
		{"S08",tLAVENDER},
		{"S09",tWHITE},
		{"BONUS",bGOLD},
	};


	// ======== Lines ======== 
	vector<vector<int>> lines =
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
};