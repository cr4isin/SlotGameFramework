
#include "MathXML.h"

MathXML::MathXML(const std::string& filename) : filename(filename) {
    if (!LoadFile()) {
        std::cerr << "Error: Failed to load XML file: " << filename << std::endl;
    }
}

MathXML::~MathXML() {}

bool MathXML::LoadFile() {
    tinyxml2::XMLError eResult = doc.LoadFile(filename.c_str());
    if (eResult != tinyxml2::XML_SUCCESS) {
        std::cerr << "Error loading XML file: " << filename << " Error Code: " << eResult << std::endl;
        return false;
    }
    return true;
}

WeightTable MathXML::GetWeightTable(const std::string& identifier) {
    tinyxml2::XMLElement* root = doc.FirstChildElement("GameMath");
    if (!root) {
        std::cerr << "Error: Missing <GameMath> root element in XML." << std::endl;
        return WeightTable({}, {});
    }

    tinyxml2::XMLElement* bonusInfo = root->FirstChildElement("BonusInfo");
    if (!bonusInfo) {
        std::cerr << "Error: Missing <BonusInfo> element in XML." << std::endl;
        return WeightTable({}, {});
    }

    tinyxml2::XMLElement* weightedTableList = bonusInfo->FirstChildElement("WeightedTableList");
    if (!weightedTableList) {
        std::cerr << "Error: Missing <WeightedTableList> in XML." << std::endl;
        return WeightTable({}, {});
    }

    // Iterate through all WeightedTable elements
    for (tinyxml2::XMLElement* table = weightedTableList->FirstChildElement("WeightedTable");
        table != nullptr; table = table->NextSiblingElement("WeightedTable")) {

        const char* id = table->FirstChildElement("Identifier")->GetText();
        if (id && identifier == id) {
            std::vector<long long> weights;
            std::vector<double> values;

            tinyxml2::XMLElement* elementList = table->FirstChildElement("WeightedElementList");
            if (!elementList) {
                std::cerr << "Error: Missing <WeightedElementList> for " << identifier << std::endl;
                return WeightTable({}, {});
            }

            for (tinyxml2::XMLElement* element = elementList->FirstChildElement("WeightedElement");
                element != nullptr; element = element->NextSiblingElement("WeightedElement")) {

                long long weight = element->FirstChildElement("Weight")->Int64Text(0);
                double value = element->FirstChildElement("Value")->DoubleText(0.0);

                weights.push_back(weight);
                values.push_back(value);
            }

            return WeightTable(weights, values);
        }
    }

    std::cerr << "Error: WeightTable " << identifier << " not found in XML." << std::endl;
    return WeightTable({}, {});
}

void MathXML::LoadAllWeightTables(std::map<std::string, 
    WeightTable>& tableMap,
    const std::string& pattern) {
    tinyxml2::XMLElement* root = doc.FirstChildElement("GameMath");
    if (!root) {
        std::cerr << "Error: Missing <GameMath> root element in XML." << std::endl;
        return;
    }

    tinyxml2::XMLElement* bonusInfo = root->FirstChildElement("BonusInfo");
    if (!bonusInfo) {
        std::cerr << "Error: Missing <BonusInfo> element in XML." << std::endl;
        return;
    }

    tinyxml2::XMLElement* weightedTableList = bonusInfo->FirstChildElement("WeightedTableList");
    if (!weightedTableList) {
        std::cerr << "Error: Missing <WeightedTableList> in XML." << std::endl;
        return;
    }

    std::regex reFilter;
    bool useRegex = !pattern.empty();
    if (useRegex) reFilter = std::regex(pattern);

    // Iterate through all WeightedTable elements
    for (tinyxml2::XMLElement* table = weightedTableList->FirstChildElement("WeightedTable");
        table != nullptr; table = table->NextSiblingElement("WeightedTable")) {

        std::string id = table->FirstChildElement("Identifier")->GetText();
        if (useRegex && !std::regex_search(id, reFilter)) continue;


        std::vector<long long> weights;
        std::vector<double> values;

        tinyxml2::XMLElement* elementList = table->FirstChildElement("WeightedElementList");
        if (!elementList) {
            std::cerr << "Error: Missing <WeightedElementList> for " << id << std::endl;
            continue;
        }

        for (tinyxml2::XMLElement* element = elementList->FirstChildElement("WeightedElement");
            element != nullptr; element = element->NextSiblingElement("WeightedElement")) {

            long long weight = element->FirstChildElement("Weight")->Int64Text(0);
            double value = element->FirstChildElement("Value")->DoubleText(0.0);

            weights.push_back(weight);
            values.push_back(value);
        }

        tableMap.emplace(id, WeightTable(weights, values));
    }
}

