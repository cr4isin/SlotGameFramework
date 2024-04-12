
import os
import xlsxwriter
import statistics as stat
from natsort import os_sorted
from itertools import batched
from collections import defaultdict
from xlsxwriter.utility import xl_range

def ConvertType(value):
    try:
        return int(value)
    except ValueError:
        try:
            return float(value)
        except ValueError:
            return value

def SimAnalyzerToText():
    sim_output_files = os_sorted([f for f in os.listdir() if f.startswith('SimsOutput') and f.endswith('.txt')])
    values_per_tracker = 6
    output_file = open('output.txt', 'w')
    for file_index, file_name in enumerate(sim_output_files,1):
        print(f'{file_index}/{len(sim_output_files)} Calculating data for {file_name}')
        num_trials = 0
        ev_list, max_win_list = (list() for _ in range(2))
        hits, wins = (int() for _ in range(2))
        bet, games_played,  = (set() for _ in range(2))
        tracker_ev = defaultdict(list)
        tracker_game_hits, tracker_game_wins, tracker_total_hits, tracker_total_wins = (defaultdict(int) for _ in range(4))
        # Read Data
        file = open(file_name)
        for line in file:
            if line.isspace():
                continue
            num_trials += 1
            trial_ev, trial_bet, trial_games_played, trial_max_win, trial_hits, trial_wins, *trial_trackers = [ConvertType(value) for value in line.split('\t')]
            ev_list.append(trial_ev)
            max_win_list.append(trial_max_win)
            hits += trial_hits
            wins += trial_wins
            bet.add(trial_bet)
            games_played.add(trial_games_played)
            for name, ev, game_hits, game_wins, total_hits, total_wins in batched(trial_trackers, values_per_tracker):
                tracker_ev[name].append(ev)
                tracker_game_hits[name] += game_hits
                tracker_game_wins[name] += game_wins
                tracker_total_hits[name] += total_hits
                tracker_total_wins[name] += total_wins
        for name in tracker_ev:
            tracker_ev[name] += [0.0] * (num_trials - len(tracker_ev[name]))
        file.close()
        
        # Analyze Data
        games_played = max(games_played)
        bet = max(bet)
        total_ev = stat.mean(ev_list)
        total_stdev = stat.stdev(ev_list) if num_trials > 1 else 0
        hit_rate = hits / games_played / num_trials
        win_rate = wins / games_played / num_trials
        average_max_win = stat.mean(max_win_list)
        total_max_win = max(max_win_list)
        tracker_total_ev = dict()
        tracker_total_stdev = dict()
        for name in tracker_ev:
            tracker_total_ev[name] = stat.mean(tracker_ev[name])
            tracker_total_stdev[name] = stat.stdev(tracker_ev[name]) if num_trials > 1 else 0
        
        # Output to file
        output_file.write(f'{file_name[11:-4]}\t{total_ev}\t{total_stdev}\t{num_trials}\t{games_played}\t{bet}\t{hit_rate}\t{win_rate}\t{total_max_win}\t{average_max_win}\n')
        for name in tracker_total_ev:
            output_file.write(f'{name}\t{tracker_total_ev[name]}\t{tracker_total_stdev[name]}\t{tracker_game_hits[name]}\t{tracker_game_wins[name]}\t{tracker_total_hits[name]}\t{tracker_total_wins[name]}\n')
        output_file.write('\n')
        
    output_file.close()


def CreateWorkbook()->xlsxwriter.Workbook:
    workbook = xlsxwriter.Workbook('SimResults.xlsx', {'use_future_functions': True})
    workbook.add_worksheet('Summary')
    workbook.add_worksheet('Data')
    workbook.add_worksheet('Input')
    return workbook

