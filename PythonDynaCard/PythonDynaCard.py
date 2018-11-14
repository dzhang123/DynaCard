

import sys

def parse_file (file):
    filename = file
    f = open(filename, 'rU')
    points = []
    for line in f:
        if line.strip().startswith('#'):
            continue
        row = line.strip().split(',')
        points.append((row[0], row[1], row[2]))

    f.close()
    return points

def main():
    if len(sys.argv[1]) > 1 :        
        print (parse_file('full_pump.csv'))


if __name__ == '__main__':
    main()