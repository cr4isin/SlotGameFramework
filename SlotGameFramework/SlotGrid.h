#pragma once

#include "SlotReels.h"
#include "SymbolCombos.h"

class SlotGrid 
{
public:
	SlotGrid();
	SlotGrid(int numReels, int numRows);
	int GetSymbol(int reelIndex, int rowIndex);
	void SetSymbol(int symbol, int reelIndex, int rowIndex);
	vector<vector<int>> GetGrid();
	int GetNumReels();
	int GetNumRows();
	
	// Standard Grid Functions
	void FillGrid(vector<int> &positions, SlotReels &reels);
	void FillGridReel(int reelIndex, int position, SlotReels &reels);
	void PrintGrid();

	// Grid Evaluation
	void SetLines(vector<vector<int>> lines, int numLines = 0);
	void SetWays(int numSymbols, map<int, vector<double>> paytable, map<int, set<int>> symbolSubstitutions, map<int, int> symbolMultipliers);
	void SetSymbolPrintInfo(map<int, string> symbolStrings, map<int, Colors> symbolColors = {});
	void SetInFreePlay(bool inFreePlay);
	double EvaluateLines(SymbolCombos& symbolCombos, int multiplier);
	double EvaluateLines(SymbolCombos& symbolCombos, int multiplier, int& bonusCount);
	double EvaluateWays(int multiplier = 1);

	bool IsSymbolOnGrid(int symbol);
	bool IsSymbolOnReel(int symbol, int reelIndex);
	bool IsSymbolOnRow(int symbol, int rowIndex);
	int CountSymbolOnGrid(int symbol);
	int CountSymbolOnReel(int symbol, int reelIndex);
	int CountSymbolOnRow(int symbol, int rowIndex);
	void FillGridWithSymbol(int symbol);
	void FillReelWithSymbol(int symbol, int reelIndex);
	void FillRowWithSymbol(int symbol, int rowIndex);
	void ReplaceSymbolOnGrid(int oldSymbol, int newSymbol);
	void ReplaceSymbolOnReel(int oldSymbol, int newSymbol, int reelIndex);
	void ReplaceSymbolOnRow(int oldSymbol, int newSymbol, int rowIndex);

	string GetSymbolString(int symbol);
	string GetSymbolColor(int symbol);

private:
	int m_numReels = 1; // Number of Reels in the Grid
	int m_numRows = 1; // Number of Rows in the Grid
	vector<vector<int>> m_grid; // 2D Vector (Reel, Row) that contains the symbols on the grid
	vector<vector<int>> m_lines; // 2D Vector (Line, Row) that contains the current active lines
	int m_numSymbols = 1; // The number of symbols to evaluate (used for evaluating ways-pays)
	map<int, set<int>> m_symbolSubstitutions; // The symbol substitutions mapping (used for evaluating ways-pays)
	map<int, int> m_symbolMultipliers; // The symbol multipiers mapping (used for evaluating ways-pays)
	map<int, vector<double>> m_paytable; // The symbol paytable mapping (used for evaluating ways-pays)
	vector<vector<int>> m_symbolSubstitutionCount; // Combines the symbol substitutions and multipliers for efficient ways evaluation
	map<int, string> m_symbolStrings; // The symbol string mapping (used for printing the Grid and combo info)
	map<int, Colors> m_symbolColors; // The symbol color mapping (used for printing the Grid and combo info)
	int m_maxSymbolLength = 3;
	bool m_inFreePlay = false; // Determines if combo info will be printed after Grid Evaluation
};