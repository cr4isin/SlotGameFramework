#pragma once

#include "General.h"
#include "tinyxml2.h"
#include "WeightTable.h"
#include "SymbolSet.h"
#include "SlotReels.h"
#include "Paytable.h"
#include "MysteryReplacement.h"

class MathXML {
public:
    MathXML() = default;
    explicit MathXML(const std::string filename);
    ~MathXML();

    void LoadFile(const std::string filename);

    std::string GetConfigName() const;


    WeightTable GetWeightTable(const std::string& identifier);
    void GetAllWeightTables(std::map<std::string, WeightTable>& tableMap,
        const std::string& filter = "");


    std::vector<double> GetValueTable(const std::string& identifier);
    void GetAllValueTables(std::map<std::string, std::vector<double>>& tableMap,
        const std::string& filter = "");


    SymbolSet GetSymbolSet(const std::string& identifier,
        const std::map<std::string, std::set<std::string>>& wilds = {},
        const std::map<std::string, int>& multipliers = {},
        const std::map<std::string, Colors>& colors = {});

    void GetAllSymbolSets(std::map<std::string, SymbolSet>& symbolSetMap,
        const std::map<std::string, std::set<std::string>>& wilds = {},
        const std::map<std::string, int>& multipliers = {},
        const std::map<std::string, Colors>& colors = {},
        const std::string& filter = "");


    SlotReels GetReelStripSet(const std::string& identifier, const SymbolSet& symbolSet);

    void GetAllReelStripSets(std::map<std::string, SlotReels>& reelSetMap,
        const SymbolSet& symbolSet,
        const std::string& filter = "");

    WeightTable GetReelStripAsWeightTable(const std::string& identifier, const SymbolSet& symbolSet);

    void GetAllReelStripsAsWeightTables(std::map<std::string, WeightTable>& tableMap,
        const SymbolSet& symbolSet,
        const std::string& filter = "");

    PaylineCombo GetPaylineComboSet(const std::string& identifier,
        const SymbolSet& symbolSet,
        int numReels,
        int numLines,
        double payMultiplier = 1.0) const;

    void GetAllPaylineComboSets(std::map<std::string, PaylineCombo>& comboMap,
        const SymbolSet& symbolSet,
        int numReels,
        int numLines,
        double payMultiplier = 1.0,
        const std::string& filter = "") const;

    AnywaysCombo GetAnywaysComboSet(const std::string& identifier,
        const SymbolSet& symbolSet,
        int numReels,
        double payMultiplier = 1.0) const;

    void GetAllAnywaysComboSets(std::map<std::string, AnywaysCombo>& comboMap,
        const SymbolSet& symbolSet,
        int numReels,
        double payMultiplier = 1.0,
        const std::string& filter = "") const;

    ScatterCombo GetScatterComboSet(const std::string& identifier,
        const SymbolSet& symbolSet,
        int numReels,
        double payMultiplier = 1.0) const;

    void GetAllScatterComboSets(std::map<std::string, ScatterCombo>& comboMap,
        const SymbolSet& symbolSet,
        int numReels,
        double payMultiplier = 1.0,
        const std::string& filter = "") const;

    CountScatterCombo GetCountScatterComboSet(const std::string& identifier,
        const SymbolSet& symbolSet,
        int numReels,
        int numRows,
        double payMultiplier = 1.0) const;

    void GetAllCountScatterComboSets(std::map<std::string, CountScatterCombo>& comboMap,
        const SymbolSet& symbolSet,
        int numReels,
        int numRows,
        double payMultiplier = 1.0,
        const std::string& filter = "") const;

    MysteryReplacement GetMysteryReplacement(const std::string& identifier, const SymbolSet& symbolSet) const;

    void GetAllMysteryReplacements(std::map<std::string, MysteryReplacement>& mysteryMap,
        const SymbolSet& symbolSet,
        const std::string& filter = "") const;

private:
    std::string filename = "";
    tinyxml2::XMLDocument doc;
};
