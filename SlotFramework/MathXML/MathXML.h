#pragma once

#include "General.h"
#include "tinyxml2.h"
#include "WeightTable.h"
#include "SymbolSet.h"
#include "SlotReels.h"
#include "Paytable.h"

class MathXML {
public:
    explicit MathXML(const std::string& filename);
    ~MathXML();

    bool LoadFile(); // Loads and parses the XML file

    WeightTable GetWeightTable(const std::string& identifier);
    void LoadAllWeightTables(std::map<std::string, WeightTable>& tableMap,
        const std::string& pattern = "");


    std::vector<double> GetValueTable(const std::string& identifier);
    void LoadAllValueTables(std::map<std::string, std::vector<double>>& tableMap,
        const std::string& pattern = "");


    SymbolSet GetSymbolSet(const std::string& identifier,
        const std::map<std::string, std::set<std::string>>& wilds = {},
        const std::map<std::string, int>& multipliers = {},
        const std::map<std::string, Colors>& colors = {});

    void LoadAllSymbolSets(std::map<std::string, SymbolSet>& symbolSetMap,
        const std::map<std::string, std::set<std::string>>& wilds = {},
        const std::map<std::string, int>& multipliers = {},
        const std::map<std::string, Colors>& colors = {},
        const std::string& pattern = "");


    SlotReels GetReelStripSet(const std::string& identifier, const SymbolSet& symbolSet);

    void LoadAllReelStripSets(std::map<std::string, SlotReels>& reelSetMap,
        const SymbolSet& symbolSet,
        const std::string& pattern = "");

    WeightTable GetReelStripAsWeightTable(const std::string& identifier, const SymbolSet& symbolSet);

    void LoadAllReelStripsAsWeightTables(std::map<std::string, WeightTable>& tableMap,
        const SymbolSet& symbolSet,
        const std::string& pattern = "");

    PaylineCombo GetPaylineComboSet(const std::string& identifier,
        const SymbolSet& symbolSet,
        int numReels,
        double payMultiplier = 1.0) const;

    void LoadAllPaylineComboSets(std::map<std::string, PaylineCombo>& comboMap,
        const SymbolSet& symbolSet,
        int numReels,
        double payMultiplier = 1.0,
        const std::string& regexStr = "") const;

    AnywaysCombo GetAnywaysComboSet(const std::string& identifier,
        const SymbolSet& symbolSet,
        int numReels,
        double payMultiplier = 1.0) const;

    void LoadAllAnywaysComboSets(std::map<std::string, AnywaysCombo>& comboMap,
        const SymbolSet& symbolSet,
        int numReels,
        double payMultiplier = 1.0,
        const std::string& regexStr = "") const;

    ScatterCombo GetScatterComboSet(const std::string& identifier,
        const SymbolSet& symbolSet,
        int numReels,
        double payMultiplier = 1.0) const;

    void LoadAllScatterComboSets(std::map<std::string, ScatterCombo>& comboMap,
        const SymbolSet& symbolSet,
        int numReels,
        double payMultiplier = 1.0,
        const std::string& regexStr = "") const;

    CountScatterCombo GetCountScatterComboSet(const std::string& identifier,
        const SymbolSet& symbolSet,
        int numReels,
        int numRows,
        double payMultiplier = 1.0) const;

    void LoadAllCountScatterComboSets(std::map<std::string, CountScatterCombo>& comboMap,
        const SymbolSet& symbolSet,
        int numReels,
        int numRows,
        double payMultiplier = 1.0,
        const std::string& regexStr = "") const;

private:
    std::string filename;
    tinyxml2::XMLDocument doc;
};
