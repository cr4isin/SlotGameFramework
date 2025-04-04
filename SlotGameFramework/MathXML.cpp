
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

void MathXML::LoadAllWeightTables(std::map<std::string, WeightTable>& tableMap) {
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

    // Iterate through all WeightedTable elements
    for (tinyxml2::XMLElement* table = weightedTableList->FirstChildElement("WeightedTable");
        table != nullptr; table = table->NextSiblingElement("WeightedTable")) {

        const char* id = table->FirstChildElement("Identifier")->GetText();
        if (!id) {
            std::cerr << "Error: WeightedTable with missing identifier found." << std::endl;
            continue;
        }

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

void MathXML::LoadAllValueTables(std::map<std::string, std::vector<double>>& tableMap) {
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

    // Iterate through all ValueTable elements
    for (tinyxml2::XMLElement* table = valueTableList->FirstChildElement("ValueTable");
        table != nullptr; table = table->NextSiblingElement("ValueTable")) {

        const char* id = table->FirstChildElement("Identifier")->GetText();
        if (!id) {
            std::cerr << "Error: ValueTable with missing identifier found." << std::endl;
            continue;
        }

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