def UpdateWorkbook(workbook:xlsxwriter.Workbook, filenames:list[str], max_trackers:int):
    data_title_row = ['Sim Name', 'Name Index', 'Simmed EV', 'Standard Deviation', 'Rounded EV', 'Rounding Risk', 'Rounding Confidence', 'Volatility', 'Number of Trials', 'Trial Size', 'Number of Games', 'Bet', 'Hit Rate', 'Win Rate', 'Max Win', 'Avg Max Win', 'Median Spins', 'Number of Trackers', 'Lists']
    filenames = [file[11:-4] for file in filenames]
    names_range = 'Data!' + xl_range(1, 0, len(filenames), 0)

    # Formats
    formats = dict()
    formats['title'] = workbook.add_format({'bold':True, 'align':'center'})
    formats['title_tblr'] = workbook.add_format({'bold':True, 'align':'center', 'top':1, 'bottom':1, 'left':1, 'right':1})
    formats['title_tb'] = workbook.add_format({'bold':True, 'align':'center', 'top':1, 'bottom':1})
    formats['title_t'] = workbook.add_format({'bold':True, 'align':'center', 'top':1,})
    formats['title_b'] = workbook.add_format({'bold':True, 'align':'center', 'bottom':1})
    formats['title_tbl'] = workbook.add_format({'bold':True, 'align':'center', 'top':1, 'bottom':1, 'left':1})
    formats['title_tbr'] = workbook.add_format({'bold':True, 'align':'center', 'top':1, 'bottom':1, 'right':1})
    formats['title_bl'] = workbook.add_format({'bold':True, 'align':'center', 'bottom':1, 'left':1})
    formats['title_tl'] = workbook.add_format({'bold':True, 'align':'center', 'top':1, 'left':1})
    formats['title_l'] = workbook.add_format({'bold':True, 'align':'center', 'left':1})
    formats['center'] = workbook.add_format({'align':'center'})
    formats['center_t'] = workbook.add_format({'align':'center', 'top':1})
    formats['center_l'] = workbook.add_format({'align':'center', 'left':1})
    formats['center_r'] = workbook.add_format({'align':'center', 'right':1})
    formats['center_lr'] = workbook.add_format({'align':'center', 'left':1, 'right':1})
    formats['center_br'] = workbook.add_format({'align':'center', 'bottom':1, 'right':1})
    formats['center_tbr'] = workbook.add_format({'align':'center', 'top':1, 'bottom':1, 'right':1})
    formats['center_tr'] = workbook.add_format({'align':'center', 'top':1, 'right':1})
    formats['percent'] = workbook.add_format({'num_format':'0.00%','align':'center'})
    formats['percent_tr'] = workbook.add_format({'num_format':'0.00%','align':'center', 'top':1, 'right':1})
    formats['percent_br'] = workbook.add_format({'num_format':'0.00%','align':'center', 'bottom':1, 'right':1})
    formats['percent_r'] = workbook.add_format({'num_format':'0.00%','align':'center', 'right':1})
    formats['long_percent'] = workbook.add_format({'num_format':'0.0000%','align':'center'})
    formats['long_percent_tr'] = workbook.add_format({'num_format':'0.0000%','align':'center', 'top':1, 'right':1})
    formats['long_percent_br'] = workbook.add_format({'num_format':'0.0000%','align':'center', 'bottom':1, 'right':1})
    formats['long_percent_r'] = workbook.add_format({'num_format':'0.0000%','align':'center', 'right':1})
    formats['large_int'] = workbook.add_format({'num_format':'#,##0','align':'center'})
    formats['large_int_tr'] = workbook.add_format({'num_format':'#,##0','align':'center', 'top':1, 'right':1})
    formats['large_int_br'] = workbook.add_format({'num_format':'#,##0','align':'center', 'bottom':1, 'right':1})
    formats['large_int_r'] = workbook.add_format({'num_format':'#,##0','align':'center', 'right':1})
    formats['decimal'] = workbook.add_format({'num_format':'#,##0.00','align':'center'})
    formats['decimal_tr'] = workbook.add_format({'num_format':'#,##0.00','align':'center', 'top':1, 'right':1})
    formats['decimal_br'] = workbook.add_format({'num_format':'#,##0.00','align':'center', 'bottom':1, 'right':1})
    formats['decimal_r'] = workbook.add_format({'num_format':'#,##0.00','align':'center', 'right':1})
    formats['really_long_percent'] = workbook.add_format({'num_format':'0.00000000%','align':'center'})

    # Getting Worksheets
    summary_ws:xlsxwriter.Workbook.worksheet_class = workbook.get_worksheet_by_name('Summary')
    data_ws:xlsxwriter.Workbook.worksheet_class = workbook.get_worksheet_by_name('Data')
    input_ws:xlsxwriter.Workbook.worksheet_class = workbook.get_worksheet_by_name('Input')

    # Summary Worksheet
    summary_ws.set_column('A:A',30,formats['center_lr'])
    summary_ws.set_column('B:C',20,formats['center_lr'])
    summary_ws.set_column('D:D',5,formats['center'])
    summary_ws.set_column('E:E',20,formats['center_lr'])
    summary_ws.set_column('F:F',20,formats['center_l'])
    summary_ws.set_column('G:G',20,formats['center_r'])
    summary_ws.set_column('H:H',20,formats['center_l'])
    summary_ws.set_column('I:I',20,formats['center_r'])
    summary_ws.set_column('J:J',20,formats['center_l'])
    summary_ws.set_column('K:L',20,formats['center'])
    summary_ws.set_column('M:M',20,formats['center_r'])
    for row in range(6):
        summary_ws.set_row(row,15,formats['center'])

    summary_ws.write('A2','Filter by Substring:',formats['title_t'])
    summary_ws.write('A3', 'Confidence Level:', formats['title'])
    summary_ws.write('A4', 'Show Values As:', formats['title'])
    summary_ws.write('A5', 'Hit Type:', formats['title_b'])
    summary_ws.write('B2', '', formats['center_t'])
    summary_ws.write('C2', '', formats['center_tbr'])
    summary_ws.write('B3',f'=.95',formats['percent_r'])
    summary_ws.write('B4',f'Per Bet', formats['center_r'])
    summary_ws.write('B5',f'Game Hits', formats['center_br'])

    summary_ws.write_row('A7',['Sim Name', 'Simmed EV', 'Number of Games'],formats['title_tblr'])
    summary_ws.write_dynamic_array_formula('A8',f'=FILTER({names_range},ISNUMBER(SEARCH($B$2,{names_range})))')
    summary_ws.write_dynamic_array_formula('B8',f'=INDEX(Data!$B$2:$R${1 + len(filenames)},XMATCH(ANCHORARRAY($A$8),{names_range}),XMATCH($B$7,Data!$B$1:$R$1))')
    summary_ws.write_dynamic_array_formula('C8',f'=INDEX(Data!$B$2:$R${1 + len(filenames)},XMATCH(ANCHORARRAY($A$8),{names_range}),XMATCH($C$7,Data!$B$1:$R$1))')


    summary_ws.write_row('E2',['View Details For:','', 'Simmed EV','', 'Median Spins','', 'Hit Rate'],formats['title_tl'])
    summary_ws.write_row('E3',['Bet','', 'Rounded EV','', 'Number of Trials','', 'Win Rate'],formats['title_l'])
    summary_ws.write_row('E4',['Number of Trackers','', 'Rounding Confidence','', 'Trial Size','', 'Max Win'],formats['title_l'])
    summary_ws.write_row('E5',['Volatility','', 'Rounding Risk','', 'Number of Games','', 'Average Max Win'],formats['title_bl'])

    summary_ws.write('F2',filenames[0],formats['center_tr'])
    summary_ws.write('F3',f'=XLOOKUP($F$2,{names_range},ANCHORARRAY(Data!$L$2))',formats['large_int_r'])
    summary_ws.write('F4',f'=XLOOKUP($F$2,{names_range},ANCHORARRAY(Data!$R$2))',formats['large_int_r'])
    summary_ws.write('F5',f'=XLOOKUP($F$2,{names_range},ANCHORARRAY(Data!$H$2))',formats['decimal_br'])

    summary_ws.write('H2',f'=XLOOKUP($F$2,{names_range},ANCHORARRAY(Data!$C$2))',formats['long_percent_tr'])
    summary_ws.write('H3',f'=XLOOKUP($F$2,{names_range},ANCHORARRAY(Data!$E$2))',formats['percent_r'])
    summary_ws.write('H4',f'=XLOOKUP($F$2,{names_range},ANCHORARRAY(Data!$G$2))',formats['percent_r'])
    summary_ws.write('H5',f'=XLOOKUP($F$2,{names_range},ANCHORARRAY(Data!$F$2))',formats['percent_br'])

    summary_ws.write('J2',f'=XLOOKUP($F$2,{names_range},ANCHORARRAY(Data!$Q$2))',formats['center_tr'])
    summary_ws.write('J3',f'=XLOOKUP($F$2,{names_range},ANCHORARRAY(Data!$I$2))',formats['large_int_r'])
    summary_ws.write('J4',f'=XLOOKUP($F$2,{names_range},ANCHORARRAY(Data!$J$2))',formats['large_int_r'])
    summary_ws.write('J5',f'=XLOOKUP($F$2,{names_range},ANCHORARRAY(Data!$K$2))',formats['large_int_br'])

    summary_ws.write('L2',f'=XLOOKUP($F$2,{names_range},ANCHORARRAY(Data!$M$2))',formats['percent_tr'])
    summary_ws.write('L3',f'=XLOOKUP($F$2,{names_range},ANCHORARRAY(Data!$N$2))',formats['percent_r'])
    summary_ws.write('L4',f'=XLOOKUP($F$2,{names_range},ANCHORARRAY(Data!$O$2))',formats['decimal_r'])
    summary_ws.write('L5',f'=XLOOKUP($F$2,{names_range},ANCHORARRAY(Data!$P$2))',formats['decimal_br'])


    summary_ws.write('E7','Tracker Name', formats['title_tblr'])
    summary_ws.write('F7','EV', formats['title_tbl'])
    summary_ws.write('G7','Std Dev', formats['title_tbr'])
    summary_ws.write('J7','Avg Pay', formats['title_tbl'])
    summary_ws.write('K7','Freq', formats['title_tb'])
    summary_ws.write('L7','Odds', formats['title_tb'])
    summary_ws.write('M7','Hit Count', formats['title_tbr'])
    summary_ws.write_dynamic_array_formula('E8',f'=LET(_xlpm.trackers,INDEX(Input!$A:$A,SEQUENCE($F$4,,1+XLOOKUP($F$2,{names_range},ANCHORARRAY(Data!$B$2)))),FILTER(_xlpm.trackers,ISNUMBER(SEARCH($C$2,_xlpm.trackers))))')
    summary_ws.write_dynamic_array_formula('F8',f'=LET(_xlpm.name_index,XLOOKUP($F$2,{names_range},ANCHORARRAY(Data!$B$2)),INDEX(Input!$B:$B,_xlpm.name_index+XMATCH(ANCHORARRAY($E$8),INDEX(Input!$A:$A,SEQUENCE($F$4,,1+_xlpm.name_index))))*IF($B$4=Data!$S$8,1,1/$F$3))')
    summary_ws.write_dynamic_array_formula('G8',f'=LET(_xlpm.name_index,XLOOKUP($F$2,{names_range},ANCHORARRAY(Data!$B$2)),INDEX(Input!$C:$C,_xlpm.name_index+XMATCH(ANCHORARRAY($E$8),INDEX(Input!$A:$A,SEQUENCE($F$4,,1+_xlpm.name_index))))*IF($B$4=Data!$S$8,1,1/$F$3))')
    summary_ws.write_dynamic_array_formula('H8',f'=(ANCHORARRAY($F$8)-NORM.INV(1-(1-$B$3)/2,0,1)*ANCHORARRAY($G$8)/SQRT($J$3))*IF($H$7=Data!$S$10,1,ANCHORARRAY($L$8))')
    summary_ws.write_dynamic_array_formula('I8',f'=(ANCHORARRAY($F$8)+NORM.INV(1-(1-$B$3)/2,0,1)*ANCHORARRAY($G$8)/SQRT($J$3))*IF($H$7=Data!$S$10,1,ANCHORARRAY($L$8))')
    summary_ws.write_dynamic_array_formula('J8',f'=ANCHORARRAY($F$8)*ANCHORARRAY($L$8)')
    summary_ws.write_dynamic_array_formula('K8',f'=ANCHORARRAY($M$8)/$J$5')
    summary_ws.write_dynamic_array_formula('L8',f'=IF(ANCHORARRAY($K$8)=0,0,1/ANCHORARRAY($K$8))')
    summary_ws.write_dynamic_array_formula('M8',f'=LET(_xlpm.name_index,XLOOKUP($F$2,{names_range},ANCHORARRAY(Data!$B$2)),INDEX(Input!$D:$G,_xlpm.name_index+XMATCH(ANCHORARRAY($E$8),INDEX(Input!$A:$A,SEQUENCE($F$4,,1+_xlpm.name_index))),XMATCH($B$5,Data!$S$2:$S$5)))')

    summary_ws.data_validation('F2',{"validate": "list", "source": f"={names_range}"})
    summary_ws.data_validation('C7',{"validate": "list", "source": f"=Data!$C$1:$R$1"})
    summary_ws.data_validation('B7',{"validate": "list", "source": f"=Data!$C$1:$R$1"})
    summary_ws.data_validation('B4',{"validate": "list", "source": f"=Data!$S$7:$S$8"})
    summary_ws.data_validation('B5',{"validate": "list", "source": f"=Data!$S$2:$S$5"})

    summary_ws.merge_range('H7:I7','Confidence Interval (EV)',formats['title_tblr'])
    summary_ws.data_validation('H7',{"validate": "list", "source": f"=Data!$S$10:$S$11"})

    summary_ws.conditional_format(f'$B$8:$C${7 + len(filenames)}', {'type':'formula','criteria': '=OR(B$7=Data!$S$13)','format':formats['long_percent']})
    summary_ws.conditional_format(f'$B$8:$C${7 + len(filenames)}', {'type':'formula','criteria': '=OR(B$7=Data!$S$14:$S$18)','format':formats['percent']})
    summary_ws.conditional_format(f'$B$8:$C${7 + len(filenames)}', {'type':'formula','criteria': '=OR(B$7=Data!$S$19:$S$22)','format':formats['large_int']})
    summary_ws.conditional_format(f'$B$8:$C${7 + len(filenames)}', {'type':'formula','criteria': '=OR(B$7=Data!$S$23:$S$25)','format':formats['decimal']})
    summary_ws.conditional_format(f'$F$8:$G${7 + max_trackers}', {'type':'formula','criteria': '=$B$4=Data!$S$7','format':formats['really_long_percent']})
    summary_ws.conditional_format(f'$H$8:$I${7 + max_trackers}', {'type':'formula','criteria': '=AND($B$4=Data!$S$7,$H$7=Data!$S$10)','format':formats['really_long_percent']})
    summary_ws.conditional_format(f'$K$8:$K${7 + max_trackers}', {'type':'cell','criteria': '>','value':0,'format':formats['really_long_percent']})
    summary_ws.conditional_format(f'$L$8:$L${7 + max_trackers}', {'type':'cell','criteria': '>','value':0,'format':formats['decimal']})
    summary_ws.conditional_format(f'$M$8:$M${7 + max_trackers}', {'type':'cell','criteria': '>','value':0,'format':formats['large_int']})

    summary_ws.write_comment('B2','This cell will filter the Sim Names on the left.\nUse an asterisk * to represent any string of characters', {'x_scale': 2.1, 'y_scale': .5})
    summary_ws.write_comment('C2','This cell will filter the Tracker Names on the right.\nUse an asterisk * to represent any string of characters', {'x_scale': 2.1, 'y_scale': .5})
    summary_ws.write_comment('B3','This is the level of confidence used in the confidence intervals', {'x_scale': 1.25, 'y_scale': .5})
    summary_ws.write_comment('B4','Use the drop down to determine if you want values shown in credits or as a ratio to the total bet', {'x_scale': 2, 'y_scale': .5})
    summary_ws.write_comment('B5','Use the drop down to select the type of hits to use in the calculations.\nGame Hits: Number of games this tracker was hit\nGame Wins: Number of games this tracker was hit with a positive value\nTotal Hits: Number of times this tracker was hit\nTotal Wins: Number of times this tracker was hit with a positive value', {'x_scale': 2.8, 'y_scale': 1})
    summary_ws.write_comment('C7','Use the drop down in this cell and the one to the left to compare various sim values across paytables', {'x_scale': 2, 'y_scale': .5})
    summary_ws.write_comment('F2','Use the drop down to select the Sim you want to view the details of', {'x_scale': 1.5, 'y_scale': .5})
    summary_ws.write_comment('H7','Use the drop down to switch between the confidence interval being centered around the EV or the Avg Pay', {'x_scale': 2.2, 'y_scale': .5})


    # Data Worksheet
    data_ws.set_column('A:A',30,formats['center'])
    data_ws.set_column('B:S',20,formats['center'])

    data_ws.write_row('A1',data_title_row,formats['title'])
    data_ws.write_column('A2',filenames)

    data_ws.write_dynamic_array_formula('B2',f'=XMATCH({names_range},Input!$A:$A)')
    data_ws.write_dynamic_array_formula('C2','=INDEX(Input!$A:$K,ANCHORARRAY(Data!$B$2),2)')
    data_ws.write_dynamic_array_formula('D2','=INDEX(Input!$A:$K,ANCHORARRAY(Data!$B$2),3)')
    data_ws.write_dynamic_array_formula('E2','=ROUND(ANCHORARRAY($C$2),4)')
    data_ws.write_dynamic_array_formula('F2','=2*10000*ABS(ANCHORARRAY($C$2)-ANCHORARRAY($E$2))')
    data_ws.write_dynamic_array_formula('G2','=NORM.DIST((ANCHORARRAY($C$2)-ANCHORARRAY($E$2)+0.00005)/(ANCHORARRAY($D$2)/SQRT(ANCHORARRAY($I$2))),0,1,TRUE)-NORM.DIST((ANCHORARRAY($C$2)-ANCHORARRAY($E$2)-0.00005)/(ANCHORARRAY($D$2)/SQRT(ANCHORARRAY($I$2))),0,1,TRUE)')
    data_ws.write_dynamic_array_formula('H2','=ANCHORARRAY($D$2)*SQRT(ANCHORARRAY($J$2))')
    data_ws.write_dynamic_array_formula('I2','=INDEX(Input!$A:$K,ANCHORARRAY(Data!$B$2),4)')
    data_ws.write_dynamic_array_formula('J2','=INDEX(Input!$A:$K,ANCHORARRAY(Data!$B$2),5)')
    data_ws.write_dynamic_array_formula('K2','=ANCHORARRAY($I$2)*ANCHORARRAY($J$2)')
    data_ws.write_dynamic_array_formula('L2','=INDEX(Input!$A:$K,ANCHORARRAY(Data!$B$2),6)')
    data_ws.write_dynamic_array_formula('M2','=INDEX(Input!$A:$K,ANCHORARRAY(Data!$B$2),7)')
    data_ws.write_dynamic_array_formula('N2','=INDEX(Input!$A:$K,ANCHORARRAY(Data!$B$2),8)')
    data_ws.write_dynamic_array_formula('O2','=INDEX(Input!$A:$K,ANCHORARRAY(Data!$B$2),9)')
    data_ws.write_dynamic_array_formula('P2','=INDEX(Input!$A:$K,ANCHORARRAY(Data!$B$2),10)')
    data_ws.write_dynamic_array_formula('Q2','=INDEX(Input!$A:$K,ANCHORARRAY(Data!$B$2),11)')
    data_ws.write_dynamic_array_formula('R2',f'=MAP(ANCHORARRAY($B$2),LAMBDA(_xlpm.row,XMATCH(TRUE,ISBLANK(INDEX(Input!$A:$A,SEQUENCE({2 + max_trackers},,_xlpm.row))))))-2')
    data_ws.write_column('S2', ['Game Hits', 'Game Wins', 'Total Hits', 'Total Wins', '', 'Per Bet', 'Credits', '', 'Confidence Interval (EV)', 'Confidence Interval (Avg Pay)', '', 'Simmed EV', 'Rounded EV', 'Rounding Risk', 'Rounding Confidence', 'Hit Rate', 'Win Rate', 'Number of Trials', 'Trial Size', 'Number of Games', 'Bet', 'Volatility', 'Max Win', 'Avg Max Win'])

    # Input Worksheet
    input_ws.set_column('A:A',30)
    input_ws.set_column('B:K',15)

    return


