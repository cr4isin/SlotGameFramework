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
		WC,
		AA,
		BB,
		CC,
		DD,
		EE,
		FF,
		GG,
		HH,
		JJ,
		KK,
		BN,
		numSymbols
	};
	
	// ==================== General Functions and Variables ====================
	// Grid and Reels
	SlotGrid* baseGrid;
	SlotReels* baseReelSet;
	// Grid Evaluation
	//MultiSymbolComboInfo* multiSymbolComboInfo;
	SymbolComboInfo* symbolComboInfo;
	int baseBet = 1;
	int betMult = 1;
	int totalBet = 1;
	bool printComboInfo = false;
	// Setup
	void SetBetScheme(int baseBet, int betMult, int totalBet = 0);
	void SetupGame();
	void SetupGrids();
	void SetupReels();
	void SetupWeightTables();
	// Playing Game
	double PlayGame();
	double PlayBonus();
	// Other Functions
	void DoSomething();
	void CycleStops();
	void CycleStopsRecursive(map<double, size_t> &hist, vector<int> &stops, int currentReel = 0);
	void RunSims(int numTrials, int trialSize);
	void FreePlay();

	// ==================== Game Specific ====================
	// STATIC VARIABLES (can be defined in SlotGameDefs.cpp)
	static int numReels;
	static int numRows;
	static vector<vector<int>> baseReels;
	static vector<vector<int>> baseReelWeights;
	static map<int, set<int>> symbolSubstitutions;
	static map<int, int> symbolMultipliers;
	static map<int, vector<double>> paytable;
	static map<int, string> symbolStrings;
	static vector<vector<int>> CustomLines;

	// NON-STATIC VARIABLES

};