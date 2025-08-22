
#include "SlotGrid.h"

// ===================== Standard Grid Functions ===================== 
SlotGrid::SlotGrid(int numReels, int numRows)
	: numReels(numReels),
	numRows(numRows)
{
	grid.resize(numReels, vector<int>(numRows));
	highlights.resize(numReels, vector<bool>(numRows));
}

int SlotGrid::GetSymbol(int reelIndex, int rowIndex)
{
	return grid[reelIndex][rowIndex];
}

void SlotGrid::SetSymbol(int symbol, int reelIndex, int rowIndex)
{
	grid[reelIndex][rowIndex] = symbol;
}

vector<vector<int>> SlotGrid::GetGrid()
{
	return grid;
}

int SlotGrid::GetNumReels()
{
	return numReels;
}

int SlotGrid::GetNumRows()
{
	return numRows;
}

void SlotGrid::FillGrid(vector<int> &positions, SlotReels &reels)
{
	for (int iReel = 0; iReel < numReels; iReel++)
	{
		FillGridReel(iReel, positions[iReel], reels);
	}
}

void SlotGrid::FillGridReel(int reelIndex, int position, SlotReels &reels)
{
	reels.FillReel(grid[reelIndex], reelIndex, position, numRows);
}

void SlotGrid::PrintGrid(SymbolSet& symbolSet)
{
	for (int iRow = 0; iRow < numRows; iRow++)
	{
		for (int iReel = 0; iReel < numReels; iReel++)
		{
			int symbol = grid[iReel][iRow];
			cout << ANSI::Color(symbolSet.GetColor(symbol)) << F("{:^{}}", symbolSet.GetSymbolString(symbol), symbolSet.GetMaxSymbolLength()) << ANSI::Reset() << "\t";
		}
		cout << "\n";
	}
	cout << "\n";
}

void SlotGrid::PrintGridTimed(SymbolSet& symbolSet, int delayInMillis)
{
	cout << ANSI::HideCursor();
	int textWidth = symbolSet.GetMaxSymbolLength();
	// Create empty space
	for (int iRow = 0; iRow < numRows + 1; iRow++) cout << "\n";
	cout << ANSI::MoveCursorUp(numRows + 1);

	// Fill in reel by reel
	for (int iReel = 0; iReel < numReels; iReel++)
	{
		for (int iRow = 0; iRow < numRows; iRow++)
		{
			int symbol = grid[iReel][iRow];
			if (highlights[iReel][iRow]) cout << ANSI::Blink();
			cout << ANSI::Color(symbolSet.GetColor(symbol)) << F("{:^{}}", symbolSet.GetSymbolString(symbol), textWidth)
				<< ANSI::Reset() << ANSI::MoveCursorDown(1) << ANSI::MoveCursorLeft(textWidth);
		}
		cout << ANSI::MoveCursorUp(numRows) << ANSI::MoveCursorRight(textWidth + 3) << flush;
		if (iReel + 1 < numReels) this_thread::sleep_for(chrono::milliseconds(delayInMillis));
	}
	cout << ANSI::NextLine(numRows+1) << ANSI::ShowCursor();
}

void SlotGrid::ClearHighlights()
{
	for (int iReel = 0; iReel < numReels; iReel++)
	{
		for (int iRow = 0; iRow < numRows; iRow++)
		{
			highlights[iReel][iRow] = false;
		}
	}
}

void SlotGrid::AddHighlights(PaylineCombo& paylineCombo)
{
	for (int iLine = 0; iLine < lines.size(); iLine++)
	{
		const Combo& combo = paylineCombo.combos[iLine];
		if (combo.pay > 0 || combo.bonusCode > 0 || combo.progressive > 0)
		{
			int length = combo.length;
			for (int iReel = 0; iReel < length; iReel++)
			{
				highlights[iReel][lines[iLine][iReel]] = true;
			}
		}
	}
}

void SlotGrid::SetHighlights(PaylineCombo& paylineCombo)
{
	ClearHighlights();
	AddHighlights(paylineCombo);
}

void SlotGrid::AddHighlights(AnywaysCombo& anywaysCombo)
{
	for (int iSymbol = 0; iSymbol < anywaysCombo.GetNumSymbols(); iSymbol++)
	{
		const Combo& combo = anywaysCombo.combos[iSymbol];
		if (combo.pay > 0 || combo.bonusCode > 0 || combo.progressive > 0)
		{
			int length = combo.length;
			for (int iReel = 0; iReel < length; iReel++)
			{
				for (int iRow = 0; iRow < numRows; iRow++)
				{
					if (anywaysCombo.GetSymbolSubstitutionCount(iSymbol, GetSymbol(iReel, iRow)) > 0)
					{
						highlights[iReel][iRow] = true;
					}
				}
			}
		}
	}
}

void SlotGrid::SetHighlights(AnywaysCombo& anywaysCombo)
{
	ClearHighlights();
	AddHighlights(anywaysCombo);
}

// ===================== Grid Evaluation ===================== 
void SlotGrid::SetLines(vector<vector<int>> lines, int numLines)
{
	this->lines.clear();
	if (numLines <= 0)
	{
		numLines = lines.size();
	}
	for (int iLine = 0; iLine < numLines; iLine++)
	{
		this->lines.push_back(lines.at(iLine));
	}
}

void SlotGrid::Evaluate(PaylineCombo& paylineCombo)
{
	paylineCombo.ClearResults();
	for (int iLine = 0; iLine < lines.size(); iLine++)
	{
		size_t symbolKey = 0;
		for (int iReel = 0; iReel < numReels; iReel++)
		{
			symbolKey += paylineCombo.GetSymbolKey(iReel, grid[iReel][lines[iLine][iReel]]);
		}
		paylineCombo.AddResult(symbolKey, iLine);
	}
}