def SimAnalyzerToExcel():
    sim_output_files = os_sorted([f for f in os.listdir() if f.startswith('SimsOutput') and f.endswith('.txt')])
    values_per_tracker = 6
    max_trackers = 1
    workbook = CreateWorkbook()
    input_ws:xlsxwriter.Workbook.worksheet_class = workbook.get_worksheet_by_name('Input')
    row_number = 0
    for file_index, file_name in enumerate(sim_output_files,1):
        print(f'{file_index}/{len(sim_output_files)} Calculating data for {file_name}')
        num_trials = 0
        ev_list, max_win_list, med_spin_list = (list() for _ in range(3))
        hits, wins = (int() for _ in range(2))
        bet, games_played,  = (set() for _ in range(2))
        tracker_ev = defaultdict(list)
        tracker_game_hits, tracker_game_wins, tracker_total_hits, tracker_total_wins = (defaultdict(int) for _ in range(4))
        # Read Data
        file = open(file_name)
        for line in file:
            if line.isspace():
                continue
            num_trials += 1
            trial_ev, trial_bet, trial_games_played, trial_max_win, trial_hits, trial_wins, trial_med_spins, *trial_trackers = [ConvertType(value) for value in line.split('\t')]
            ev_list.append(trial_ev)
            max_win_list.append(trial_max_win)
            med_spin_list.append(trial_med_spins)
            hits += trial_hits
            wins += trial_wins
            bet.add(trial_bet)
            games_played.add(trial_games_played)
            for name, ev, game_hits, game_wins, total_hits, total_wins in batched(trial_trackers, values_per_tracker):
                tracker_ev[name].append(ev)
                tracker_game_hits[name] += game_hits
                tracker_game_wins[name] += game_wins
                tracker_total_hits[name] += total_hits
                tracker_total_wins[name] += total_wins
        for name in tracker_ev:
            tracker_ev[name] += [0.0] * (num_trials - len(tracker_ev[name]))
        file.close()
        
        # Analyze Data
        games_played = max(games_played)
        bet = max(bet)
        total_ev = stat.mean(ev_list)
        total_stdev = stat.stdev(ev_list) if num_trials > 1 else 0
        hit_rate = hits / games_played / num_trials
        win_rate = wins / games_played / num_trials
        average_max_win = stat.mean(max_win_list)
        total_max_win = max(max_win_list)
        med_spins = stat.median(med_spin_list)
        tracker_total_ev = dict()
        tracker_total_stdev = dict()
        max_trackers = max(max_trackers, len(tracker_ev))
        for name in tracker_ev:
            tracker_total_ev[name] = stat.mean(tracker_ev[name])
            tracker_total_stdev[name] = stat.stdev(tracker_ev[name]) if num_trials > 1 else 0
        
        # Output to file
        
        input_ws.write_row(row_number, 0, [file_name[11:-4],total_ev,total_stdev,num_trials,games_played,bet,hit_rate,win_rate,total_max_win,average_max_win,med_spins])
        row_number+=1
        for name in tracker_total_ev:
            input_ws.write_row(row_number, 0,[name,tracker_total_ev[name],tracker_total_stdev[name],tracker_game_hits[name],tracker_game_wins[name],tracker_total_hits[name],tracker_total_wins[name]])
            row_number+=1
        row_number+=1
        
    UpdateWorkbook(workbook,sim_output_files,max_trackers)
    workbook.close()

if __name__ == '__main__':
    SimAnalyzerToExcel()