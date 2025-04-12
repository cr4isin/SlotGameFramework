
#include "SlotGrid.h"


// ===================== Standard Grid Functions ===================== 
SlotGrid::SlotGrid()
{
	// Empty Default Constructor
}

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

void SlotGrid::SetSymbol(int symbol, int reelIndex, int rowIndex)
{
	m_grid[reelIndex][rowIndex] = symbol;
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

void SlotGrid::FillGrid(vector<int> &positions, SlotReels &reels)
{
	for (int iReel = 0; iReel < m_numReels; iReel++)
	{
		FillGridReel(iReel, positions[iReel], reels);
	}
}

void SlotGrid::FillGridReel(int reelIndex, int position, SlotReels &reels)
{
	reels.FillReel(m_grid[reelIndex], reelIndex, position, m_numRows);
}

void SlotGrid::PrintGrid()
{
	for (int iRow = 0; iRow < m_numRows; iRow++)
	{
		for (int iReel = 0; iReel < m_numReels; iReel++)
		{
			int symbol = m_grid[iReel][iRow];
			cout << GetSymbolColor(symbol) << FormatString(GetSymbolString(symbol), m_maxSymbolLength) << "\033[0m\t";
		}
		cout << "\n";
	}
	cout << "\n";
}

// ===================== Grid Evaluation ===================== 
void SlotGrid::SetLines(vector<vector<int>> lines, int numLines)
{
	m_lines.clear();
	if (numLines <= 0)
	{
		numLines = lines.size();
	}
	for (int iLine = 0; iLine < numLines; iLine++)
	{
		m_lines.push_back(lines.at(iLine));
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
			m_paytable[symbol] = vector<double>(m_numReels, 0);
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

	// Create the symbol substitution count for each symbol
	m_symbolSubstitutionCount.resize(numSymbols);
	for (int iSymbol = 0; iSymbol < numSymbols; iSymbol++)
	{
		m_symbolSubstitutionCount[iSymbol].resize(numSymbols);
		for (int currentSymbol = 0; currentSymbol < numSymbols; currentSymbol++)
		{
			if (m_symbolSubstitutions[currentSymbol].contains(iSymbol))
			{
				m_symbolSubstitutionCount[iSymbol][currentSymbol] = m_symbolMultipliers[currentSymbol];
			}
			else
			{
				m_symbolSubstitutionCount[iSymbol][currentSymbol] = 0;
			}
		}
	}
}

void SlotGrid::SetSymbolPrintInfo(map<int, string> symbolStrings, map<int, Colors> symbolColors)
{
	m_symbolStrings = symbolStrings;
	m_symbolColors = symbolColors;

	m_maxSymbolLength = 3;
	for (auto const& [symbol, name] : symbolStrings)
	{
		m_maxSymbolLength = max(m_maxSymbolLength, name.size());
	}
}

void SlotGrid::SetInFreePlay(bool inFreePlay)
{
	m_inFreePlay = inFreePlay;
}

double SlotGrid::EvaluateLines(PaylineCombo& symbolCombos, int multiplier)
{
	int bonusCode = 0;
	return EvaluateLines(symbolCombos, multiplier, bonusCode);
}

double SlotGrid::EvaluateLines(PaylineCombo& symbolCombos, int multiplier, int& bonusCount)
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
			symbolKey += symbolCombos.GetSymbolKey(iReel, m_grid[iReel][m_lines[iLine][iReel]]);
		}
		// Grab the combo pay for this Symbol Key
		double lineScore = 0;
		int bonusCode = 0;
		symbolCombos.GetCombo(symbolKey);
		score += lineScore;
		bonusCount += bonusCode; // Add custom logic for bonus codes here
		// Print Combos
		if (m_inFreePlay && lineScore > 0)
		{
			cout << "Line " << iLine + 1 << " pays " << lineScore * multiplier << "\n";
		}
	}
	if (m_inFreePlay && score > 0)
	{
		cout << "\n";
	}
	return score * multiplier;
}

double SlotGrid::EvaluateWays(AnywaysCombo& symbolCombos, int multiplier)
{
	double score = 0;
	if (m_inFreePlay)
	{
		PrintGrid();
	}
	for (int iSymbol = 0; iSymbol < m_numSymbols; iSymbol++)
	{
		int comboLength = 0;
		int numCombos = 1;
		for (int iReel = 0; iReel < m_numReels; iReel++)
		{
			int symbolCount = 0;
			for (int iRow = 0; iRow < m_numRows; iRow++)
			{
				int currentSymbol = m_grid[iReel][iRow];
				symbolCount += m_symbolSubstitutionCount[iSymbol][currentSymbol];
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
				cout << comboLength << "-" << GetSymbolColor(iSymbol) << GetSymbolString(iSymbol) << "\033[0m x " << numCombos << " Ways pays " << symbolScore * multiplier << "\n";
			}

			score += symbolScore;
		}
	}
	if (m_inFreePlay && score > 0)
	{
		cout << "\n";
	}
	return score * multiplier;
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
		return "S" + FormatInt(symbol, 2, '0');
	}
}

string SlotGrid::GetSymbolColor(int symbol)
{
	if (m_symbolColors.contains(symbol))
	{
		int color = m_symbolColors[symbol];
		return "\033[" + to_string(38 + 10 * (color / 256)) + ";5;" + to_string(color % 256) + "m";
	}
	else
	{
		return "\033[0m";
	}
}
