
#include "MultiSymbolComboInfo.h"

MultiSymbolComboInfo::MultiSymbolComboInfo(int numReels, int numSymbols, map<int, vector<double>> paytable, map<int, set<int>> wildMapping, map<int, int> symbolMultipliers, multiplierType multType)
{
	cout << "Calculating Multi-Symbol Combos... ";
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

	// Set up m_combos size and m_combo_location size/values
	m_combos.resize(numReels);
	m_combo_location.resize(numReels);
	for (int iReel = 0; iReel < numReels; iReel++)
	{
		m_combos.at(iReel).resize(pow(numSymbols, iReel + 1));
		m_combo_location.at(iReel).resize(numSymbols);
		for (int currentSymbol = 0; currentSymbol < numSymbols; currentSymbol++)
		{
			m_combo_location[iReel][currentSymbol] = currentSymbol * pow(numSymbols, iReel);
		}
	}

	// Calculate all possible symbol combos
	EvaluateSymbolCombos();
	cout << "Done!\n";
}

MultiSymbolComboInfo::~MultiSymbolComboInfo()
{
	m_combos.clear();
	m_combo_location.clear();
}

set<int> MultiSymbolComboInfo::SetIntersect(set<int> seta, set<int> setb)
{
	set<int> return_set;
	set_intersection(seta.begin(), seta.end(), setb.begin(), setb.end(), inserter(return_set, return_set.begin()));
	return return_set;
}

void MultiSymbolComboInfo::EvaluateSymbolCombos(int reel, double pay, int multiplier, size_t symbol_key, set<int> possible_symbols)
{
	for (size_t symbol = 0; symbol < m_numSymbols; symbol++)
	{
		size_t current_symbol_key = symbol_key + m_combo_location[reel][symbol];
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
		// Check for combo breaking
		if (current_possible_symbols.empty())
		{
			m_combos[reel][current_symbol_key].breaks = true;
			m_combos[reel][current_symbol_key].pay = max_pay * mult;
		}
		else
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
			m_combos[reel][current_symbol_key].pay = max_pay * mult;
			// Check if this is the final reel or the combo continues
			if (reel + 1 >= m_numReels)
			{
				m_combos[reel][current_symbol_key].breaks = true;
			}
			else
			{
				EvaluateSymbolCombos(reel + 1, max_pay, mult, current_symbol_key, current_possible_symbols);
			}
		}
	}
}