std::vector<double> MathXML::GetValueTable(const std::string& identifier) {
    tinyxml2::XMLElement* root = doc.FirstChildElement("GameMath");
    if (!root) {
        std::cerr << "Error: Missing <GameMath> root element in XML." << std::endl;
        return {};
    }

    tinyxml2::XMLElement* bonusInfo = root->FirstChildElement("BonusInfo");
    if (!bonusInfo) {
        std::cerr << "Error: Missing <BonusInfo> element in XML." << std::endl;
        return {};
    }

    tinyxml2::XMLElement* valueTableList = bonusInfo->FirstChildElement("ValueTableList");
    if (!valueTableList) {
        std::cerr << "Error: Missing <ValueTableList> in XML." << std::endl;
        return {};
    }

    // Iterate through all ValueTable elements
    for (tinyxml2::XMLElement* table = valueTableList->FirstChildElement("ValueTable");
        table != nullptr; table = table->NextSiblingElement("ValueTable")) {

        const char* id = table->FirstChildElement("Identifier")->GetText();
        if (id && identifier == id) {
            std::vector<double> values;

            tinyxml2::XMLElement* valueList = table->FirstChildElement("ValueList");
            if (!valueList) {
                std::cerr << "Error: Missing <ValueList> for " << identifier << std::endl;
                return {};
            }

            for (tinyxml2::XMLElement* valueElement = valueList->FirstChildElement("Value");
                valueElement != nullptr; valueElement = valueElement->NextSiblingElement("Value")) {

                double value = valueElement->DoubleText(0.0);
                values.push_back(value);
            }

            return values;
        }
    }

    std::cerr << "Error: ValueTable " << identifier << " not found in XML." << std::endl;
    return {};
}

void MathXML::LoadAllValueTables(std::map<std::string, 
    std::vector<double>>& tableMap,
    const std::string& pattern) {
    tinyxml2::XMLElement* root = doc.FirstChildElement("GameMath");
    if (!root) {
        std::cerr << "Error: Missing <GameMath> root element in XML." << std::endl;
        return;
    }

    tinyxml2::XMLElement* bonusInfo = root->FirstChildElement("BonusInfo");
    if (!bonusInfo) {
        std::cerr << "Error: Missing <BonusInfo> element in XML." << std::endl;
        return;
    }

    tinyxml2::XMLElement* valueTableList = bonusInfo->FirstChildElement("ValueTableList");
    if (!valueTableList) {
        std::cerr << "Error: Missing <ValueTableList> in XML." << std::endl;
        return;
    }

    std::regex reFilter;
    bool useRegex = !pattern.empty();
    if (useRegex) reFilter = std::regex(pattern);

    // Iterate through all ValueTable elements
    for (tinyxml2::XMLElement* table = valueTableList->FirstChildElement("ValueTable");
        table != nullptr; table = table->NextSiblingElement("ValueTable")) {

        std::string id = table->FirstChildElement("Identifier")->GetText();
        if (useRegex && !std::regex_search(id, reFilter)) continue;


        std::vector<double> values;

        tinyxml2::XMLElement* valueList = table->FirstChildElement("ValueList");
        if (!valueList) {
            std::cerr << "Error: Missing <ValueList> for " << id << std::endl;
            continue;
        }

        for (tinyxml2::XMLElement* valueElement = valueList->FirstChildElement("Value");
            valueElement != nullptr; valueElement = valueElement->NextSiblingElement("Value")) {

            double value = valueElement->DoubleText(0.0);
            values.push_back(value);
        }

        tableMap[id] = values;
    }
}

