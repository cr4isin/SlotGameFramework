
#include "SymbolCombos.h"

SymbolCombos::SymbolCombos()
{
	// Empty Default Constructor
}

SymbolCombos::SymbolCombos(int numReels, int numSymbols, map<int, vector<double>> paytable, map<int, set<int>> wildMapping, map<int, int> symbolMultipliers)
{
	m_numReels = numReels;
	m_numSymbols = numSymbols;
	m_paytable = paytable;
	m_wildMapping = wildMapping;
	m_symbolMultipliers = symbolMultipliers;

	// Give default values to symbols not included
	for (int symbol = 0; symbol < m_numSymbols; symbol++)
	{
		if (!m_paytable.contains(symbol))
		{
			m_paytable[symbol] = vector<double>(m_numReels, 0);
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

	// Create the Inverse Wild Mapping
	for (const auto& [symbol, subSymbolSet] : m_wildMapping)
	{
		for (int subSymbol : subSymbolSet)
		{
			m_inverseWildMapping[subSymbol].insert(symbol);
		}
	}

	// Set up m_combos size and m_combo_location
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
}

SymbolCombos::SymbolCombos(int numReels, int numSymbols, map<int, set<int>> wildMapping, map<int, int> symbolMultipliers)
{
	m_numReels = numReels;
	m_numSymbols = numSymbols;
	m_wildMapping = wildMapping;
	m_symbolMultipliers = symbolMultipliers;

	// Give default values to symbols not included
	for (int symbol = 0; symbol < m_numSymbols; symbol++)
	{
		if (!m_paytable.contains(symbol))
		{
			m_paytable[symbol] = vector<double>(m_numReels, 0);
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

	// Create the Inverse Wild Mapping
	for (const auto& [symbol, subSymbolSet] : m_wildMapping)
	{
		for (int subSymbol : subSymbolSet)
		{
			m_inverseWildMapping[subSymbol].insert(symbol);
		}
	}

	// Set up m_combos size and m_combo_location
	m_combos.resize(pow(m_numSymbols, m_numReels));
	m_combo_location.resize(m_numReels);
	for (int iReel = 0; iReel < m_numReels; iReel++)
	{
		m_combo_location.at(iReel).resize(m_numSymbols);
		for (int currentSymbol = 0; currentSymbol < m_numSymbols; currentSymbol++)
		{
			m_combo_location[iReel][currentSymbol] = currentSymbol * pow(m_numSymbols, iReel);
		}
	}
}

void SymbolCombos::GetComboInfo(size_t symbolkey, double& pay, int& bonusCode)
{
	Combo& combo = m_combos[symbolkey];
	pay = combo.totalPay;
	bonusCode = combo.bonusCode;
}

size_t SymbolCombos::GetSymbolLocation(const int reel, const int symbol)
{
	return m_combo_location[reel][symbol];
}

void SymbolCombos::SetCombo(vector<int> combo, double pay, int bonusCode, bool overWrite)
{
	vector<int> symbolKeys = { 0 };
	for (int iReel = 0; iReel < combo.size(); iReel++)
	{
		vector<int> newSymbolKeys;
		int symbol = combo[iReel];

		if (symbol < 0) // ANY
		{
			for (int symbolKey : symbolKeys)
			{
				for (int subSymbol = 0; subSymbol < m_numSymbols; subSymbol++)
				{
					newSymbolKeys.push_back(symbolKey + m_combo_location[iReel][subSymbol]);
				}
			}
		}
		else
		{
			for (int symbolKey : symbolKeys)
			{
				for (int subSymbol : m_inverseWildMapping[symbol])
				{
					newSymbolKeys.push_back(symbolKey + m_combo_location[iReel][subSymbol]);
				}
			}
		}

		symbolKeys = newSymbolKeys;
	}
	for (int symbolKey : symbolKeys)
	{
		vector<int> currentCombo = ChangeBase(symbolKey, m_numSymbols, m_numReels);
		int multiplier = 1;
		for (int symbol : currentCombo)
		{
			multiplier *= m_symbolMultipliers[symbol];
		}
		Combo& comboInfo = m_combos[symbolKey];
		if (!comboInfo.set || overWrite)
		{
			comboInfo.basePay = pay;
			comboInfo.multiplier = multiplier;
			comboInfo.totalPay = comboInfo.basePay * comboInfo.multiplier;
			comboInfo.bonusCode = bonusCode;
			comboInfo.set = true;
		}
	}
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
			mult *= m_symbolMultipliers[symbol];
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
			m_combos[current_symbol_key] = Combo(max_pay, max_pay * mult, mult, 0);
		}
	}
}