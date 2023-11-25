
#include "SlotGrid.h"


// ===================== Standard Grid Functions ===================== 
SlotGrid::SlotGrid(int numReels, int numRows)
{
	m_numReels = numReels;
	m_numRows = numRows;
	m_grid.resize(numReels);
	for (int iReel = 0; iReel < numReels; iReel++)
	{
		m_grid.at(iReel).resize(numRows);
	}
}

void SlotGrid::FillGrid(vector<int> &stops, SlotReels* &reels)
{
	for (int iCol = 0; iCol < m_numReels; iCol++)
	{
		FillGridColumn(stops, reels, iCol);
	}
}

void SlotGrid::FillGridColumn(vector<int> &stops, SlotReels* &reels, int colIndex)
{
	int stopIndex = reels->GetIndexFromStop(colIndex, stops[colIndex]);
	vector<int> newCol(reels->m_reels[colIndex].begin() + stopIndex, reels->m_reels[colIndex].begin() + stopIndex + m_numRows);
	m_grid[colIndex] = newCol;
}

void SlotGrid::PrintGrid()
{
	for (int iRow = 0; iRow < m_numRows; iRow++)
	{
		for (int iReel = 0; iReel < m_numReels; iReel++)
		{
			cout << m_grid[iReel][iRow] << "\t";
		}
		cout << "\n";
	}
	cout << "\n";
}

void SlotGrid::PrintGrid(vector<string> symbolStrings)
{
	for (int iRow = 0; iRow < m_numRows; iRow++)
	{
		for (int iReel = 0; iReel < m_numReels; iReel++)
		{
			cout << symbolStrings.at(m_grid[iReel][iRow]) << "\t";
		}
		cout << "\n";
	}
	cout << "\n";
}

// ===================== Grid Evaluation ===================== 
void SlotGrid::SetLines(vector<vector<int>> lines, int numLines)
{
	m_lines.clear();
	m_lineElements.clear();
	if (numLines <= 0)
	{
		numLines = lines.size();
	}
	for (int iLine = 0; iLine < numLines; iLine++)
	{
		m_lines.push_back(lines.at(iLine));
		int row = lines.at(iLine).at(0);
		bool found = false;
		for (auto& line_element : m_lineElements)
		{
			if (line_element.m_row == row)
			{
				line_element.AddElement(lines.at(iLine));
				found = true;
				break;
			}
		}
		if (!found)
		{
			m_lineElements.push_back(LineElement(0, row, m_numReels, lines.at(iLine)));
		}
	}
}

SlotGrid::LineElement::LineElement(int reel, int row, int numReels, vector<int> line)
{
	m_line_count++;
	m_reel = reel;
	m_row = row;
	m_numReels = numReels;
	if (m_reel + 1 < m_numReels)
	{
		int next_row = line.at(m_reel + 1);
		m_subLineElements.push_back(LineElement(m_reel + 1, next_row, m_numReels, line));
	}
}

void SlotGrid::LineElement::AddElement(vector<int>& line)
{
	// Adds a branching path from the current line element
	m_line_count++;
	if (m_reel + 1 < m_numReels)
	{
		int next_row = line.at(m_reel + 1);
		bool found = false;
		for (auto& line_element : m_subLineElements)
		{
			if (line_element.m_row == next_row)
			{
				line_element.AddElement(line);
				found = true;
				break;
			}
		}
		if (!found)
		{
			m_subLineElements.push_back(LineElement(m_reel + 1, next_row, m_numReels, line));
		}
	}
}

double SlotGrid::EvaluateGrid(MultiSymbolComboInfo* &currentSymbolCombos)
{
	double score = 0;
	vector<vector<int>> grid = m_grid;
	for (const auto& line_element : m_lineElements)
	{
		line_element.EvaluateElement(score, grid, currentSymbolCombos);
	}
	return score;
}

void SlotGrid::LineElement::EvaluateElement(double& score, vector<vector<int>> &grid, MultiSymbolComboInfo* &currentSymbolCombos, size_t symbol_key) const
{
	int current_symbol = grid[m_reel][m_row];
	symbol_key += currentSymbolCombos->GetSymbolLocation(m_reel, current_symbol);
	double pay = 0;
	bool breaks = false;
	currentSymbolCombos->GetComboInfo(symbol_key, m_reel, pay, breaks);
	if (breaks)
	{
		score += m_line_count * pay;
	}
	else
	{
		for (const auto& line_element : m_subLineElements)
		{
			line_element.EvaluateElement(score, grid, currentSymbolCombos, symbol_key);
		}
	}
}

double SlotGrid::EvaluateGrid(SymbolComboInfo*& currentSymbolCombos)
{
	return 0.0;
}