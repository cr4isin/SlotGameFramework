#pragma once

#include "General.h"
#include "SlotGrid.h"
#include "WeightTable.h"

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
		LL,
		FG,
		numSymbols
	};
	
	// ==================== General Functions and Variables ====================
	// Grid and Reels
	SlotGrid* baseGrid;
	SlotReels* baseReelSet;
	// Grid Evaluation
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
	void RunSims(int numTrials, int trialSize);
	void FreePlay();
	void CyclePositions();
	void CyclePositionsRecursive(map<double, size_t> &hist, vector<int> &positions, double &maxScore, vector<int> &maxPositions, int currentReel = 0);

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