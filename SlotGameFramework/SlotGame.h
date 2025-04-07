#pragma once

#include "General.h"
#include "SlotGrid.h"
#include "MathXML.h"

class SlotGame 
{
public:

	enum Symbols 
	{
		ANY = -1,
		BLANK, 
		WILD,
		S01,
		S02,
		S03,
		S04,
		S05,
		S06,
		S07,
		S08,
		S09,
		BONUS,
		numSymbols
	};
	
	// ==================== Functions ====================
	// Setup
	SlotGame(string configName, int baseBet, int betMult, int totalBet = 0);
	void SetConfig(string configName);
	void SetBetScheme(int baseBet, int betMult, int totalBet = 0);
	void SetupGrids();
	void SetupReels();
	void SetupWeightTables();
	// Game Functions
	double PlayGame();
	double PlayBonus();
	// Other Functions
	void AddToTracker(string name, double value);
	void ClearTrackers();
	void AddToHistogram(string name, double value, long long numHits = 1);
	void PrintHistograms(string simName);
	void RunSims(int numGames, vector<string>& args);
	void FreePlay(bool clearConsole = false);
	void CyclePositions();
	void CyclePositionsRecursive(map<double, size_t>& hist, vector<int>& positions, double& maxScore, vector<int>& maxPositions, int currentReel = 0);
	void ReadXMLTables(string xmlName);
	void ReadXMLReels(string xmlName);
	void ReadXMLCombos(string xmlName);
	// ==================== Variables ====================
	string configName = "";
	string mathxml = "";
	int baseBet = 1;
	int betMult = 1;
	int totalBet = 1;
	bool inFreePlay = false;
	map<string, WeightTable> weightTables;
	map<string, vector<double>> valueTables;
	map<string, SlotReels> reelSets;
	struct Tracker
	{
		double value = 0;
		double maxValue = 0;
		int totalHits = 0;
		int totalWins = 0;
	};
	struct TotalTracker
	{
		double value = 0;
		double maxValue = 0;
		int gameHits = 0;
		int gameWins = 0;
		int totalHits = 0;
		int totalWins = 0;
	};
	map<string, Tracker> trackers;
	map<string, TotalTracker> totalTrackers;
	map<string, map<double, long long>> histograms;

	// ==================== Game Specific ====================
	// NON-STATIC VARIABLES
	SlotGrid baseGrid;
	SlotGrid freeGrid;
	SlotReels baseReelSet;
	SlotReels freeReelSet;
	int numBonus = 0;

	// STATIC VARIABLES (can be defined in SlotGameDefs.cpp)
	static int numReels;
	static int numRows;
	static map<int, set<int>> symbolSubstitutions;
	static map<int, int> symbolMultipliers;
	static map<int, vector<double>> paytable;
	static map<int, string> symbolStrings;
	static map<int, Colors> symbolColors;
	static vector<vector<int>> lines;

};