
#include "MathXML.h"

MathXML::MathXML(const std::string filename) {
    LoadFile(filename);
}

MathXML::~MathXML() {}

void MathXML::LoadFile(const std::string filename) {
    this->filename = filename;
    tinyxml2::XMLError eResult = doc.LoadFile(filename.c_str());
    if (eResult != tinyxml2::XML_SUCCESS) {
        std::cerr << "Error loading XML file: " << filename << " Error Code: " << eResult << std::endl;
    }
}

std::string MathXML::GetConfigName() const {
    auto* root = doc.FirstChildElement("GameMath");

    std::string gameID = root->FirstChildElement("GameId")->GetText();
    double pct = root->FirstChildElement("GamePct")->DoubleText(0.0);

    // Convert to integer by multiplying by 10, rounding, then truncate
    int pctInt = static_cast<int>(pct * 10 + 0.5); // Round to nearest whole number

    return gameID + "_" + to_string(pctInt);
}

WeightTable MathXML::GetWeightTable(const std::string& identifier) {
    auto* root = doc.FirstChildElement("GameMath");
    auto* bonusInfo = root->FirstChildElement("BonusInfo");
    auto* weightedTableList = bonusInfo->FirstChildElement("WeightedTableList");

    for (auto* table = weightedTableList->FirstChildElement("WeightedTable");
        table != nullptr;
        table = table->NextSiblingElement("WeightedTable")) {

        std::string id = table->FirstChildElement("Identifier")->GetText();
        if (id != identifier) continue;

        std::vector<long long> weights;
        std::vector<double> values;

        for (auto* element = table->FirstChildElement("WeightedElementList")->FirstChildElement("WeightedElement");
            element != nullptr;
            element = element->NextSiblingElement("WeightedElement")) {

            long long weight = element->FirstChildElement("Weight")->Int64Text(0);
            double value = element->FirstChildElement("Value")->DoubleText(0.0);
            weights.push_back(weight);
            values.push_back(value);
        }

        return WeightTable(weights, values);
    }

    return WeightTable({}, {});
}

void MathXML::GetAllWeightTables(std::map<std::string, WeightTable>& tableMap,
    const std::string& filter) {
    auto* root = doc.FirstChildElement("GameMath");
    auto* bonusInfo = root->FirstChildElement("BonusInfo");
    auto* weightedTableList = bonusInfo->FirstChildElement("WeightedTableList");

    std::regex reFilter;
    bool useRegex = !filter.empty();
    if (useRegex) reFilter = std::regex(filter);

    for (auto* table = weightedTableList->FirstChildElement("WeightedTable");
        table != nullptr;
        table = table->NextSiblingElement("WeightedTable")) {

        std::string id = table->FirstChildElement("Identifier")->GetText();
        if (useRegex && !std::regex_search(id, reFilter)) continue;

        tableMap.emplace(id, GetWeightTable(id));
    }
}

std::vector<double> MathXML::GetValueTable(const std::string& identifier) {
    auto* root = doc.FirstChildElement("GameMath");
    auto* bonusInfo = root->FirstChildElement("BonusInfo");
    auto* valueTableList = bonusInfo->FirstChildElement("ValueTableList");

    for (auto* table = valueTableList->FirstChildElement("ValueTable");
        table != nullptr;
        table = table->NextSiblingElement("ValueTable")) {

        std::string id = table->FirstChildElement("Identifier")->GetText();
        if (id != identifier) continue;

        std::vector<double> values;
        for (auto* valueElement = table->FirstChildElement("ValueList")->FirstChildElement("Value");
            valueElement != nullptr;
            valueElement = valueElement->NextSiblingElement("Value")) {

            double value = valueElement->DoubleText(0.0);
            values.push_back(value);
        }

        return values;
    }

    return {};
}

void MathXML::GetAllValueTables(std::map<std::string, std::vector<double>>& tableMap,
    const std::string& filter) {
    auto* root = doc.FirstChildElement("GameMath");
    auto* bonusInfo = root->FirstChildElement("BonusInfo");
    auto* valueTableList = bonusInfo->FirstChildElement("ValueTableList");

    std::regex reFilter;
    bool useRegex = !filter.empty();
    if (useRegex) reFilter = std::regex(filter);

    for (auto* table = valueTableList->FirstChildElement("ValueTable");
        table != nullptr;
        table = table->NextSiblingElement("ValueTable")) {

        std::string id = table->FirstChildElement("Identifier")->GetText();
        if (useRegex && !std::regex_search(id, reFilter)) continue;

        tableMap.emplace(id, GetValueTable(id));
    }
}

