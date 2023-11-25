#pragma once

#include "General.h"
#include "SlotGrid.h"

class SlotGame 
{
public:
	SlotGame();
	~SlotGame();

	enum Symbols 
	{
		BLANK,
		W7,
		SE,
		BE,
		PL,
		OR,
		CH,
		MM,
		numSymbols
	};
	
	// ==================== General Functions and Variables ====================
	// Grid and Reels
	SlotReels* baseReelSet;
	SlotGrid* baseGrid;
	// Grid Evaluation
	MultiSymbolComboInfo* multiSymbolComboInfo;
	SymbolComboInfo* symbolComboInfo;
	// Playing Game
	double PlayGame();
	double PlayBonus();
	// Other Functions
	void DoSomething();
	void SetupGame();
	// Cycle Stops
	void CycleStops();
	void CycleStopsRecursive(map<double, size_t> &hist, vector<int> &stops, int currentReel = 0);


	// ==================== Game Specific ====================
	// STATIC VARIABLES (can be defined in SlotGameDefs.cpp)
	static int numReels;
	static int numRows;
	static vector<string> symbolStrings;
	static vector<vector<int>> baseReels;
	static vector<vector<int>> baseReelWeights;
	static map<int, set<int>> symbolSubstitutions;
	static map<int, int> symbolMultipliers;
	static map<int, vector<double>> paytable;
	static vector<vector<int>> CustomLines;

	// NON-STATIC VARIABLES

};