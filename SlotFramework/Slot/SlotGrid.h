#pragma once

#include "SlotReels.h"
#include "Paytable.h"
#include "MysteryReplacement.h"

class SlotGrid 
{
public:
	// Constructors
	SlotGrid() = default;
	SlotGrid(int numReels, int numRows);

	// Getters & Setters
	int GetNumReels();
	int GetNumRows();
	vector<vector<int>> GetGrid();
	int GetSymbol(int reelIndex, int rowIndex);
	void SetSymbol(int symbol, int reelIndex, int rowIndex);
	
	// Fill Grid Functions
	void FillGrid(vector<int> &positions, SlotReels &reels);
	void FillGridReel(int reelIndex, int position, SlotReels &reels);
	void PrintGrid(SymbolSet& symbolSet);
	void PrintGridTimed(SymbolSet& symbolSet, int delayInMillis = 250);
	void ClearHighlights();
	void AddHighlights(PaylineCombo& paylineCombo);
	void SetHighlights(PaylineCombo& paylineCombo);
	void AddHighlights(AnywaysCombo& anywaysCombo);
	void SetHighlights(AnywaysCombo& anywaysCombo);

	// Grid Evaluation
	void SetLines(vector<vector<int>> lines, int numLines = 0);
	void Evaluate(PaylineCombo& paylineCombo);
	void Evaluate(AnywaysCombo& anywaysCombo);
	void Evaluate(ScatterCombo& scatterCombo);
	void Evaluate(CountScatterCombo& countScatterCombo);

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
	void Replace(MysteryReplacement& mysteryReplacement);

private:
	int numReels = 1; // Number of Reels in the Grid
	int numRows = 1; // Number of Rows in the Grid
	vector<vector<int>> grid; // 2D Vector (Reel, Row) that contains the symbols on the grid
	vector<vector<int>> lines; // 2D Vector (Line, Reel) that contains the current active lines
	vector<vector<bool>> highlights;
};