SymbolSet MathXML::GetSymbolSet(const std::string& identifier,
    const std::map<std::string, std::set<std::string>>& wilds,
    const std::map<std::string, int>& multipliers,
    const std::map<std::string, Colors>& colors) {
    auto* root = doc.FirstChildElement("GameMath");
    auto* symbolSetList = root->FirstChildElement("SymbolSetList");

    for (auto* set = symbolSetList->FirstChildElement("SymbolSet");
        set != nullptr;
        set = set->NextSiblingElement("SymbolSet")) {

        std::string id = set->FirstChildElement("Identifier")->GetText();
        if (id != identifier) continue;

        std::vector<std::string> symbols;
        auto* symbolList = set->FirstChildElement("SymbolList");

        for (auto* symbolElem = symbolList->FirstChildElement("Symbol");
            symbolElem != nullptr;
            symbolElem = symbolElem->NextSiblingElement("Symbol")) {
            symbols.emplace_back(symbolElem->GetText());
        }

        std::map<std::string, std::set<std::string>> usedWilds = wilds;
        if (wilds.empty()) {
            auto* wildList = set->FirstChildElement("WildSymbolList");
            if (wildList) {
                for (auto* wildElem = wildList->FirstChildElement("WildSymbol");
                    wildElem != nullptr;
                    wildElem = wildElem->NextSiblingElement("WildSymbol")) {

                    std::string wildName = wildElem->FirstChildElement("Identifier")->GetText();
                    std::set<std::string> subs;

                    auto* subsList = wildElem->FirstChildElement("SymbolList");
                    for (auto* sub = subsList->FirstChildElement("Symbol");
                        sub != nullptr;
                        sub = sub->NextSiblingElement("Symbol")) {
                        subs.insert(sub->GetText());
                    }

                    usedWilds[wildName] = subs;
                }
            }
        }

        return SymbolSet(symbols, usedWilds, multipliers, colors);
    }

    throw std::runtime_error("SymbolSet not found: " + identifier);
}

void MathXML::GetAllSymbolSets(std::map<std::string, SymbolSet>& symbolSetMap,
    const std::map<std::string, std::set<std::string>>& wilds,
    const std::map<std::string, int>& multipliers,
    const std::map<std::string, Colors>& colors,
    const std::string& filter) {
    auto* root = doc.FirstChildElement("GameMath");
    auto* symbolSetList = root->FirstChildElement("SymbolSetList");

    std::regex reFilter;
    bool useRegex = !filter.empty();
    if (useRegex) reFilter = std::regex(filter);

    for (auto* set = symbolSetList->FirstChildElement("SymbolSet");
        set != nullptr;
        set = set->NextSiblingElement("SymbolSet")) {

        std::string id = set->FirstChildElement("Identifier")->GetText();
        if (useRegex && !std::regex_search(id, reFilter)) continue;

        symbolSetMap.emplace(id, GetSymbolSet(id, wilds, multipliers, colors));
    }
}

SlotReels MathXML::GetReelStripSet(const std::string& identifier, const SymbolSet& symbolSet) {
    auto* root = doc.FirstChildElement("GameMath");
    auto* setList = root->FirstChildElement("ReelStripSetList");

    for (auto* stripSet = setList->FirstChildElement("ReelStripSet");
        stripSet != nullptr;
        stripSet = stripSet->NextSiblingElement("ReelStripSet")) {

        std::string id = stripSet->FirstChildElement("Identifier")->GetText();
        if (id != identifier) continue;

        std::vector<std::vector<int>> reels;
        std::vector<std::vector<int>> weights;

        for (auto* stripID = stripSet->FirstChildElement("ReelStripIDList")->FirstChildElement("ReelStripID");
            stripID != nullptr;
            stripID = stripID->NextSiblingElement("ReelStripID")) {

            std::string stripName = stripID->GetText();

            auto* reelStripList = root->FirstChildElement("ReelStripList");
            for (auto* strip = reelStripList->FirstChildElement("ReelStrip");
                strip != nullptr;
                strip = strip->NextSiblingElement("ReelStrip")) {

                if (strip->FirstChildElement("Identifier")->GetText() == stripName) {
                    std::vector<int> reelSymbols, reelWeights;

                    for (auto* elem = strip->FirstChildElement("WeightedElementList")->FirstChildElement("WeightedElement");
                        elem != nullptr;
                        elem = elem->NextSiblingElement("WeightedElement")) {

                        std::string symbol = elem->FirstChildElement("StringValue")->GetText();
                        int weight = elem->FirstChildElement("Weight")->IntText(1);
                        reelSymbols.push_back(symbolSet.GetSymbolIndex(symbol));
                        reelWeights.push_back(weight);
                    }

                    reels.push_back(reelSymbols);
                    weights.push_back(reelWeights);
                    break;
                }
            }
        }

        return SlotReels(reels, weights);
    }

    throw std::runtime_error("ReelStripSet not found: " + identifier);
}

