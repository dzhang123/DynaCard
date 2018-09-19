'''
This code is produced by Tacocat Labs LLC for Motus Data LLC,
for purposes of their Dyna Card Shape Recognition project.
This code is proprietary.
'''


import numpy as np
import pandas as pd
import os, code
from scipy import stats
from scipy.ndimage import imread
from matplotlib.pyplot import imshow
from matplotlib import pyplot as plt
from scipy.stats import linregress

pd.set_option('display.width', 1000)
DATA_DIR = 'DYNA Screenshots clean/'

class Line:
    def __init__(self, pt1, pt2):
        self.pt1=pt1
        self.pt2=pt2
        x1, y1 = pt1
        x2, y2 = pt2
        # ax+by+c=0
        self.a = 1.0
        self.b = -1*float(x1-x2)/(y1-y2)
        self.c = -1*(x1+self.b*y1)
        #
        self.slope = float(y2-y1) / (x2-x1)
        self.intercept = y1-self.slope*x1
    def dist(self, pt):
        return self.dist_from_line(pt)
        #return self.dist_from_segment(pt)
    def dist_from_line(self, pt):
        x, y = pt
        return np.abs(self.a*x + self.b*y + self.c) / np.sqrt(self.a**2+self.b**2)
    def dist_from_segment(self, pt):
        x, y = pt
        x1, y1 = self.pt1
        x2, y2 = self.pt2
        vector_x, vector_y = (x2-x1), (y2-y1)
        dot_prod = vector_x*x + vector_y*y
        dot_prod1 = vector_x*x1 + vector_y*y1
        dot_prod2 = vector_x*x2 + vector_y*y2
        if dot_prod>dot_prod2:
            return np.sqrt((x-x2)**2+(y-y2)**2)
        elif dot_prod<dot_prod1:
            return np.sqrt((x-x1)**2+(y-y1)**2)
        else: return self.dist_from_line(pt)
    def intersection(self, ln):
        y = -1*(ln.c-self.c)/(ln.b-self.b)
        x = -1*(self.c+self.b*y)
        return x, y
    def angle_from_vertical(self):
        x1, y1 = self.pt1
        x2, y2 = self.pt2
        return np.arctan((x2-x1)/(y2-y1))
    def plot(self, c='r'):
        pt1, pt2 = self.pt1, self.pt2
        plt.plot([pt1[0], pt2[0]], [pt1[1], pt2[1]], 'ro-', color=c)
        #plt.plot((0,self.intercept), (0,self.intercept+self.slope), 'ro-')
    @classmethod
    def from_slope_intercept(cls, slope, intercept):
        return cls((0.0,intercept), (1.0,intercept+slope))

class Trapezoid:
    def __init__(self, left_edge, top_edge, right_edge, bottom_edge):
        bottom_left = bottom_edge.intersection(left_edge)
        top_left = left_edge.intersection(top_edge)
        top_right = top_edge.intersection(right_edge)
        bottom_right = right_edge.intersection(bottom_edge)
        self.left_edge = Line(bottom_left, top_left)
        self.top_edge = Line(top_left, top_right)
        self.right_edge = Line(top_right, bottom_right)
        self.bottom_edge = Line(bottom_right, bottom_left)
        self.bottom_left = bottom_left
        self.top_left = top_left
        self.bottom_right = bottom_right
    def dist(self, pt):
        return min(
            self.left_edge.dist_from_segment(pt),
            self.top_edge.dist_from_segment(pt),
            self.right_edge.dist_from_segment(pt),
            self.bottom_edge.dist_from_segment(pt)
            )
    def area(self):
        x_orig, y_orig = self.bottom_left
        x_up, y_up = self.top_left
        x_right, y_right = self.bottom_right
        dot_product = (x_right-x_orig)*(x_up-x_orig) + (y_right-y_orig)*(y_up-y_orig)
        return dot_product


'''
Init:
upper left/right are used as vertices of the

'''

