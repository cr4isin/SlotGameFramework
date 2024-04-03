
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

int SlotGrid::GetSymbol(int reelIndex, int rowIndex)
{
	return m_grid[reelIndex][rowIndex];
}

vector<vector<int>> SlotGrid::GetGrid()
{
	return m_grid;
}

int SlotGrid::GetNumReels()
{
	return m_numReels;
}

int SlotGrid::GetNumRows()
{
	return m_numRows;
}

void SlotGrid::FillGrid(vector<int> &positions, SlotReels* &reels)
{
	for (int iReel = 0; iReel < m_numReels; iReel++)
	{
		FillGridReel(iReel, positions[iReel], reels);
	}
}

void SlotGrid::FillGridReel(int reelIndex, int position, SlotReels*& reels)
{
	m_grid[reelIndex] = reels->GetSubReel(reelIndex, position, m_numRows);
}

void SlotGrid::PrintGrid()
{
	for (int iRow = 0; iRow < m_numRows; iRow++)
	{
		for (int iReel = 0; iReel < m_numReels; iReel++)
		{
			int symbol = m_grid[iReel][iRow];
			cout << GetSymbolColor(symbol) << GetSymbolString(symbol) << "\033[0m\t";
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

void SlotGrid::SetWays(int numSymbols, map<int, vector<double>> paytable, map<int, set<int>> symbolSubstitutions, map<int, int> symbolMultipliers)
{
	m_numSymbols = numSymbols;
	m_paytable = paytable;
	m_symbolSubstitutions = symbolSubstitutions;
	m_symbolMultipliers = symbolMultipliers;

	// Give default values to symbols not included
	for (int symbol = 0; symbol < m_numSymbols; symbol++)
	{
		if (!m_paytable.contains(symbol))
		{
			vector<double> zero_pays(m_numReels, 0);
			m_paytable[symbol] = zero_pays;
		}
		if (!m_symbolSubstitutions.contains(symbol))
		{
			m_symbolSubstitutions[symbol] = { symbol };
		}
		if (!m_symbolMultipliers.contains(symbol))
		{
			m_symbolMultipliers[symbol] = 1;
		}
	}
}

void SlotGrid::SetSymbolPrintInfo(map<int, string> symbolStrings, map<int, Colors> symbolColors)
{
	m_symbolStrings = symbolStrings;
	m_symbolColors = symbolColors;
}

void SlotGrid::SetInFreePlay(bool inFreePlay)
{
	m_inFreePlay = inFreePlay;
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

double SlotGrid::EvaluateGridLines(SymbolComboTree* &currentSymbolCombos)
{
	double score = 0;
	vector<vector<int>> grid = m_grid;
	for (const auto& line_element : m_lineElements)
	{
		line_element.EvaluateElement(score, grid, currentSymbolCombos);
	}
	return score;
}

void SlotGrid::LineElement::EvaluateElement(double& score, vector<vector<int>> &grid, SymbolComboTree* &currentSymbolCombos, size_t symbol_key) const
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

double SlotGrid::EvaluateGridLines(SymbolCombos*& currentSymbolCombos)
{
	double score = 0;
	if (m_inFreePlay)
	{
		PrintGrid();
	}
	for (int iLine = 0; iLine < m_lines.size(); iLine++)
	{
		// Calculate the Symbol Key for this line
		size_t symbolKey = 0;
		for (int iReel = 0; iReel < m_numReels; iReel++)
		{
			symbolKey += currentSymbolCombos->GetSymbolLocation(iReel, m_grid[iReel][m_lines[iLine][iReel]]);
		}
		// Grab the combo pay for this Symbol Key
		double lineScore = currentSymbolCombos->GetComboInfo(symbolKey);
		score += lineScore;
		// Print Combos
		if (m_inFreePlay && lineScore > 0)
		{
			cout << "Line " << iLine + 1 << " pays " << lineScore << "\n";
		}
	}
	if (m_inFreePlay && score > 0)
	{
		cout << "\n";
	}
	return score;
}

double SlotGrid::EvaluateGridWays()
{
	double score = 0;
	for (int iSymbol = 0; iSymbol < m_numSymbols; iSymbol++)
	{
		int numCombos = 1;
		int comboLength = 0;
		for (int iReel = 0; iReel < m_numReels; iReel++)
		{
			int symbolCount = 0;
			for (int iRow = 0; iRow < m_numRows; iRow++)
			{
				int currentSymbol = m_grid[iReel][iRow];
				if (m_symbolSubstitutions[currentSymbol].contains(iSymbol))
				{
					symbolCount += m_symbolMultipliers[currentSymbol];
				}
			}
			if (symbolCount > 0)
			{
				comboLength++;
				numCombos *= symbolCount;
			}
			else
			{
				break;
			}
		}
		if (comboLength > 0)
		{
			double symbolScore = numCombos * m_paytable[iSymbol][comboLength-1];

			if (m_inFreePlay && symbolScore > 0)
			{
				cout << comboLength << "-" << GetSymbolString(iSymbol) << " x " << numCombos << " Ways pays " << symbolScore << "\n";
			}

			score += symbolScore;
		}
	}
	return score;
}

bool SlotGrid::IsSymbolOnGrid(int symbol)
{
	for (int iRow = 0; iRow < m_numRows; iRow++)
	{
		for (int iReel = 0; iReel < m_numReels; iReel++)
		{
			if (m_grid[iReel][iRow] == symbol) return true;
		}
	}
	return false;
}

bool SlotGrid::IsSymbolOnReel(int symbol, int reelIndex)
{
	for (int iRow = 0; iRow < m_numRows; iRow++)
	{
		if (m_grid[reelIndex][iRow] == symbol) return true;
	}
	return false;
}

bool SlotGrid::IsSymbolOnRow(int symbol, int rowIndex)
{
	for (int iReel = 0; iReel < m_numReels; iReel++)
	{
		if (m_grid[iReel][rowIndex] == symbol) return true;
	}
	return false;
}

int SlotGrid::CountSymbolOnGrid(int symbol)
{
	int symbolCount = 0;
	for (int iRow = 0; iRow < m_numRows; iRow++)
	{
		for (int iReel = 0; iReel < m_numReels; iReel++)
		{
			symbolCount += m_grid[iReel][iRow] == symbol;
		}
	}
	return symbolCount;
}

int SlotGrid::CountSymbolOnReel(int symbol, int reelIndex)
{
	int symbolCount = 0;
	for (int iRow = 0; iRow < m_numRows; iRow++)
	{
		symbolCount += m_grid[reelIndex][iRow] == symbol;
	}
	return symbolCount;
}

int SlotGrid::CountSymbolOnRow(int symbol, int rowIndex)
{
	int symbolCount = 0;
	for (int iReel = 0; iReel < m_numReels; iReel++)
	{
		symbolCount += m_grid[iReel][rowIndex] == symbol;
	}
	return symbolCount;
}

void SlotGrid::FillGridWithSymbol(int symbol)
{
	for (int iRow = 0; iRow < m_numRows; iRow++)
	{
		for (int iReel = 0; iReel < m_numReels; iReel++)
		{
			m_grid[iReel][iRow] = symbol;
		}
	}
}

void SlotGrid::FillReelWithSymbol(int symbol, int reelIndex)
{
	for (int iRow = 0; iRow < m_numRows; iRow++)
	{
		m_grid[reelIndex][iRow] = symbol;
	}
}

void SlotGrid::FillRowWithSymbol(int symbol, int rowIndex)
{
	for (int iReel = 0; iReel < m_numReels; iReel++)
	{
		m_grid[iReel][rowIndex] = symbol;
	}
}

void SlotGrid::ReplaceSymbolOnGrid(int oldSymbol, int newSymbol)
{
	for (int iRow = 0; iRow < m_numRows; iRow++)
	{
		for (int iReel = 0; iReel < m_numReels; iReel++)
		{
			if (m_grid[iReel][iRow] == oldSymbol)
			{
				m_grid[iReel][iRow] = newSymbol;
			}
		}
	}
}

void SlotGrid::ReplaceSymbolOnReel(int oldSymbol, int newSymbol, int reelIndex)
{
	for (int iRow = 0; iRow < m_numRows; iRow++)
	{
		if (m_grid[reelIndex][iRow] == oldSymbol)
		{
			m_grid[reelIndex][iRow] = newSymbol;
		}
	}
}

void SlotGrid::ReplaceSymbolOnRow(int oldSymbol, int newSymbol, int rowIndex)
{
	for (int iReel = 0; iReel < m_numReels; iReel++)
	{
		if (m_grid[iReel][rowIndex] == oldSymbol)
		{
			m_grid[iReel][rowIndex] = newSymbol;
		}
	}
}

string SlotGrid::GetSymbolString(int symbol)
{
	if (m_symbolStrings.contains(symbol))
	{
		return m_symbolStrings[symbol];
	}
	else
	{
		return "S" + to_string(symbol);
	}
}

string SlotGrid::GetSymbolColor(int symbol)
{
	if (m_symbolColors.contains(symbol))
	{
		return "\033[" + to_string(m_symbolColors[symbol]) + "m";
	}
	else
	{
		return "\033[0m";
	}
}
