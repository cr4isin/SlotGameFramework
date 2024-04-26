
import os
import xlsxwriter
import xml.etree.ElementTree as ET
from itertools import chain


def CodeConverter(math_files:list[str]):
    math_xmls = [ET.parse(math_file).getroot() for math_file in math_files]
    workbook = xlsxwriter.Workbook('mathXMLToCode.xlsx')
    title_format = workbook.add_format({'bold':True, 'align':'center'})
    
    # Config Mapping sheet
    xml_names = [math_xml.find('{*}GameId').text.strip() for math_xml in math_xmls]
    config_mapping = [f'{{ "{name}", {index} }},' for index, name in enumerate(xml_names)]
    worksheet = workbook.add_worksheet('Configs')
    worksheet.set_column('A:B', 80)
    worksheet.write_row('A1',['// SetConfig()', '// main.cpp'], title_format)
    worksheet.write_column('A2',config_mapping)
    worksheet.write('B2', f'vector<string> configs = {{ "{ '", "'.join(xml_names)}" }};')

    # Make Reels sheet
    worksheet = workbook.add_worksheet('Reels')
    worksheet.set_column('A:A', 60)
    worksheet.set_column('B:B', 60)
    worksheet.set_column('C:D', 10)
    worksheet.set_column('E:E', 120)
    worksheet.write('A1', '// SetupReels()', title_format)
    worksheet.write('B1', '// SlotGame.h', title_format)
    worksheet.merge_range('C1:E1', '// SlotGameDefs.cpp', title_format)
    row_0 = row_1 = row_2 = 1

    # Reels
    for reel_strip_sets in zip(*(math_xml.findall('{*}ReelStripSetList/{*}ReelStripSet') for math_xml in math_xmls)):
        reels = []
        weights = []
        for reel_strip_set, math_xml in zip(reel_strip_sets, math_xmls):
            temp_reels = []
            temp_weights = []
            name = reel_strip_set.find('{*}Identifier').text.strip()
            reel_strip_names = [rs.text.strip() for rs in reel_strip_set.findall('{*}ReelStripIDList/{*}ReelStripID')]
            for reel_strip_name in reel_strip_names:
                reel_strip = [reel for reel in math_xml.findall('{*}ReelStripList/{*}ReelStrip') if reel.find('{*}Identifier').text.strip() == reel_strip_name][0]
                temp_reels.append([symbol.text.strip() for symbol in reel_strip.findall('{*}WeightedElementList/{*}WeightedElement/{*}StringValue')])
                temp_weights.append([symbol.text.strip() for symbol in reel_strip.findall('{*}WeightedElementList/{*}WeightedElement/{*}Weight')])
            reels.append(temp_reels)
            weights.append(temp_weights)
        equal_reels = all(reels[0] == r for r in reels[1:])
        equal_weights = all(weights[0] == w for w in weights[1:])
        unique_weights = any(weight != '1' for weight in chain(*chain(*weights)))
        
        if equal_reels and equal_weights:
            if unique_weights:
                worksheet.write(row_0, 0, f'{name} = SlotReels({name}Reels, {name}Weights);')
                row_0 += 1
                worksheet.write(row_1, 1, f'static vector<vector<int>> {name}Reels;')
                worksheet.write(row_1 + 1, 1, f'static vector<vector<int>> {name}Weights;')
                worksheet.write(row_1 + 2, 1, f'SlotReels {name};')
                row_1 += 3
                reel_text = ['{ ' + ', '.join(r) + ' },' for r in reels[0]]
                worksheet.write_column(row_2, 2, [f'vector<vector<int>> SlotGame::{name}Reels =', '{'] + ['']*len(reel_text) + ['};'])
                worksheet.write_column(row_2+2, 3, reel_text)
                row_2 += 3 + len(reel_text)
                weight_text = ['{ ' + ', '.join(r) + ' },' for r in weights[0]]
                worksheet.write_column(row_2, 2, [f'vector<vector<int>> SlotGame::{name}Weights =', '{'] + ['']*len(reel_text) + ['};'])
                worksheet.write_column(row_2+2, 3, weight_text)
                row_2 += 3 + len(weight_text)
            else:
                worksheet.write(row_0, 0, f'{name} = SlotReels({name}Reels);')
                row_0 += 1
                worksheet.write(row_1, 1, f'static vector<vector<int>> {name}Reels;')
                worksheet.write(row_1 + 1, 1, f'SlotReels {name};')
                row_1 += 2
                reel_text = ['{ ' + ', '.join(r) + ' },' for r in reels[0]]
                worksheet.write_column(row_2, 2, [f'vector<vector<int>> SlotGame::{name}Reels =', '{'] + ['']*len(reel_text) + ['};'])
                worksheet.write_column(row_2+2, 3, reel_text)
                row_2 += 3 + len(reel_text)
        elif equal_reels:
            worksheet.write(row_0, 0, f'{name} = SlotReels({name}Reels, {name}Weights[configIndex]);')
            row_0 += 1
            worksheet.write(row_1, 1, f'static vector<vector<int>> {name}Reels;')
            worksheet.write(row_1 + 1, 1, f'static vector<vector<vector<int>>> {name}Weights;')
            worksheet.write(row_1 + 2, 1, f'SlotReels {name};')
            row_1 += 3

            reel_text = ['{ ' + ', '.join(r) + ' },' for r in reels[0]]
            worksheet.write_column(row_2, 2, [f'vector<vector<int>> SlotGame::{name}Reels =', '{'] + ['']*len(reel_text) + ['};'])
            worksheet.write_column(row_2+2, 3, reel_text)
            row_2 += 3 + len(reel_text)
            
            worksheet.write_column(row_2, 2, [f'vector<vector<vector<int>>> SlotGame::{name}Weights =', '{'])
            row_2 += 2
            for rs in weights:
                weight_text = ['{ ' + ', '.join(r) + ' },' for r in rs]
                worksheet.write_column(row_2, 3, ['{'] + ['']*len(weight_text) + ['},'])
                worksheet.write_column(row_2+1, 4, weight_text)
                row_2 += 2 + len(weight_text)
            worksheet.write(row_2, 2, '};')
            row_2 += 1
        elif equal_weights:
            if unique_weights:
                worksheet.write(row_0, 0, f'{name} = SlotReels({name}Reels[configIndex], {name}Weights);')
                row_0 += 1
                worksheet.write(row_1, 1, f'static vector<vector<vector<int>>> {name}Reels;')
                worksheet.write(row_1 + 1, 1, f'static vector<vector<int>> {name}Weights;')
                worksheet.write(row_1 + 2, 1, f'SlotReels {name};')
                row_1 += 3

                worksheet.write_column(row_2, 2, [f'vector<vector<vector<int>>> SlotGame::{name}Reels =', '{'])
                row_2 += 2
                for rs in reels:
                    reel_text = ['{ ' + ', '.join(r) + ' },' for r in rs]
                    worksheet.write_column(row_2, 3, ['{'] + ['']*len(reel_text) + ['},'])
                    worksheet.write_column(row_2+1, 4, reel_text)
                    row_2 += 2 + len(reel_text)
                worksheet.write(row_2, 2, '};')
                row_2 += 1
                
                weight_text = ['{ ' + ', '.join(r) + ' },' for r in weights[0]]
                worksheet.write_column(row_2, 2, [f'vector<vector<int>> SlotGame::{name}Weights =', '{'] + ['']*len(reel_text) + ['};'])
                worksheet.write_column(row_2+2, 3, weight_text)
                row_2 += 3 + len(weight_text)
            else:
                worksheet.write(row_0, 0, f'{name} = SlotReels({name}Reels[configIndex]);')
                row_0 += 1
                worksheet.write(row_1, 1, f'static vector<vector<vector<int>>> {name}Reels;')
                worksheet.write(row_1 + 1, 1, f'SlotReels {name};')
                row_1 += 2

                worksheet.write_column(row_2, 2, [f'vector<vector<vector<int>>> SlotGame::{name}Reels =', '{'])
                row_2 += 2
                for rs in reels:
                    reel_text = ['{ ' + ', '.join(r) + ' },' for r in rs]
                    worksheet.write_column(row_2, 3, ['{'] + ['']*len(reel_text) + ['},'])
                    worksheet.write_column(row_2+1, 4, reel_text)
                    row_2 += 2 + len(reel_text)
                worksheet.write(row_2, 2, '};')
                row_2 += 1
        else:
            worksheet.write(row_0, 0, f'{name} = SlotReels({name}Reels[configIndex], {name}Weights[configIndex]);')
            row_0 += 1
            worksheet.write(row_1, 1, f'static vector<vector<vector<int>>> {name}Reels;')
            worksheet.write(row_1 + 1, 1, f'static vector<vector<vector<int>>> {name}Weights;')
            worksheet.write(row_1 + 2, 1, f'SlotReels {name};')
            row_1 += 3

            worksheet.write_column(row_2, 2, [f'vector<vector<vector<int>>> SlotGame::{name}Reels =', '{'])
            row_2 += 2
            for rs in reels:
                reel_text = ['{ ' + ', '.join(r) + ' },' for r in rs]
                worksheet.write_column(row_2, 3, ['{'] + ['']*len(reel_text) + ['},'])
                worksheet.write_column(row_2+1, 4, reel_text)
                row_2 += 2 + len(reel_text)
            worksheet.write(row_2, 2, '};')
            row_2 += 1
            
            worksheet.write_column(row_2, 2, [f'vector<vector<vector<int>>> SlotGame::{name}Weights =', '{'])
            row_2 += 2
            for rs in weights:
                weight_text = ['{ ' + ', '.join(r) + ' },' for r in rs]
                worksheet.write_column(row_2, 3, ['{'] + ['']*len(weight_text) + ['},'])
                worksheet.write_column(row_2+1, 4, weight_text)
                row_2 += 2 + len(weight_text)
            worksheet.write(row_2, 2, '};')
            row_2 += 1

    # Make Weight and Value Tables sheet
    worksheet = workbook.add_worksheet('Weight and Value Tables')
    worksheet.set_column('A:A', 60)
    worksheet.set_column('B:B', 60)
    worksheet.set_column('C:C', 10)
    worksheet.set_column('D:D', 120)
    worksheet.write('A1', '// SetupWeightTables()', title_format)
    worksheet.write('B1', '// SlotGame.h', title_format)
    worksheet.merge_range('C1:D1', '// SlotGameDefs.cpp', title_format)
    row_0 = row_1 = row_2 = 1
    # Weight Tables
    for weight_tables in zip(*(math_xml.findall('{*}BonusInfo/{*}WeightedTableList/{*}WeightedTable') for math_xml in math_xmls)):
        name = weight_tables[0].find('{*}Identifier').text.strip()
        weights = [[element.text.strip() for element in weight_table.findall('{*}WeightedElementList/{*}WeightedElement/{*}Weight')] for weight_table in weight_tables]
        values = [[element.text.strip() for element in weight_table.findall('{*}WeightedElementList/{*}WeightedElement/{*}Value')] for weight_table in weight_tables]
        equal_weights = all(weights[0] == w for w in weights[1:])
        equal_values = all(values[0] == v for v in values[1:])
        unique_values = values[0] != list(map(str,range(len(values[0]))))
        if equal_weights and equal_values:
            if unique_values:
                worksheet.write(row_0, 0,f'weightTable.emplace("{name}", WeightTable({name}Weights, {name}Values));')
                row_0 += 1
                worksheet.write(row_1, 1,f'static vector<long long> {name}Weights;')
                worksheet.write(row_1 + 1, 1,f'static vector<double> {name}Values;')
                row_1 += 2
                worksheet.write(row_2, 2,f'vector<long long> SlotGame::{name}Weights = {{ {', '.join(weights[0])} }};')
                row_2 += 1
                worksheet.write(row_2, 2,f'vector<double> SlotGame::{name}Values = {{ {', '.join(values[0])} }};')
                row_2 += 1
            else:
                worksheet.write(row_0, 0,f'weightTable.emplace("{name}", WeightTable({name}Weights));')
                row_0 += 1
                worksheet.write(row_1, 1,f'static vector<long long> {name}Weights;')
                row_1 += 1
                worksheet.write(row_2, 2,f'vector<long long> SlotGame::{name}Weights = {{ {', '.join(weights[0])} }};')
                row_2 += 1
        elif equal_weights:
            worksheet.write(row_0, 0,f'weightTable.emplace("{name}", WeightTable({name}Weights, {name}Values[configIndex]));')
            row_0 += 1
            worksheet.write(row_1, 1,f'static vector<long long> {name}Weights;')
            worksheet.write(row_1 + 1, 1,f'static vector<vector<double>> {name}Values;')
            row_1 += 2
            worksheet.write(row_2, 2,f'vector<long long> SlotGame::{name}Weights = {{ {', '.join(weights[0])} }};')
            row_2 += 1
            value_text = ['{ ' + ', '.join(v) + ' },' for v in values]
            worksheet.write_column(row_2, 2, [f'vector<vector<double>> SlotGame::{name}Values =', '{'] + ['']*len(value_text) + ['};'])
            worksheet.write_column(row_2+2, 3, value_text)
            row_2 += 3 + len(value_text)
        elif equal_values:
            if unique_values:
                worksheet.write(row_0, 0,f'weightTable.emplace("{name}", WeightTable({name}Weights[configIndex], {name}Values));')
                row_0 += 1
                worksheet.write(row_1, 1,f'static vector<vector<long long>> {name}Weights;')
                worksheet.write(row_1 + 1, 1,f'static vector<double> {name}Values;')
                row_1 += 2
                weight_text = ['{ ' + ', '.join(w) + ' },' for w in weights]
                worksheet.write_column(row_2, 2, [f'vector<vector<long long>> SlotGame::{name}Weights =', '{'] + ['']*len(weight_text) + ['};'])
                worksheet.write_column(row_2+2, 3, weight_text)
                row_2 += 3 + len(weight_text)
                worksheet.write(row_2, 2,f'vector<double> SlotGame::{name}Values = {{ {', '.join(values[0])} }};')
                row_2 += 1
            else:
                worksheet.write(row_0, 0,f'weightTable.emplace("{name}", WeightTable({name}Weights[configIndex]));')
                row_0 += 1
                worksheet.write(row_1, 1,f'static vector<vector<long long>> {name}Weights;')
                row_1 += 1
                weight_text = ['{ ' + ', '.join(w) + ' },' for w in weights]
                worksheet.write_column(row_2, 2, [f'vector<vector<long long>> SlotGame::{name}Weights =', '{'] + ['']*len(weight_text) + ['};'])
                worksheet.write_column(row_2+2, 3, weight_text)
                row_2 += 3 + len(weight_text)
        else:
            worksheet.write(row_0, 0,f'weightTable.emplace("{name}", WeightTable({name}Weights[configIndex], {name}Values[configIndex]));')
            row_0 += 1
            worksheet.write(row_1, 1,f'static vector<vector<long long>> {name}Weights;')
            worksheet.write(row_1 + 1, 1,f'static vector<vector<double>> {name}Values;')
            row_1 += 2
            weight_text = ['{ ' + ', '.join(w) + ' },' for w in weights]
            worksheet.write_column(row_2, 2, [f'vector<vector<long long>> SlotGame::{name}Weights =', '{'] + ['']*len(weight_text) + ['};'])
            worksheet.write_column(row_2+2, 3, weight_text)
            row_2 += 3 + len(weight_text)
            value_text = ['{ ' + ', '.join(v) + ' },' for v in values]
            worksheet.write_column(row_2, 2, [f'vector<vector<double>> SlotGame::{name}Values =', '{'] + ['']*len(value_text) + ['};'])
            worksheet.write_column(row_2+2, 3, value_text)
            row_2 += 3 + len(value_text)


    # Value Tables
    for value_tables in zip(*(math_xml.findall('{*}BonusInfo/{*}ValueTableList/{*}ValueTable') for math_xml in math_xmls)):
        name = value_tables[0].find('{*}Identifier').text.strip()
        values = [[element.text.strip() for element in value_table.findall('{*}ValueList/{*}Value')] for value_table in value_tables]
        if all(values[0] == v for v in values[1:]):
            worksheet.write(row_1, 1,f'static vector<double> {name};')
            row_1 += 1
            worksheet.write(row_2, 2,f'vector<double> SlotGame::{name} = {{ {', '.join(values[0])} }};')
            row_2 += 1
        else:
            worksheet.write(row_1, 1,f'static vector<vector<double>> {name};')
            row_1 += 1
            value_text = ['{ ' + ', '.join(v) + ' },' for v in values]
            worksheet.write_column(row_2, 2, [f'vector<vector<double>> SlotGame::{name} =', '{'] + ['']*len(value_text) + ['};'])
            worksheet.write_column(row_2+2, 3, value_text)
            row_2 += 3 + len(value_text)
    workbook.close()


def main():
    math_files = [f for f in os.listdir() if f.startswith('math') and f.endswith('.xml')]
    print('Reading the following files:\n' + '\n'.join(math_files))
    CodeConverter(math_files)
    print('Done!')


if __name__ == '__main__':
    main()