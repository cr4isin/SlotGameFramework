#pragma once

#include "SlotReels.h"
#include "MultiSymbolComboInfo.h"
#include "SymbolComboInfo.h"

class SlotGrid 
{
public:
	SlotGrid(int numReels=5, int numRows=3);

	// Standard Grid Functions
	int m_numReels = 1;
	int m_numRows = 1;
	void FillGrid(vector<int> &stops, SlotReels* &reels);
	void FillGridColumn(vector<int> &stops, SlotReels* &reels, int colIndex);
	void PrintGrid();
	void PrintGrid(vector<string> symbolStrings);

	// Grid Evaluation
	void SetLines(vector<vector<int>> lines, int numLines = 0);
	double EvaluateGrid(MultiSymbolComboInfo* &currentSymbolCombos);
	double EvaluateGrid(SymbolComboInfo* &currentSymbolCombos);

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

	vector<vector<int>> m_grid;
	vector<vector<int>> m_lines;
	vector<LineElement> m_lineElements;

};