void SlotGrid::Evaluate(AnywaysCombo& anywaysCombo)
{
	anywaysCombo.ClearResults();
	for (int iSymbol = 0; iSymbol < anywaysCombo.GetNumSymbols(); iSymbol++)
	{
		int comboLength = 0;
		int numCombos = 1;
		for (int iReel = 0; iReel < numReels; iReel++)
		{
			int symbolCount = 0;
			for (int iRow = 0; iRow < numRows; iRow++)
			{
				int currentSymbol = grid[iReel][iRow];
				symbolCount += anywaysCombo.GetSymbolSubstitutionCount(iSymbol, currentSymbol);
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
		anywaysCombo.AddResult(iSymbol, comboLength, numCombos);
	}
}

void SlotGrid::Evaluate(ScatterCombo& scatterCombo)
{
	scatterCombo.ClearResults();
	vector<bool> reelsHit(numReels, false);
	const vector<int>& scatterSymbols = scatterCombo.GetScatterSymbols();

	for (int iReel = 0; iReel < numReels; iReel++)
	{
		reelsHit[iReel] = IsSymbolOnReel(scatterSymbols[iReel], iReel);
	}

	int comboKey = scatterCombo.GetComboKey(reelsHit);
	scatterCombo.AddResult(comboKey);
}


void SlotGrid::Evaluate(CountScatterCombo& countScatterCombo)
{
	countScatterCombo.ClearResults();
	int symbolToCount = countScatterCombo.GetScatterSymbol();
	int matchCount = CountSymbolOnGrid(symbolToCount);
	countScatterCombo.AddResult(matchCount);
}

bool SlotGrid::IsSymbolOnGrid(int symbol)
{
	for (int iRow = 0; iRow < numRows; iRow++)
	{
		for (int iReel = 0; iReel < numReels; iReel++)
		{
			if (grid[iReel][iRow] == symbol) return true;
		}
	}
	return false;
}

bool SlotGrid::IsSymbolOnReel(int symbol, int reelIndex)
{
	for (int iRow = 0; iRow < numRows; iRow++)
	{
		if (grid[reelIndex][iRow] == symbol) return true;
	}
	return false;
}

bool SlotGrid::IsSymbolOnRow(int symbol, int rowIndex)
{
	for (int iReel = 0; iReel < numReels; iReel++)
	{
		if (grid[iReel][rowIndex] == symbol) return true;
	}
	return false;
}

int SlotGrid::CountSymbolOnGrid(int symbol)
{
	int symbolCount = 0;
	for (int iRow = 0; iRow < numRows; iRow++)
	{
		for (int iReel = 0; iReel < numReels; iReel++)
		{
			symbolCount += grid[iReel][iRow] == symbol;
		}
	}
	return symbolCount;
}

int SlotGrid::CountSymbolOnReel(int symbol, int reelIndex)
{
	int symbolCount = 0;
	for (int iRow = 0; iRow < numRows; iRow++)
	{
		symbolCount += grid[reelIndex][iRow] == symbol;
	}
	return symbolCount;
}

int SlotGrid::CountSymbolOnRow(int symbol, int rowIndex)
{
	int symbolCount = 0;
	for (int iReel = 0; iReel < numReels; iReel++)
	{
		symbolCount += grid[iReel][rowIndex] == symbol;
	}
	return symbolCount;
}

void SlotGrid::FillGridWithSymbol(int symbol)
{
	for (int iRow = 0; iRow < numRows; iRow++)
	{
		for (int iReel = 0; iReel < numReels; iReel++)
		{
			grid[iReel][iRow] = symbol;
		}
	}
}

void SlotGrid::FillReelWithSymbol(int symbol, int reelIndex)
{
	for (int iRow = 0; iRow < numRows; iRow++)
	{
		grid[reelIndex][iRow] = symbol;
	}
}

void SlotGrid::FillRowWithSymbol(int symbol, int rowIndex)
{
	for (int iReel = 0; iReel < numReels; iReel++)
	{
		grid[iReel][rowIndex] = symbol;
	}
}

void SlotGrid::ReplaceSymbolOnGrid(int oldSymbol, int newSymbol)
{
	for (int iRow = 0; iRow < numRows; iRow++)
	{
		for (int iReel = 0; iReel < numReels; iReel++)
		{
			if (grid[iReel][iRow] == oldSymbol)
			{
				grid[iReel][iRow] = newSymbol;
			}
		}
	}
}

void SlotGrid::ReplaceSymbolOnReel(int oldSymbol, int newSymbol, int reelIndex)
{
	for (int iRow = 0; iRow < numRows; iRow++)
	{
		if (grid[reelIndex][iRow] == oldSymbol)
		{
			grid[reelIndex][iRow] = newSymbol;
		}
	}
}

void SlotGrid::ReplaceSymbolOnRow(int oldSymbol, int newSymbol, int rowIndex)
{
	for (int iReel = 0; iReel < numReels; iReel++)
	{
		if (grid[iReel][rowIndex] == oldSymbol)
		{
			grid[iReel][rowIndex] = newSymbol;
		}
	}
}

void SlotGrid::Replace(MysteryReplacement& mysteryReplacement)
{
	for (const auto& instruction: mysteryReplacement.GetRandomInstructions())
	{
		if (instruction.reel == -1)
		{
			ReplaceSymbolOnGrid(instruction.originalSymbol, instruction.newSymbol);
		}
		else
		{
			ReplaceSymbolOnReel(instruction.originalSymbol, instruction.newSymbol, instruction.reel);
		}
	}
}