void MathXML::GetAllReelStripSets(std::map<std::string, SlotReels>& reelSetMap,
    const SymbolSet& symbolSet,
    const std::string& filter) {
    auto* root = doc.FirstChildElement("GameMath");
    auto* setList = root->FirstChildElement("ReelStripSetList");

    std::regex reFilter;
    bool useRegex = !filter.empty();
    if (useRegex) reFilter = std::regex(filter);

    for (auto* stripSet = setList->FirstChildElement("ReelStripSet");
        stripSet != nullptr;
        stripSet = stripSet->NextSiblingElement("ReelStripSet")) {

        std::string id = stripSet->FirstChildElement("Identifier")->GetText();
        if (useRegex && !std::regex_search(id, reFilter)) continue;

        reelSetMap.emplace(id, GetReelStripSet(id, symbolSet));
    }
}

WeightTable MathXML::GetReelStripAsWeightTable(const std::string& identifier, const SymbolSet& symbolSet) {
    auto* root = doc.FirstChildElement("GameMath");
    auto* reelStripList = root->FirstChildElement("ReelStripList");

    for (auto* strip = reelStripList->FirstChildElement("ReelStrip");
        strip != nullptr;
        strip = strip->NextSiblingElement("ReelStrip")) {

        std::string id = strip->FirstChildElement("Identifier")->GetText();
        if (id != identifier) continue;

        std::vector<long long> weights;
        std::vector<double> values;

        for (auto* elem = strip->FirstChildElement("WeightedElementList")->FirstChildElement("WeightedElement");
            elem != nullptr;
            elem = elem->NextSiblingElement("WeightedElement")) {

            std::string symbol = elem->FirstChildElement("StringValue")->GetText();
            long long weight = elem->FirstChildElement("Weight")->Int64Text(1);
            int symbolIndex = symbolSet.GetSymbolIndex(symbol);

            weights.push_back(weight);
            values.push_back(static_cast<double>(symbolIndex));
        }

        return WeightTable(weights, values);
    }

    throw std::runtime_error("ReelStrip not found: " + identifier);
}

void MathXML::GetAllReelStripsAsWeightTables(std::map<std::string, WeightTable>& tableMap,
    const SymbolSet& symbolSet,
    const std::string& filter) {
    auto* root = doc.FirstChildElement("GameMath");
    auto* reelStripList = root->FirstChildElement("ReelStripList");

    std::regex reFilter;
    bool useRegex = !filter.empty();
    if (useRegex) reFilter = std::regex(filter);

    for (auto* strip = reelStripList->FirstChildElement("ReelStrip");
        strip != nullptr;
        strip = strip->NextSiblingElement("ReelStrip")) {

        std::string id = strip->FirstChildElement("Identifier")->GetText();
        if (useRegex && !std::regex_search(id, reFilter)) continue;

        tableMap.emplace(id, GetReelStripAsWeightTable(id, symbolSet));
    }
}

