#pragma once

#include "SlotReels.h"
#include "MultiSymbolComboInfo.h"
#include "SymbolComboInfo.h"

class SlotGrid 
{
public:
	SlotGrid(int numReels=5, int numRows=3);
	int GetSymbol(int reelIndex, int rowIndex);
	vector<vector<int>> GetGrid();
	int GetNumReels();
	int GetNumRows();
	
	// Standard Grid Functions
	void FillGrid(vector<int> &positions, SlotReels* &reels);
	void FillGridReel(int reelIndex, int position, SlotReels* &reels);
	void PrintGrid();

	// Grid Evaluation
	void SetLines(vector<vector<int>> lines, int numLines = 0);
	void SetWays(int numSymbols, map<int, vector<double>> paytable, map<int, set<int>> symbolSubstitutions, map<int, int> symbolMultipliers);
	void SetSymbolPrintInfo(map<int, string> symbolStrings, map<int, Colors> symbolColors = {});
	void SetInFreePlay(bool inFreePlay);
	double EvaluateGridLines(MultiSymbolComboInfo* &currentSymbolCombos);
	double EvaluateGridLines(SymbolComboInfo* &currentSymbolCombos);
	double EvaluateGridWays();

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
	class LineElement
	{
	public:
		LineElement(int reel = 0, int row = 0, int numReels = 1, vector<int> line = {});
		int m_line_count = 0;
		int m_reel = 0;
		int m_row = 0;
		int m_numReels = 0;
		vector<LineElement> m_subLineElements;
		void AddElement(vector<int>& line);
		void EvaluateElement(double& score, vector<vector<int>> &grid, MultiSymbolComboInfo* &currentSymbolCombos, size_t symbol_key = 0) const;
	};

	int m_numReels = 1; // Number of Reels in the Grid
	int m_numRows = 1; // Number of Rows in the Grid
	vector<vector<int>> m_grid; // 2D Vector (Reel, Row) that contains the symbols on the grid
	vector<vector<int>> m_lines; // 2D Vector (Line, Row) that contains the current active lines
	vector<LineElement> m_lineElements; // Root of the tree of line elements (used when evaluating lines with Multi-Symbol Combos)
	int m_numSymbols = 1; // The number of symbols to evaluate (used for evaluating ways-pays)
	map<int, set<int>> m_symbolSubstitutions; // The symbol substitutions mapping (used for evaluating ways-pays)
	map<int, int> m_symbolMultipliers; // The symbol multipiers mapping (used for evaluating ways-pays)
	map<int, vector<double>> m_paytable; // The symbol paytable mapping (used for evaluating ways-pays)
	map<int, string> m_symbolStrings; // The symbol string mapping (used for printing the Grid and combo info)
	map<int, Colors> m_symbolColors;
	bool m_inFreePlay = false; // Determines if combo info will be printed after Grid Evaluation
};