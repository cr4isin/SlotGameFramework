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
	
	// General Functions and Variables
	SlotReels* baseReelSet;
	SlotGrid* baseGrid;
	MultiSymbolComboInfo* multiSymbolComboInfo;
	SymbolComboInfo* symbolComboInfo;
	void DoSomething();
	void SetupGame();
	void CycleStops();
	void CycleStopsRecursive(map<double, size_t> &hist, vector<int> &stops, int currentReel = 0);


	// === GAME SPECIFIC ===
	// STATIC VARIABLES (can be defined in SlotGameDefs.cpp)
	static int numReels;
	static int numRows;
	static vector<vector<int>> baseReels;
	static vector<vector<int>> baseReelWeights;
	static map<int, set<int>> symbolSubstitutions;
	static map<int, int> symbolMultipliers;
	static map<int, vector<double>> paytable;
	static vector<vector<int>> CustomLines;

	// NON-STATIC VARIABLES

};