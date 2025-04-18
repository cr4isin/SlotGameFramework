

import os
import xlsxwriter
from re import findall
from natsort import os_sorted
from xlsxwriter.utility import xl_range

def remove_common_prefix(strings):
    if len(strings) <= 1:
         return strings
    
    prefix = strings[0]
    for s in strings[1:]:
        while prefix and not s.startswith(prefix):
            prefix = prefix[:-1]
    
    return [s[len(prefix):] for s in strings]


def main():
    hist_output_files = os_sorted([f for f in os.listdir() if f.startswith('Histogram_') and f.endswith('.txt')])
    sheetnames = [name[:31] for name in remove_common_prefix([file[10:-4] for file in hist_output_files])]

    # Creating workbook
    workbook = xlsxwriter.Workbook('HistResults.xlsx', {
        'default_format_properties': {
            'font_name': 'Aptos Narrow',
            'font_size': 11
        }
    })
    
    for filename, sheetname in zip(hist_output_files, sheetnames):
        # Reading histogram data
        pays = []
        counts = []
        with open(filename) as file:
            for line in file.read().splitlines():
                v1, v2 = line.split('\t')
                pays.append(float(v1))
                counts.append(int(v2))

        # Attempt to get the bet amount, default to 1
        base_bet = 1
        if matches:= findall(r'_(\d+)cr_', filename):
            base_bet = int(matches[0])
        bet_mult = 1
        if matches:= findall(r'_(\d+)x_', filename):
            bet_mult = int(matches[0])
        bet = base_bet * bet_mult

        # Formats
        formats = dict()
        formats['title'] = workbook.add_format({'bold': True, 'align': 'center'})
        formats['center'] = workbook.add_format({'align': 'center'})
        formats['gray_center'] = workbook.add_format({'bg_color': '#D3D3D3', 'align': 'center'})
        formats['comma_center'] = workbook.add_format({'num_format': '#,##0', 'align': 'center'})

        # Creating worksheet and setting column widths
        worksheet = workbook.add_worksheet(sheetname)
        worksheet.set_column(0, 32, 12, formats['center'])
        worksheet.set_column(0, 0, 6, formats['center'])
        worksheet.set_column(6, 6, 6, formats['center'])
        worksheet.set_column(12, 12, 6, formats['center'])

        # Writing histogram data
        worksheet.write_column('B6', pays)
        pays_range = xl_range(5, 1, 5+len(pays)-1, 1)
        worksheet.write_column('C6', counts)
        counts_range = xl_range(5, 2, 5+len(counts)-1, 2)

        # Writing titles and formulas for the left half
        worksheet.write_column('B2', ['Bet', 'Total Count'], formats['title'])
        worksheet.write_column('C2', [bet, f'=SUM({counts_range})'], formats['comma_center'])
        worksheet.write_row('B5', ['Pay', 'Count', 'Pay/Bet', 'Prob', 'RTP'], formats['title'])
        worksheet.write_row('H5', ['Lower', 'Upper', 'Range', 'Prob', 'RTP'], formats['title'])

        worksheet.write_dynamic_array_formula('D6', f'={pays_range}/C2')
        worksheet.write_dynamic_array_formula('E6', f'={counts_range}/C3')
        worksheet.write_dynamic_array_formula('F6', f'=ANCHORARRAY(D6)*ANCHORARRAY(E6)')

        worksheet.write_column('E2', ['Avg Pay', 'Std Dev'], formats['title'])
        worksheet.write_column('F2', ['=SUM(ANCHORARRAY(F6))', '=SQRT(SUMPRODUCT(ANCHORARRAY(E6),(ANCHORARRAY(D6)-F2)^2))'])

        # Writing titles and formulas for the right half
        categories = [0, 0.5, 1, 2, 4, 7.5, 15, 25, 37.5, 50, 75, 100, 150, 500]

        worksheet.write_column('H7', categories)
        worksheet.write('H6', '=H7', formats['gray_center'])
        worksheet.write_column('I6', [f'=H{i}' for i in range(7, 7+len(categories))], formats['gray_center'])
        worksheet.write(f'I{6+len(categories)}', 10**300, formats['gray_center'])

        colors = [
            '#FCFCFF',
            '#DFC3FB',
            '#C189F7',
            '#8E89DF',
            '#5A8AC6',
            '#6CC5EC',
            '#70D7D4',
            '#63BE7B',
            '#97CD7E',
            '#CBDC81',
            '#FFEB84',
            '#FDC07C',
            '#FB9574',
            '#F8696B',
            '#DC3939',
        ]

        category_ranges = [
            '="["&H6&"]"',
            '="("&H7&", "&I7&")"',
            '="["&H8&", "&I8&")"',
            '="["&H9&", "&I9&")"',
            '="["&H10&", "&I10&")"',
            '="["&H11&", "&I11&")"',
            '="["&H12&", "&I12&")"',
            '="["&H13&", "&I13&")"',
            '="["&H14&", "&I14&")"',
            '="["&H15&", "&I15&")"',
            '="["&H16&", "&I16&")"',
            '="["&H17&", "&I17&")"',
            '="["&H18&", "&I18&")"',
            '="["&H19&", "&I19&")"',
            '="["&H20&"+]"',
        ]

        for row, color, formula in zip(range(6, 6+len(colors)), colors, category_ranges):
            color_format = workbook.add_format({'bg_color': color, 'align': 'center', 'border': 1})
            worksheet.write(f'J{row}', formula, color_format)

        lower_limit_range = xl_range(5, 7, 5+len(categories), 7)
        upper_limit_range = xl_range(5, 8, 5+len(categories), 8)
        total_range = xl_range(5, 9, 5+len(categories), 9)

        worksheet.write_dynamic_array_formula('K6', f'=SUMIFS(ANCHORARRAY(E6),ANCHORARRAY(D6),">"&IF(LEFT({total_range})="[","=","")&{lower_limit_range},ANCHORARRAY(D6),"<"&IF(RIGHT({total_range})="]","=","")&{upper_limit_range})')
        worksheet.write_dynamic_array_formula('L6', f'=SUMIFS(ANCHORARRAY(F6),ANCHORARRAY(D6),">"&IF(LEFT({total_range})="[","=","")&{lower_limit_range},ANCHORARRAY(D6),"<"&IF(RIGHT({total_range})="]","=","")&{upper_limit_range})/F2')

        # Getting data ranges for the charts
        hits_range = xl_range(5, 10, 5+len(categories), 10)
        rtp_range = xl_range(5, 11, 5+len(categories), 11)

        # Hits Bar Chart
        hits_bar_chart = workbook.add_chart({'type': 'column'})
        hits_bar_chart.add_series(
            {
                'categories': f'={sheetname}!{total_range}',
                'values': f'={sheetname}!{hits_range}',
                'points': [{'fill': {'color': color}, 'border': {'color': '#000000', 'width': 0.75}} for color in colors],
                'gap': 20,
            }
        )
        hits_bar_chart.set_legend({'position': 'none'})
        hits_bar_chart.set_size({'width': 969, 'height': 420})
        hits_bar_chart.set_title({'name': 'Hits', 'overlay': True, 'layout': {'x': .05, 'y': .05}})
        worksheet.insert_chart('N1', hits_bar_chart, {'x_offset': 5, 'y_offset': 5})

        # Hits Pie Chart
        hits_pie_chart = workbook.add_chart({'type': 'pie'})
        hits_pie_chart.add_series(
            {
                'categories': f'={sheetname}!{total_range}',
                'values': f'={sheetname}!{hits_range}',
                'points': [{'fill': {'color': color}, 'border': {'color': '#000000', 'width': 0.75}} for color in colors],
                'data_labels': {
                    'value': True,
                    'leader_lines': True,
                    'legend_key': True,
                    'num_format': '0.0000%'
                }
            }
        )

        hits_pie_chart.set_rotation(90)
        hits_pie_chart.set_plotarea({'layout': {'x': 0.00, 'y': 0.05, 'width': 0.9, 'height': 0.9}})
        hits_pie_chart.set_title({'name': 'Hits', 'overlay': True, 'layout': {'x': .05, 'y': .05}})
        hits_pie_chart.set_size({'width': 791, 'height': 420})
        worksheet.insert_chart('Y1', hits_pie_chart, {'x_offset': 5, 'y_offset': 5})

        # RTP Bar Chart
        rtp_bar_chart = workbook.add_chart({'type': 'column'})
        rtp_bar_chart.add_series(
            {
                'categories': f'={sheetname}!{total_range}',
                'values': f'={sheetname}!{rtp_range}',
                'points': [{'fill': {'color': color}, 'border': {'color': '#000000', 'width': 0.75}} for color in colors],
                'gap': 20,
            }
        )
        rtp_bar_chart.set_legend({'position': 'none'})
        rtp_bar_chart.set_size({'width': 969, 'height': 420})
        rtp_bar_chart.set_title({'name': 'RTP', 'overlay': True, 'layout': {'x': .05, 'y': .05}})
        worksheet.insert_chart('N22', rtp_bar_chart, {'x_offset': 5, 'y_offset': 10})

        # RTP Pie Chart
        rtp_pie_chart = workbook.add_chart({'type': 'pie'})
        rtp_pie_chart.add_series(
            {
                'categories': f'={sheetname}!{total_range}',
                'values': f'={sheetname}!{rtp_range}',
                'points': [{'fill': {'color': color}, 'border': {'color': '#000000', 'width': 0.75}} for color in colors],
                'data_labels': {
                    'value': True,
                    'leader_lines': True,
                    'legend_key': True,
                    'num_format': '0.0000%'
                }
            }
        )

        rtp_pie_chart.set_rotation(90)
        rtp_pie_chart.set_plotarea({'layout': {'x': 0.00, 'y': 0.05, 'width': 0.9, 'height': 0.9}})
        rtp_pie_chart.set_title({'name': 'RTP', 'overlay': True, 'layout': {'x': .05, 'y': .05}})
        rtp_pie_chart.set_size({'width': 791, 'height': 420})
        worksheet.insert_chart('Y22', rtp_pie_chart, {'x_offset': 5, 'y_offset': 10})

        worksheet.set_top_left_cell(0,12)
    # Close and save workbook
    workbook.close()


if __name__ == '__main__':
    main()