def get_initial_edges(df):
    df['xy'] = list(zip(df.x, df.y))
    df['dist_upper_right'] = df.x+df.y
    df['dist_lower_right'] = df.x-df.y
    lower_left_ind = df.dist_upper_right.argmin()
    upper_left_ind = df.dist_lower_right.argmin()
    upper_right_ind = df.dist_upper_right.argmax()
    lower_right_ind = df.dist_lower_right.argmax()
    #
    left_edge = Line( df.ix[lower_left_ind][['x','y']], df.ix[upper_left_ind][['x','y']] )
    top_edge = Line( df.ix[upper_left_ind][['x','y']], df.ix[upper_right_ind][['x','y']] )
    right_edge = Line( df.ix[upper_right_ind][['x','y']], df.ix[lower_right_ind][['x','y']] )
    bottom_edge = Line( df.ix[lower_right_ind][['x','y']], df.ix[lower_left_ind][['x','y']] )
    return left_edge, top_edge, right_edge, bottom_edge

def get_refined_edges(df):
    left_edge, top_edge, right_edge, bottom_edge = get_initial_edges(df)
    df['dist_left'] = df.xy.apply(lambda pt: left_edge.dist(pt))
    df['dist_top'] = df.xy.apply(lambda pt: top_edge.dist(pt))
    df['dist_right'] = df.xy.apply(lambda pt: right_edge.dist(pt))
    df['dist_bottom'] = df.xy.apply(lambda pt: bottom_edge.dist(pt))
    df['min_dist'] = df[['dist_left', 'dist_top', 'dist_right', 'dist_bottom']].min(axis=1)
    df['assigned_line'] = 'left'
    df['assigned_line'].ix[df.min_dist==df.dist_top] = 'top'
    df['assigned_line'].ix[df.min_dist==df.dist_right] = 'right'
    df['assigned_line'].ix[df.min_dist==df.dist_bottom] = 'bottom'
    fitted_lines = {}
    for edge in ['left', 'top', 'right', 'bottom']:
        ddf = df[df.assigned_line==edge]
        slope, intercept, r_value, p_value, std_err = stats.linregress(ddf.x, ddf.y)
        fitted_line = Line.from_slope_intercept(slope, intercept)
        fitted_lines[edge] = fitted_line
    return fitted_lines['left'], fitted_lines['top'], fitted_lines['right'], fitted_lines['bottom']

def get_df_from_file(f):
    x = imread(DATA_DIR + f)
    reds = x[:,:,0]
    ys, xs = np.where(reds<=200)
    df = pd.DataFrame({'x':xs, 'y':ys.max()-ys})
    df['x'] = (df.x-df.x.min()).astype(float) / (df.x.max()-df.x.min())
    df['y'] = (df.y-df.y.min()).astype(float) / (df.y.max()-df.y.min())
    df['xy'] = list(zip(df.x, df.y))
    return df

def plot_graphs_w_trapezoids():
    # Turn hand-edited PNGs into CSV files, and scatter plot to visualize
    for f in os.listdir(DATA_DIR):
      #x = imread('Screen Shot 2018-08-11 at 12.43.27 PM.png')
      if 'clean' in f or 'trap' in f or not f.endswith('.png'): continue
      print f
      df = get_df_from_file(f)
      left_edge, top_edge, right_edge, bottom_edge = get_initial_edges(df)
      # Plt points w starting trapezoid
      plt.close()
      plt.scatter(df.x, df.y, marker='.')
      left_edge.plot('r')
      top_edge.plot('g')
      right_edge.plot('b')
      bottom_edge.plot('y')
      plt.xlim(-0.1, 1.1)
      plt.ylim(-0.1, 1.1)
      plt.savefig('initial/' + f.replace('.png', '_clean.png'))
      #df.to_csv(f.replace('.png', '.csv'), index=False)
      # Plt points w refined trapezoid
      left_edge, top_edge, right_edge, bottom_edge = get_refined_edges(df)
      plt.close()
      plt.scatter(df.x, df.y, marker='.')
      left_edge.plot('r')
      top_edge.plot('g')
      right_edge.plot('b')
      bottom_edge.plot('y')
      plt.xlim(-0.1, 1.1)
      plt.ylim(-0.1, 1.1)
      plt.savefig('fitted/' + f.replace('.png', '_clean_fitted.png'))
      #df.to_csv(f.replace('.png', '.csv'), index=False)