SymbolSet MathXML::GetSymbolSet(const std::string& identifier,
    const std::map<std::string, std::set<std::string>>& wilds,
    const std::map<std::string, int>& multipliers,
    const std::map<std::string, Colors>& colors) {
    tinyxml2::XMLElement* root = doc.FirstChildElement("GameMath");
    if (!root) throw std::runtime_error("Missing <GameMath> in XML");

    tinyxml2::XMLElement* symbolSetList = root->FirstChildElement("SymbolSetList");
    if (!symbolSetList) throw std::runtime_error("Missing <SymbolSetList> in XML");

    for (tinyxml2::XMLElement* set = symbolSetList->FirstChildElement("SymbolSet");
        set != nullptr; set = set->NextSiblingElement("SymbolSet")) {
        const char* id = set->FirstChildElement("Identifier")->GetText();
        if (id && identifier == id) {
            std::vector<std::string> symbols;
            auto symbolList = set->FirstChildElement("SymbolList");
            for (auto symbolElem = symbolList->FirstChildElement("Symbol");
                symbolElem != nullptr; symbolElem = symbolElem->NextSiblingElement("Symbol")) {
                symbols.emplace_back(symbolElem->GetText());
            }

            // Determine which wild map to use
            std::map<std::string, std::set<std::string>> usedWilds = wilds;
            if (wilds.empty()) {
                tinyxml2::XMLElement* wildList = set->FirstChildElement("WildSymbolList");
                if (wildList) {
                    for (auto wildElem = wildList->FirstChildElement("WildSymbol");
                        wildElem != nullptr; wildElem = wildElem->NextSiblingElement("WildSymbol")) {
                        std::string wildName = wildElem->FirstChildElement("StringValue")->GetText();
                        std::set<std::string> subs;
                        auto subsList = wildElem->FirstChildElement("SubstituteList");
                        for (auto sub = subsList->FirstChildElement("StringValue");
                            sub != nullptr; sub = sub->NextSiblingElement("StringValue")) {
                            subs.insert(sub->GetText());
                        }
                        usedWilds[wildName] = subs;
                    }
                }
            }

            return SymbolSet(symbols, usedWilds, multipliers, colors);
        }
    }

    throw std::runtime_error("SymbolSet not found: " + identifier);
}

void MathXML::LoadAllSymbolSets(std::map<std::string, SymbolSet>& symbolSetMap,
    const std::map<std::string, std::set<std::string>>& wilds,
    const std::map<std::string, int>& multipliers,
    const std::map<std::string, Colors>& colors,
    const std::string& pattern) {
    tinyxml2::XMLElement* root = doc.FirstChildElement("GameMath");
    if (!root) throw std::runtime_error("Missing <GameMath> in XML");

    tinyxml2::XMLElement* symbolSetList = root->FirstChildElement("SymbolSetList");
    if (!symbolSetList) throw std::runtime_error("Missing <SymbolSetList> in XML");

    std::regex reFilter;
    bool useRegex = !pattern.empty();
    if (useRegex) reFilter = std::regex(pattern);

    for (tinyxml2::XMLElement* set = symbolSetList->FirstChildElement("SymbolSet");
        set != nullptr; set = set->NextSiblingElement("SymbolSet")) {

        std::string id = set->FirstChildElement("Identifier")->GetText();
        if (useRegex && !std::regex_search(id, reFilter)) continue;


        // ------------------------------
        // Read symbol list
        std::vector<std::string> symbols;
        auto symbolList = set->FirstChildElement("SymbolList");
        for (auto symbolElem = symbolList->FirstChildElement("Symbol");
            symbolElem != nullptr; symbolElem = symbolElem->NextSiblingElement("Symbol")) {
            symbols.emplace_back(symbolElem->GetText());
        }

        // ------------------------------
        // Use passed-in wilds, or load wilds from XML if none were passed
        std::map<std::string, std::set<std::string>> usedWilds = wilds;
        if (wilds.empty()) {
            auto wildList = set->FirstChildElement("WildSymbolList");
            if (wildList) {
                for (auto wildElem = wildList->FirstChildElement("WildSymbol");
                    wildElem != nullptr; wildElem = wildElem->NextSiblingElement("WildSymbol")) {
                    std::string wildName = wildElem->FirstChildElement("StringValue")->GetText();
                    std::set<std::string> subs;
                    auto subsList = wildElem->FirstChildElement("SubstituteList");
                    for (auto sub = subsList->FirstChildElement("StringValue");
                        sub != nullptr; sub = sub->NextSiblingElement("StringValue")) {
                        subs.insert(sub->GetText());
                    }
                    usedWilds[wildName] = subs;
                }
            }
        }

        // ------------------------------
        // Construct and insert the SymbolSet (SymbolSet handles filtering)
        symbolSetMap[id] = SymbolSet(symbols, usedWilds, multipliers, colors);
    }
}