PaylineCombo MathXML::GetPaylineComboSet(const std::string& identifier,
    const SymbolSet& symbolSet,
    int numReels,
    int numLines,
    double payMultiplier) const
{
    PaylineCombo comboSet(numReels, numLines, symbolSet);

    auto* root = doc.FirstChildElement("GameMath");
    auto* comboSetList = root->FirstChildElement("ComboSetList");

    for (auto* paylineComboSet = comboSetList->FirstChildElement("PaylineComboSet");
        paylineComboSet != nullptr;
        paylineComboSet = paylineComboSet->NextSiblingElement("PaylineComboSet"))
    {
        std::string id = paylineComboSet->FirstChildElement("Identifier")->GetText();
        if (id != identifier) continue;

        auto* comboList = paylineComboSet->FirstChildElement("PaylineComboList");
        for (auto* combo = comboList->FirstChildElement("PaylineCombo");
            combo != nullptr;
            combo = combo->NextSiblingElement("PaylineCombo"))
        {
            std::vector<int> symbolCombo;
            for (auto* symbolElem = combo->FirstChildElement("SymbolList")->FirstChildElement("Symbol");
                symbolElem != nullptr;
                symbolElem = symbolElem->NextSiblingElement("Symbol"))
            {
                std::string symbolStr = symbolElem->GetText();
                symbolCombo.push_back(symbolStr == "ANY" ? -1 : symbolSet.GetSymbolIndex(symbolStr));
            }

            double pay = std::stod(combo->FirstChildElement("Value")->GetText()) * payMultiplier;

            int bonusCode = 0;
            if (auto* bonusElem = combo->FirstChildElement("BonusCode"))
                bonusCode = std::stoi(bonusElem->GetText());

            int progressive = 0;
            if (auto* progElem = combo->FirstChildElement("Progressive"))
                progressive = std::stoi(progElem->GetText());

            comboSet.SetCombo(symbolCombo, pay, bonusCode, progressive);
        }

        break; // Found the right set and loaded it
    }

    return comboSet;
}

void MathXML::GetAllPaylineComboSets(std::map<std::string, PaylineCombo>& comboMap,
    const SymbolSet& symbolSet,
    int numReels,
    int numLines,
    double payMultiplier,
    const std::string& filter) const
{
    std::regex idRegex(filter);

    auto* root = doc.FirstChildElement("GameMath");
    auto* comboSetList = root->FirstChildElement("ComboSetList");

    for (auto* paylineComboSet = comboSetList->FirstChildElement("PaylineComboSet");
        paylineComboSet != nullptr;
        paylineComboSet = paylineComboSet->NextSiblingElement("PaylineComboSet"))
    {
        std::string id = paylineComboSet->FirstChildElement("Identifier")->GetText();
        if (!std::regex_search(id, idRegex)) continue;

        comboMap.emplace(id, GetPaylineComboSet(id, symbolSet, numReels, payMultiplier));
    }
}

AnywaysCombo MathXML::GetAnywaysComboSet(const std::string& identifier,
    const SymbolSet& symbolSet,
    int numReels,
    double payMultiplier) const
{
    AnywaysCombo comboSet(numReels, symbolSet);

    auto* root = doc.FirstChildElement("GameMath");
    auto* comboSetList = root->FirstChildElement("ComboSetList");

    for (auto* anyComboSet = comboSetList->FirstChildElement("AnywaysComboSet");
        anyComboSet != nullptr;
        anyComboSet = anyComboSet->NextSiblingElement("AnywaysComboSet"))
    {
        std::string id = anyComboSet->FirstChildElement("Identifier")->GetText();
        if (id != identifier) continue;

        auto* comboList = anyComboSet->FirstChildElement("AnywaysComboList");
        for (auto* combo = comboList->FirstChildElement("AnywaysCombo");
            combo != nullptr;
            combo = combo->NextSiblingElement("AnywaysCombo"))
        {
            std::vector<int> symbols;
            std::set<int> uniqueSymbols;

            for (auto* symbolElem = combo->FirstChildElement("SymbolList")->FirstChildElement("Symbol");
                symbolElem != nullptr;
                symbolElem = symbolElem->NextSiblingElement("Symbol"))
            {
                std::string symbolStr = symbolElem->GetText();
                int symbolIndex = symbolStr == "ANY" ? -1 : symbolSet.GetSymbolIndex(symbolStr);
                symbols.push_back(symbolIndex);
                if (symbolIndex != -1) uniqueSymbols.insert(symbolIndex);
            }

            // Error check: should only be one unique symbol (excluding ANY)
            if (uniqueSymbols.size() > 1)
            {
                std::cerr << "Warning: Invalid AnywaysCombo with multiple symbols before ANY in set '" << id << "'\n";
                continue;
            }

            // Determine the main symbol and combo length
            int symbol = (uniqueSymbols.empty() ? -1 : *uniqueSymbols.begin());
            int comboLength = 0;
            for (int s : symbols) {
                if (s == -1) break;
                ++comboLength;
            }

            double pay = std::stod(combo->FirstChildElement("Value")->GetText()) * payMultiplier;

            int bonusCode = 0;
            if (auto* bonusElem = combo->FirstChildElement("BonusCode"))
                bonusCode = std::stoi(bonusElem->GetText());

            int progressive = 0;
            if (auto* progElem = combo->FirstChildElement("Progressive"))
                progressive = std::stoi(progElem->GetText());

            comboSet.SetCombo(symbol, comboLength, pay, bonusCode, progressive);
        }

        break;
    }

    return comboSet;
}

