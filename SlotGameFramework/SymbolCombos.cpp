
#include "SymbolCombos.h"

SymbolCombos::SymbolCombos(int numReels, int numSymbols, map<int, vector<double>> paytable, map<int, set<int>> wildMapping, map<int, int> symbolMultipliers, bool bothWays, multiplierType multType)
{
	cout << "Calculating Symbol Combos... ";
	m_numReels = numReels;
	m_numSymbols = numSymbols;
	m_paytable = paytable;
	m_wildMapping = wildMapping;
	m_symbolMultipliers = symbolMultipliers;
	m_multType = multType;

	// Give default values to symbols not included
	for (int symbol = 0; symbol < m_numSymbols; symbol++)
	{
		if (!m_paytable.contains(symbol))
		{
			vector<double> zero_pays(m_numReels, 0);
			m_paytable[symbol] = zero_pays;
		}
		if (!m_wildMapping.contains(symbol))
		{
			m_wildMapping[symbol] = { symbol };
		}
		if (!m_symbolMultipliers.contains(symbol))
		{
			m_symbolMultipliers[symbol] = 1;
		}
	}

	// Set up m_combos size
	m_combos.resize(pow(m_numSymbols, m_numReels));
	m_combo_location.resize(numReels);
	for (int iReel = 0; iReel < numReels; iReel++)
	{
		m_combo_location.at(iReel).resize(numSymbols);
		for (int currentSymbol = 0; currentSymbol < numSymbols; currentSymbol++)
		{
			m_combo_location[iReel][currentSymbol] = currentSymbol * pow(numSymbols, iReel);
		}
	}

	// Calculate all possible symbol combos
	EvaluateSymbolCombos();
	if (bothWays)
	{
		vector<double> m_combosCopy = m_combos;
		for (int iCombo = 0; iCombo < m_combos.size(); iCombo++)
		{
			vector<int> combo = ChangeBase(iCombo, m_numSymbols, m_numReels);
			reverse(combo.begin(), combo.end());
			size_t symbolKey = 0;
			for (int iReel = 0; iReel < m_numReels; iReel++)
			{
				symbolKey += GetSymbolLocation(iReel, combo[iReel]);
			}
			m_combos[iCombo] = max(m_combosCopy[iCombo], m_combosCopy[symbolKey]);
		}
	}
	cout << "Done!\n";
}

SymbolCombos::~SymbolCombos()
{
	m_combos.clear();
}

double SymbolCombos::GetComboInfo(size_t symbolkey)
{
	return m_combos[symbolkey];
}

size_t SymbolCombos::GetSymbolLocation(const int reel, const int symbol)
{
	return m_combo_location[reel][symbol];
}

set<int> SymbolCombos::SetIntersect(set<int> seta, set<int> setb)
{
	set<int> return_set;
	set_intersection(seta.begin(), seta.end(), setb.begin(), setb.end(), inserter(return_set, return_set.begin()));
	return return_set;
}

void SymbolCombos::EvaluateSymbolCombos(int reel, double pay, int multiplier, size_t symbol_key, set<int> possible_symbols)
{
	for (size_t symbol = 0; symbol < m_numSymbols; symbol++)
	{
		size_t current_symbol_key = symbol_key + symbol * pow(m_numSymbols, reel);
		double max_pay = pay;
		int mult = multiplier;
		// Update Wild Symbols
		set<int> current_possible_symbols;
		if (reel == 0)
		{
			current_possible_symbols = m_wildMapping[symbol];
		}
		else
		{
			current_possible_symbols = SetIntersect(possible_symbols, m_wildMapping[symbol]);
		}
		// If combo is not broken, update the current combo pay and multiplier
		if (!current_possible_symbols.empty())
		{
			// Update Multiplier
			switch (m_multType) {
			case PRODUCT:
				mult *= m_symbolMultipliers[symbol]; break;
			case SUM:
				mult += m_symbolMultipliers[symbol] - (mult == 1 || m_symbolMultipliers[symbol] == 1); break;
			case MAX:
				mult = max(mult, m_symbolMultipliers[symbol]); break;
			}
			// Update Pay
			for (int s : current_possible_symbols)
			{
				if (m_paytable[s][reel] > max_pay)
				{
					max_pay = m_paytable[s][reel];
				}
			}
		}
		// Check if this is the final reel
		if (reel + 1 < m_numReels)
		{
			EvaluateSymbolCombos(reel + 1, max_pay, mult, current_symbol_key, current_possible_symbols);
		}
		else
		{
			m_combos[current_symbol_key] = max_pay * mult;
		}
	}
}