'''
This code is produced by Tacocat Labs LLC for Motus Data LLC,
for purposes of their Dyna Card Shape Recognition project.
This code is proprietary.
'''


#import numpy as np
import pandas as pd
import os, code
#from scipy import stats
#from scipy.ndimage import imread
from matplotlib.pyplot import imshow
from matplotlib import pyplot as plt
#from scipy.stats import linregress
import subprocess as sp

pd.set_option('display.width', 1000)

EXAMPLE_DATA_DIR = 'example_data/'
def plot_example_data():
    # Turn hand-edited PNGs into CSV files, and scatter plot to visualize
    for f in os.listdir(EXAMPLE_DATA_DIR):
        if not f.endswith('.csv'): continue
        df = pd.read_csv(EXAMPLE_DATA_DIR + f, comment='#')
        plt.close()
        plt.scatter(df.length, df.weight)#, marker='.')
        plt.title(f.replace('.csv', '').replace('_', ' '))
        plt.savefig(EXAMPLE_DATA_DIR + f.replace('.csv', '.png'))

def test_example_data():
        _ = sp.check_output(['g++', 'classify_pump_state.cpp', '-lstdc++fs'])
        sp.check_output(['./a.out', EXAMPLE_DATA_DIR, '10.0'])
        
    
EXAMPLE_FILES = [
'full_pump.csv',
'tubing_movement.csv',
'fluid_pound.csv',
'flowing_well.csv',
'gas_interference.csv',
'pump_hitting.csv',
'bent_barrel.csv',
'worn_plunger.csv',
'worn_standing.csv',
'worn_or.csv',
'fluid_friction.csv',
'drag_friction.csv',
]
def test():
    _ = sp.check_output(['g++', 'classify_pump_state.cpp', '-lstdc++fs'])
    for f in EXAMPLE_FILES:
        #for f in ['gas_interference.csv']:
        if not f.endswith('.csv'): continue
        output = sp.check_output(['./a.out', EXAMPLE_DATA_DIR+f, '10.0'])
        state_pred = output.strip().split('\n')[-1]
        print f, ': ', state_pred
        if f.replace('.csv','').replace('_', ' ') not in output:
            print '  * error'

if __name__=='__main__':
    plot_example_data()
    test_example_data()
    #test()