void MathXML::GetAllAnywaysComboSets(std::map<std::string, AnywaysCombo>& comboMap,
    const SymbolSet& symbolSet,
    int numReels,
    double payMultiplier,
    const std::string& filter) const
{
    std::regex idRegex(filter);

    auto* root = doc.FirstChildElement("GameMath");
    auto* comboSetList = root->FirstChildElement("ComboSetList");

    for (auto* anyComboSet = comboSetList->FirstChildElement("AnywaysComboSet");
        anyComboSet != nullptr;
        anyComboSet = anyComboSet->NextSiblingElement("AnywaysComboSet"))
    {
        std::string id = anyComboSet->FirstChildElement("Identifier")->GetText();
        if (!std::regex_search(id, idRegex)) continue;

        comboMap.emplace(id, GetAnywaysComboSet(id, symbolSet, numReels, payMultiplier));
    }
}

ScatterCombo MathXML::GetScatterComboSet(const std::string& identifier,
    const SymbolSet& symbolSet,
    int numReels,
    double payMultiplier) const
{
    ScatterCombo comboSet(numReels, symbolSet);
    std::vector<int> reelSymbols(numReels, -1); // -1 means unset

    auto* root = doc.FirstChildElement("GameMath");
    auto* comboSetList = root->FirstChildElement("ComboSetList");

    for (auto* scatterComboSet = comboSetList->FirstChildElement("ScatterComboSet");
        scatterComboSet != nullptr;
        scatterComboSet = scatterComboSet->NextSiblingElement("ScatterComboSet"))
    {
        std::string id = scatterComboSet->FirstChildElement("Identifier")->GetText();
        if (id != identifier) continue;

        auto* comboList = scatterComboSet->FirstChildElement("ScatterComboList");
        for (auto* combo = comboList->FirstChildElement("ScatterCombo");
            combo != nullptr;
            combo = combo->NextSiblingElement("ScatterCombo"))
        {
            std::vector<bool> reelsHit(numReels, false);
            std::vector<int> symbols(numReels, -1);

            int reelIndex = 0;
            for (auto* symbolElem = combo->FirstChildElement("SymbolList")->FirstChildElement("Symbol");
                symbolElem != nullptr && reelIndex < numReels;
                symbolElem = symbolElem->NextSiblingElement("Symbol"), ++reelIndex)
            {
                std::string symbolStr = symbolElem->GetText();
                if (symbolStr != "ANY")
                {
                    int symbol = symbolSet.GetSymbolIndex(symbolStr);
                    reelsHit[reelIndex] = true;
                    symbols[reelIndex] = symbol;

                    // Check for symbol conflict
                    if (reelSymbols[reelIndex] == -1)
                    {
                        reelSymbols[reelIndex] = symbol;
                    }
                    else if (reelSymbols[reelIndex] != symbol)
                    {
                        std::cerr << "Error: Multiple scatter symbols defined for reel " << reelIndex
                            << " in set '" << id << "'\n";
                    }
                }
            }

            double pay = std::stod(combo->FirstChildElement("Value")->GetText()) * payMultiplier;

            int bonusCode = 0;
            if (auto* bonusElem = combo->FirstChildElement("BonusCode"))
                bonusCode = std::stoi(bonusElem->GetText());

            int progressive = 0;
            if (auto* progElem = combo->FirstChildElement("Progressive"))
                progressive = std::stoi(progElem->GetText());

            comboSet.SetCombo(reelsHit, pay, bonusCode, progressive);
        }

        comboSet.SetScatterSymbols(reelSymbols);
        break;
    }

    return comboSet;
}