def get_stats_from_file(f):
    ddf = get_df_from_file(f)
    left_edge, top_edge, right_edge, bottom_edge = get_initial_edges(ddf)
    trapezoid = Trapezoid(left_edge, top_edge, right_edge, bottom_edge)
    ddf['dist_from_trap'] = ddf['xy'].apply(lambda pt: trapezoid.dist(pt))
    #res['avg_dist_from_trap'] = df.dist_from_trap.mean()
    return dict(
        file=f,
        good='good' in f,
        top_slope=np.abs(top_edge.slope),
        bottom_slope=np.abs(bottom_edge.slope),
        avg_dist_from_trap=ddf.dist_from_trap.mean(),
        right_angle_from_vertical=right_edge.angle_from_vertical()
    )

def get_features_df():
    records = []
    for f in os.listdir(DATA_DIR):
        if 'clean' in f or 'trap' in f or not f.endswith('.png'): continue
        print f
        rec = get_stats_from_file(f)
        records.append(rec)
    return pd.DataFrame(records).sort_values(by='good')


EXAMPLE_DATA_DIR = 'example_data/'
def plot_example_data():
    # Turn hand-edited PNGs into CSV files, and scatter plot to visualize
    for f in os.listdir(EXAMPLE_DATA_DIR):
        if not f.endswith('.csv'): continue
        df = pd.read_csv(EXAMPLE_DATA_DIR + f, comment='#')
        plt.close()
        plt.scatter(df.length, df.weight)#, marker='.')
        plt.savefig(EXAMPLE_DATA_DIR + f.replace('.csv', '.jpg'))

EXAMPLE_FILES = [
'full_pump.csv',
'tubing_movement.csv',
'fluid_pound.csv',
'gas_interference.csv',
'pump_hitting.csv',
'bent_barrel.csv',
'worn_plunger.csv',
'worn_standing.csv',
'worn_or.csv',
'fluid_friction.csv',
'drag_friction.csv',
]
import subprocess as sp
def test():
    _ = sp.check_output(['g++', 'classify_pump_state.cpp'])
    for f in EXAMPLE_FILES:
        #for f in ['gas_interference.csv']:
        if not f.endswith('.csv'): continue
        output = sp.check_output(['./a.out', EXAMPLE_DATA_DIR+f, '60.0'])
        state_pred = output.strip().split('\n')[-1]
        print f, ': ', state_pred
        if f.replace('.csv','').replace('_', ' ') not in output:
            print '  * error'

if __name__=='__main__':
    plot_example_data()
    test()
    #plot_graphs_w_trapezoids()
    #df = get_features_df()
    #print df.groupby('good').mean()
    #code.interact(local=dict(list(locals().items())+list(globals().items())))

'''
df['guess'] = True
df['guess'].ix[df.right_angle_from_vertical>0.1] = False
df['guess'].ix[df.bottom_slope>0.25] = False
df['guess'].ix[df.top_slope>0.25] = False
'''


