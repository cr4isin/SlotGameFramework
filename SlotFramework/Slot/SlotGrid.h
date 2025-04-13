#pragma once

#include "SlotReels.h"
#include "Paytable.h"
#include "ComboResults.h"

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
	
	// Fill Grid Functions
	void FillGrid(vector<int> &positions, SlotReels &reels);
	void FillGridReel(int reelIndex, int position, SlotReels &reels);

	// Grid Evaluation
	void SetLines(vector<vector<int>> lines, int numLines = 0);
	void SetInFreePlay(bool inFreePlay);
	ComboResults Evaluate(PaylineCombo& paylineCombo);
	ComboResults Evaluate(AnywaysCombo& anywaysCombo);
	ComboResults Evaluate(ScatterCombo& scatterCombo);
	ComboResults Evaluate(CountScatterCombo& countScatterCombo);

	// Grid Manipulation
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
	void PrintGrid();

private:
	int m_numReels = 1; // Number of Reels in the Grid
	int m_numRows = 1; // Number of Rows in the Grid
	vector<vector<int>> m_grid; // 2D Vector (Reel, Row) that contains the symbols on the grid
	vector<vector<int>> m_lines; // 2D Vector (Line, Reel) that contains the current active lines
	int m_maxSymbolLength = 3;
	bool m_inFreePlay = false; // Determines if combo info will be printed after Grid Evaluation
};