#include <regex> // add near the top

SlotReels MathXML::GetReelStripSet(const std::string& identifier, const SymbolSet& symbolSet) {
    tinyxml2::XMLElement* root = doc.FirstChildElement("GameMath");
    if (!root) throw std::runtime_error("Missing <GameMath> in XML");

    tinyxml2::XMLElement* setList = root->FirstChildElement("ReelStripSetList");
    if (!setList) throw std::runtime_error("Missing <ReelStripSetList> in XML");

    for (auto* stripSet = setList->FirstChildElement("ReelStripSet");
        stripSet != nullptr; stripSet = stripSet->NextSiblingElement("ReelStripSet")) {

        const char* id = stripSet->FirstChildElement("Identifier")->GetText();
        if (id && identifier == id) {
            std::vector<std::vector<int>> reels;
            std::vector<std::vector<int>> weights;

            for (auto* stripID = stripSet->FirstChildElement("ReelStripIDList")->FirstChildElement("ReelStripID");
                stripID != nullptr; stripID = stripID->NextSiblingElement("ReelStripID")) {

                std::string stripName = stripID->GetText();

                // Find matching ReelStrip
                tinyxml2::XMLElement* reelStripList = root->FirstChildElement("ReelStripList");
                for (auto* strip = reelStripList->FirstChildElement("ReelStrip");
                    strip != nullptr; strip = strip->NextSiblingElement("ReelStrip")) {

                    if (strip->FirstChildElement("Identifier")->GetText() == stripName) {
                        std::vector<int> reelSymbols, reelWeights;
                        auto* elements = strip->FirstChildElement("WeightedElementList");
                        for (auto* elem = elements->FirstChildElement("WeightedElement");
                            elem != nullptr; elem = elem->NextSiblingElement("WeightedElement")) {

                            std::string symbol = elem->FirstChildElement("StringValue")->GetText();
                            int weight = elem->FirstChildElement("Weight")->IntText(1); // default weight = 1
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
    }

    throw std::runtime_error("ReelStripSet not found: " + identifier);
}

void MathXML::LoadAllReelStripSets(std::map<std::string, SlotReels>& reelSetMap,
    const SymbolSet& symbolSet,
    const std::string& pattern) {
    tinyxml2::XMLElement* root = doc.FirstChildElement("GameMath");
    if (!root) throw std::runtime_error("Missing <GameMath> in XML");

    tinyxml2::XMLElement* setList = root->FirstChildElement("ReelStripSetList");
    if (!setList) throw std::runtime_error("Missing <ReelStripSetList> in XML");

    std::regex reFilter;
    bool useRegex = !pattern.empty();
    if (useRegex) reFilter = std::regex(pattern);

    for (auto* stripSet = setList->FirstChildElement("ReelStripSet");
        stripSet != nullptr; stripSet = stripSet->NextSiblingElement("ReelStripSet")) {

        std::string id = stripSet->FirstChildElement("Identifier")->GetText();

        if (useRegex && !std::regex_search(id, reFilter)) continue;

        std::vector<std::vector<int>> reels;
        std::vector<std::vector<int>> weights;

        for (auto* stripID = stripSet->FirstChildElement("ReelStripIDList")->FirstChildElement("ReelStripID");
            stripID != nullptr; stripID = stripID->NextSiblingElement("ReelStripID")) {

            std::string stripName = stripID->GetText();

            // Find matching ReelStrip
            tinyxml2::XMLElement* reelStripList = root->FirstChildElement("ReelStripList");
            for (auto* strip = reelStripList->FirstChildElement("ReelStrip");
                strip != nullptr; strip = strip->NextSiblingElement("ReelStrip")) {

                if (strip->FirstChildElement("Identifier")->GetText() == stripName) {
                    std::vector<int> reelSymbols, reelWeights;
                    auto* elements = strip->FirstChildElement("WeightedElementList");
                    for (auto* elem = elements->FirstChildElement("WeightedElement");
                        elem != nullptr; elem = elem->NextSiblingElement("WeightedElement")) {

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

        reelSetMap[id] = SlotReels(reels, weights);
    }
}
WeightTable MathXML::GetReelStripAsWeightTable(const std::string& identifier, const SymbolSet& symbolSet) {
    tinyxml2::XMLElement* root = doc.FirstChildElement("GameMath");
    if (!root) throw std::runtime_error("Missing <GameMath> in XML");

    tinyxml2::XMLElement* reelStripList = root->FirstChildElement("ReelStripList");
    if (!reelStripList) throw std::runtime_error("Missing <ReelStripList> in XML");

    for (auto* strip = reelStripList->FirstChildElement("ReelStrip");
        strip != nullptr; strip = strip->NextSiblingElement("ReelStrip")) {

        std::string id = strip->FirstChildElement("Identifier")->GetText();
        if (id == identifier) {
            std::vector<long long> weights;
            std::vector<double> values;

            auto* elements = strip->FirstChildElement("WeightedElementList");
            for (auto* elem = elements->FirstChildElement("WeightedElement");
                elem != nullptr; elem = elem->NextSiblingElement("WeightedElement")) {

                std::string symbol = elem->FirstChildElement("StringValue")->GetText();
                long long weight = elem->FirstChildElement("Weight")->Int64Text(1); // default weight = 1
                int symbolIndex = symbolSet.GetSymbolIndex(symbol);

                weights.push_back(weight);
                values.push_back(static_cast<double>(symbolIndex));
            }

            return WeightTable(weights, values);
        }
    }

    throw std::runtime_error("ReelStrip not found: " + identifier);
}

void MathXML::LoadAllReelStripsAsWeightTables(std::map<std::string, WeightTable>& tableMap,
    const SymbolSet& symbolSet,
    const std::string& pattern) {
    tinyxml2::XMLElement* root = doc.FirstChildElement("GameMath");
    if (!root) throw std::runtime_error("Missing <GameMath> in XML");

    tinyxml2::XMLElement* reelStripList = root->FirstChildElement("ReelStripList");
    if (!reelStripList) throw std::runtime_error("Missing <ReelStripList> in XML");

    std::regex reFilter;
    bool useRegex = !pattern.empty();
    if (useRegex) reFilter = std::regex(pattern);

    for (auto* strip = reelStripList->FirstChildElement("ReelStrip");
        strip != nullptr; strip = strip->NextSiblingElement("ReelStrip")) {

        std::string id = strip->FirstChildElement("Identifier")->GetText();
        if (useRegex && !std::regex_search(id, reFilter)) continue;

        std::vector<long long> weights;
        std::vector<double> values;

        auto* elements = strip->FirstChildElement("WeightedElementList");
        for (auto* elem = elements->FirstChildElement("WeightedElement");
            elem != nullptr; elem = elem->NextSiblingElement("WeightedElement")) {

            std::string symbol = elem->FirstChildElement("StringValue")->GetText();
            long long weight = elem->FirstChildElement("Weight")->Int64Text(1);
            int symbolIndex = symbolSet.GetSymbolIndex(symbol);

            weights.push_back(weight);
            values.push_back(static_cast<double>(symbolIndex));
        }

        tableMap[id] = WeightTable(weights, values);
    }
}