'''

ax = df[df.good].plot(kind='scatter', x='bottom_slope', y='top_slope', color='b')
df[~df.good].plot(kind='scatter', x='bottom_slope', y='top_slope', color='r', marker='x', ax=ax)

df[df.bottom_slope.lt(0.2) & df.top_slope.gt(-0.3)]


#f = 'good Screen Shot 2018-08-11 at 12.45.39 PM.csv'
def get_stats_from_file(f):
    df = pd.read_csv(f)
    left_edge, top_edge, right_edge, bottom_edge = get_initial_edges(df)
    #
    df['dist_left'] = df.xy.apply(lambda pt: left_edge.dist(pt))
    df['dist_top'] = df.xy.apply(lambda pt: top_edge.dist(pt))
    df['dist_right'] = df.xy.apply(lambda pt: right_edge.dist(pt))
    df['dist_bottom'] = df.xy.apply(lambda pt: bottom_edge.dist(pt))
    df['min_dist'] = df[['dist_left', 'dist_top', 'dist_right', 'dist_bottom']].min(axis=1)
    df['assigned_line'] = 'left'
    df['assigned_line'].ix[df.min_dist==df.dist_top] = 'top'
    df['assigned_line'].ix[df.min_dist==df.dist_right] = 'right'
    df['assigned_line'].ix[df.min_dist==df.dist_bottom] = 'bottom'
    df.assigned_line.value_counts()
    #
    results = {'df':df}
    for edge in ['left', 'top', 'right', 'bottom']:
        ddf = df[df.assigned_line==edge]
        slope, intercept, r_value, p_value, std_err = stats.linregress(ddf.x, ddf.y)
        fitted_line = Line.from_slope_intercept(slope, intercept)
        res = dict(slope=slope, intercept=intercept,
                r_value=r_value, r_squared=r_value**2, p_value=p_value, std_err=std_err)
        for k, v in res.items():
            res[edge+'_'+k] = v
            #del results[edge][k]
        results.update(res)
        results[edge] = res
    return results

results = []
for f in os.listdir('.'):
    #x = imread('Screen Shot 2018-08-11 at 12.43.27 PM.png')
    if not f.endswith('.csv'): continue
    res = get_stats_from_file(f)
    left, top, right, bottom = Line.from_slope_intercept(res['left']['slope'], res['left']['intercept']), \
        Line.from_slope_intercept(res['top']['slope'], res['top']['intercept']), \
        Line.from_slope_intercept(res['right']['slope'], res['right']['intercept']), \
        Line.from_slope_intercept(res['bottom']['slope'], res['bottom']['intercept'])
    trapezoid = Trapezoid(left, top, right, bottom)
    df = res['df']
    df['dist_from_trap'] = df['xy'].apply(lambda pt: trapezoid.dist(pt))
    res['avg_dist_from_trap'] = df.dist_from_trap.mean()
    res['area'] = trapezoid.area()
    res['file'] = f
    if 'good' in f: res['good']=True
    else: res['good']=False
    results.append(res)
    plt.close()
    plt.scatter(df.x, df.y, marker='.')
    plt.plot(bottom.intersection(left), left_edge.intersection(top), 'ro-')
    plt.plot(left.intersection(top), top_edge.intersection(right), 'ro-')
    plt.plot(top.intersection(right), right_edge.intersection(bottom), 'ro-')
    plt.plot(bottom.intersection(right), bottom_edge.intersection(left), 'ro-')
    plt.savefig(f.replace('.csv', '_trap.png'))





full_df = pd.DataFrame(results)
full_df['max_r_squared'] = full_df[['left_r_squared', 'top_r_squared', 'right_r_squared', 'bottom_r_squared']].max(axis=1)

full_df[['good', 'area', 'file']].sort('good')




x = imread(f)
reds = x[:,:,0]
ys, xs = np.where(reds<=200)
plt.close()
plt.scatter(xs, ys.max()-ys, marker='.')
pd.DataFrame({'x':xs, 'y':ys}).to_csv(f.replace('.png', '.csv'), index=False)
plt.savefig(f.replace('.png', '_clean.png'))




plt.plot(tuple(df.ix[upper_left][['x','y']]), tuple(df.ix[upper_right][['x','y']]), 'ro-')
plt.plot(tuple(df.ix[upper_right][['x','y']]), tuple(df.ix[lower_right][['x','y']]), 'ro-')
plt.plot(tuple(df.ix[lower_right][['x','y']]), tuple(df.ix[lower_left][['x','y']]), 'ro-')



plt.scatter(df.x, df.y, marker='.')
plt.plot(tuple(df.ix[lower_left][['x','y']]), tuple(df.ix[upper_left][['x','y']]), 'ro-')
plt.plot(tuple(df.ix[upper_left][['x','y']]), tuple(df.ix[upper_right][['x','y']]), 'ro-')
plt.plot(tuple(df.ix[upper_right][['x','y']]), tuple(df.ix[lower_right][['x','y']]), 'ro-')
plt.plot(tuple(df.ix[lower_right][['x','y']]), tuple(df.ix[lower_left][['x','y']]), 'ro-')

'''