void MathXML::GetAllScatterComboSets(std::map<std::string, ScatterCombo>& comboMap,
    const SymbolSet& symbolSet,
    int numReels,
    double payMultiplier,
    const std::string& filter) const
{
    std::regex idRegex(filter);

    auto* root = doc.FirstChildElement("GameMath");
    auto* comboSetList = root->FirstChildElement("ComboSetList");

    for (auto* scatterComboSet = comboSetList->FirstChildElement("ScatterComboSet");
        scatterComboSet != nullptr;
        scatterComboSet = scatterComboSet->NextSiblingElement("ScatterComboSet"))
    {
        std::string id = scatterComboSet->FirstChildElement("Identifier")->GetText();
        if (!std::regex_search(id, idRegex)) continue;

        comboMap.emplace(id, GetScatterComboSet(id, symbolSet, numReels, payMultiplier));
    }
}

CountScatterCombo MathXML::GetCountScatterComboSet(const std::string& identifier,
    const SymbolSet& symbolSet,
    int numReels,
    int numRows,
    double payMultiplier) const
{
    CountScatterCombo comboSet(numReels, numRows, symbolSet);
    std::set<int> seenSymbols;

    auto* root = doc.FirstChildElement("GameMath");
    auto* comboSetList = root->FirstChildElement("ComboSetList");

    for (auto* countComboSet = comboSetList->FirstChildElement("CountScatterComboSet");
        countComboSet != nullptr;
        countComboSet = countComboSet->NextSiblingElement("CountScatterComboSet"))
    {
        std::string id = countComboSet->FirstChildElement("Identifier")->GetText();
        if (id != identifier) continue;

        auto* comboList = countComboSet->FirstChildElement("CountScatterComboList");
        for (auto* combo = comboList->FirstChildElement("CountScatterCombo");
            combo != nullptr;
            combo = combo->NextSiblingElement("CountScatterCombo"))
        {
            std::string symbolStr = combo->FirstChildElement("Symbol")->GetText();
            int symbol = (symbolStr == "ANY") ? -1 : symbolSet.GetSymbolIndex(symbolStr);

            if (symbol != -1) seenSymbols.insert(symbol);

            if (seenSymbols.size() > 1)
            {
                std::cerr << "Error: Multiple different symbols found in CountScatterComboSet '" << id << "'\n";
                continue;
            }

            int minCount = std::stoi(combo->FirstChildElement("Min")->GetText());
            int maxCount = std::stoi(combo->FirstChildElement("Max")->GetText());

            double pay = std::stod(combo->FirstChildElement("Value")->GetText()) * payMultiplier;

            int bonusCode = 0;
            if (auto* bonusElem = combo->FirstChildElement("BonusCode"))
                bonusCode = std::stoi(bonusElem->GetText());

            int progressive = 0;
            if (auto* progElem = combo->FirstChildElement("Progressive"))
                progressive = std::stoi(progElem->GetText());

            for (int count = minCount; count <= maxCount; ++count)
            {
                comboSet.SetCombo(count, pay, bonusCode, progressive);
            }
        }

        // Set the scatter symbol (first one seen)
        if (!seenSymbols.empty())
        {
            comboSet.SetScatterSymbol(*seenSymbols.begin());
        }

        break;
    }

    return comboSet;
}

void MathXML::GetAllCountScatterComboSets(std::map<std::string, CountScatterCombo>& comboMap,
    const SymbolSet& symbolSet,
    int numReels,
    int numRows,
    double payMultiplier,
    const std::string& filter) const
{
    std::regex idRegex(filter);

    auto* root = doc.FirstChildElement("GameMath");
    auto* comboSetList = root->FirstChildElement("ComboSetList");

    for (auto* countComboSet = comboSetList->FirstChildElement("CountScatterComboSet");
        countComboSet != nullptr;
        countComboSet = countComboSet->NextSiblingElement("CountScatterComboSet"))
    {
        std::string id = countComboSet->FirstChildElement("Identifier")->GetText();
        if (!std::regex_search(id, idRegex)) continue;

        comboMap.emplace(id, GetCountScatterComboSet(id, symbolSet, numReels, numRows, payMultiplier));
    }
}
