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
	vector<vector<int>> m_grid;
	void FillGrid(vector<int> &stops, SlotReels* &reels);
	void FillGridColumn(vector<int> &stops, SlotReels* &reels, int colIndex);
	void PrintGrid();



	// Standard Left to Right Lines Grid Evaluation (optimized with tree structure)
	double EvaluateGrid(MultiSymbolComboInfo* &currentSymbolCombos);
	void SetLines(vector<vector<int>> lines, int numLines = 0);
	void PrintLines();

	class LineElement
	{
	public:
		LineElement(int reel = 0, int row = 0, int numReels = 1, vector<int> line = {})
		{
			m_line_count++;
			m_reel = reel;
			m_row = row;
			m_numReels = numReels;
			if (m_reel + 1 < m_numReels)
			{
				int next_row = line.at(m_reel + 1);
				m_lines.push_back(LineElement(m_reel + 1, next_row, m_numReels, line));
			}
		}

		int m_line_count = 0;
		int m_reel = 0;
		int m_row = 0;
		int m_numReels = 0;
		vector<LineElement> m_lines;
		void AddElement(vector<int>& line);
		void EvaluateElement(double& score, vector<vector<int>> &grid, MultiSymbolComboInfo* &currentSymbolCombos, size_t symbol_key = 0) const;
		void PrintLine();
	};

	vector<LineElement> m_lines;

	// Left to Right AND Right to Left Lines Grid Evaluation (optimized with tree structure)
	vector<LineElement> m_leftLines;
	vector<LineElement> m_rightLines;

	void SetTwoWayLines(vector<vector<int>> lines, int numLines = 0);
	double EvaluateTwoWayGrid(MultiSymbolComboInfo*& currentSymbolCombos);

};