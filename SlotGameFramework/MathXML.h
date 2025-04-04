#pragma once

#include "General.h"
#include "tinyxml2.h"
#include "WeightTable.h"

class MathXML {
public:
    explicit MathXML(const std::string& filename);
    ~MathXML();

    bool LoadFile(); // Loads and parses the XML file

    WeightTable GetWeightTable(const std::string& identifier);
    void LoadAllWeightTables(std::map<std::string, WeightTable>& tableMap);

    std::vector<double> GetValueTable(const std::string& identifier);
    void LoadAllValueTables(std::map<std::string, std::vector<double>>& tableMap);


private:
    std::string filename;
    tinyxml2::XMLDocument doc;
};
