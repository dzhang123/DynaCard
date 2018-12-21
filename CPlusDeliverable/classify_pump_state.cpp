#include "pch.h"

/*

g++ hello.cpp

*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include<cmath>
//missing library
#include <algorithm>
using namespace std;

const string WELL_ID_NUMBER = "Well ID Number";
const string TIMESTAMP = "Timestamp";
const string DEVICE_SERIAL_NUMBER = "Device Serial Number";
const string SENSOR_SERIAL_NUMBER = "Sensor Serial Number";

struct FileHeader {
	string well_id_number;
	string timestamp;
	string deviceSerial_Number;
	string sensorSerial_Number;
};

bool peek_file(string fname, FileHeader* header) {
	string line;
	ifstream ifs(fname);
	//FileHeader header;
	bool successful = false;
	int tally = 0;
	while (ifs.good()) {
		getline(ifs, line, '\n');
		if (line[0] == '#') {
			size_t found = line.find(WELL_ID_NUMBER);
			if (found != string::npos) {
				// found well id number;
				found = line.find(":", found + 1);
				header->well_id_number = line.substr(found + 1);
				tally++;
				continue;
			}
			found = line.find(TIMESTAMP);
			if (found != string::npos) {
				// found timestamp;
				found = line.find(":", found + 1);
				header->timestamp = line.substr(found + 1);
				tally++;
				continue;
			}
			found = line.find(DEVICE_SERIAL_NUMBER);
			if (found != string::npos) {
				// found serial number
				found = line.find(":", found + 1);
				header->deviceSerial_Number = line.substr(found + 1);
				tally++;
				continue;
			}
			found = line.find(SENSOR_SERIAL_NUMBER);
			if (found != string::npos) {
				// found sensor number
				found = line.find(":", found + 1);
				header->sensorSerial_Number = line.substr(found + 1);
				tally++;
				continue;
			}
		}
		if (tally == 4) {
			successful = true;
			break;
		}
	}
	ifs.close();
	return successful;
}

std::string& ltrim(std::string& str, const std::string& chars = "\t\n\v\f\r") {
	str.erase(0, str.find_first_not_of(chars));
	return str;
}

std::string& rtrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
{
	str.erase(str.find_last_not_of(chars) + 1);
	return str;
}

std::string& trim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
{
	return ltrim(rtrim(str, chars), chars);
}

// Turn file into triple of pos/x/y vectors
vector<vector<double> > parse_file(string fname) {
  // Read each column into its own vector
  vector <double> positionVec;
  vector <double> xVec;
  vector <double> yVec;
  string line;
  vector<double> prsd;
  ifstream ifs (fname);
  while ( ifs.good() )
  {
       getline ( ifs, line, '\n' );
       if (line=="position,length,weight") continue;
       if (line[0]=='#') continue;
       stringstream ss( line );
       string number;
       int i=0;
       while ( getline(ss, number, ',') ) {
         if (i==0) positionVec.push_back(stod(number));
         else if (i==1) xVec.push_back(stod(number));
         else if (i==2) yVec.push_back(stod(number));
         else throw 20;
         i++;
       }
  }
  // find indices of first pos=0 and second pos=0, to find one cycle
  int first_zero_ind = find (positionVec.begin(), positionVec.end(), 0) - positionVec.begin();
  int second_zero_ind = find (positionVec.begin()+first_zero_ind+1, positionVec.end(), 0) - (positionVec.begin()+first_zero_ind);
  if (second_zero_ind==-1) second_zero_ind=positionVec.size();
  /*cout << "first_zero_ind " << first_zero_ind << endl
    << "second_zero_ind " << second_zero_ind << endl;*/
  // Make sub-vectors containing only one cycle
  vector<double> posReturn, xReturn, yReturn;
  for (int i=first_zero_ind; i<second_zero_ind; i++) {
    posReturn.push_back(positionVec[i]);
    xReturn.push_back(xVec[i]);
    yReturn.push_back(yVec[i]);
  }
  /*cout << "  n points " << positionVec.size() << endl
    << "  first_zero_ind " << first_zero_ind << endl
    << "  second_zero_ind " << second_zero_ind << endl;*/
  //
  vector<vector<double> > to_return;
  to_return.push_back(posReturn); to_return.push_back(xReturn); to_return.push_back(yReturn);
  //to_return.push_back(positionVec); to_return.push_back(xVec); to_return.push_back(yVec);
  return to_return;
}

// Normalize a vector
vector<double> normalize(vector<double> inVec) {
  vector<double> outVec;
  double min_val = inVec[0];
  double max_val = inVec[0];
  double current_val = 0.0;
  for (int i = 0; i<inVec.size(); i++) {
    current_val=inVec[i];
    if (current_val<min_val) min_val=current_val;
    if (current_val>max_val) max_val=current_val;
  }
  for (int i = 0; i<inVec.size(); i++)
    outVec.push_back((inVec[i]-min_val)/(max_val-min_val));
  return outVec;
}

struct FittedLine {
  double slope;
  double intercept;
  double r2;
};

FittedLine fit_a_line(vector<double> xs, vector<double> ys) {
  double xsum=0,x2sum=0,y2sum=0,ysum=0,xysum=0,r2=0;
  int n_points = xs.size();
  for (int i=0;i<n_points;i++) {
      xsum += xs[i];
      ysum += ys[i];
      x2sum += pow(xs[i],2);
      y2sum += pow(ys[i],2);
      xysum += xs[i]*ys[i];
  }
  double slope = (n_points*xysum-xsum*ysum)/(n_points*x2sum-xsum*xsum);
  double intercept = (x2sum*ysum-xsum*xysum)/(x2sum*n_points-xsum*xsum);
  //slope = (xysum + (ysum/n_points)*xsum + (xsum/n_points)*ysum + n_points*(ysum/n_points)*(xsum/n_points));
  //intercept = (ysum/n_points) - slope*(xsum/n_points);
  // Goodness of fit
  for (int i=0;i<n_points;i++) {
    double y_pred = slope*xs[i]+intercept;
    r2 += pow(y_pred-ys[i], 2);
  }
  r2 /= n_points;
  FittedLine fit;
  fit.slope=slope; fit.intercept=intercept; fit.r2=r2;
  return fit;
}

class Edge {
public:
  string name;
  FittedLine normal_fitted_line;
  FittedLine inverse_fitted_line;
  // Raw data
  int n_points;
  vector<double> xs, ys;
  // Fitted data
  double slope, intercept, r2;
  double length;
  //
  Edge(string nm) {
    name = nm;
    n_points = 0;
  }
  void display() {
    cout << "Name: " << name << endl
      << "  n points " << n_points << endl
      << "  slope " << slope << endl
      << "  vertical " << vertical() << endl
      << "  flat " << flat() << endl
      << "  length " << length << endl
      << "  r2 " << r2 << endl
      << "  inv r2 " << inverse_fitted_line.r2 << endl;
  }
  void add_point(double x, double y) {
    xs.push_back(x);
    ys.push_back(y);
    n_points++;
  }
  void finish() {
    if (n_points==0) {
      cout << "ERROR: " << name << " had no points" << endl;
    }
    normal_fitted_line = fit_a_line(xs, ys);
    inverse_fitted_line = fit_a_line(ys, xs);
    slope = normal_fitted_line.slope;
    intercept = normal_fitted_line.intercept;
    r2 = normal_fitted_line.r2;
    double x_diff_sqr = pow(xs[0]-xs[n_points-1], 2);
    double y_diff_sqr = pow(ys[0]-ys[n_points-1], 2);
    length = pow(x_diff_sqr+y_diff_sqr, 0.5);
  }
  Edge first_half() {
    Edge ret("first half of " + name);
    for (int i=0; i<n_points; i++) {
      double x_diff_sqr = pow(xs[0]-xs[i], 2);
      double y_diff_sqr = pow(ys[0]-ys[i], 2);
      double dist_from_start = pow(x_diff_sqr+y_diff_sqr, 0.5);
      if (dist_from_start<=0.5*length) {
        ret.add_point(xs[i], ys[i]);
      } else {
        break;
      }
    }
    ret.finish();
    return ret;
  }
  //
  // Properties an edge might have
  //
  bool good_fit() {
    return (normal_fitted_line.r2<0.002) | (inverse_fitted_line.r2<0.002);
  }
  bool vertical() {
    return good_fit() & (abs(inverse_fitted_line.slope)<0.1);
  }
  bool slope_up() {
    return good_fit() & (normal_fitted_line.slope>0.5);
  }
  bool slope_down() {
    return good_fit() & (normal_fitted_line.slope<-0.5);
  }
  bool flat() {
    return good_fit() & (abs(slope)<0.1);
  }
};

class Shape {
public:
  Edge *left, *top, *right, *bottom;
  Shape(Edge* e0, Edge* e1, Edge* e2, Edge* e3) {
    left=e0; top=e1; right=e2; bottom=e3;
  }
  //
  // Properties a shape might have
  //
  double top_width() {
    return top->length;
  }
  double bottom_width() {
    return bottom->length;
  }
  bool left_edge_vertical() {
    return isnan(left->slope) | (abs(left->slope)>50);
  }
  bool top_edge_flat() {
    return abs(top->slope)<0.1;
  }
  bool bottom_edge_flat() {
    return abs(bottom->slope)<0.1;
  }
};

int increment_with_rollover(int i, int max) {
  i++;
  if (i==max) i=0;
  return i;
}

vector<Edge> break_into_edges(vector<double> position, vector<double> xs, vector<double> ys) {
  // Identify indices of the corners of the trapezoid
  vector<double> direction_upper_right, direction_lower_right;
  for (int i=0; i<position.size(); i++) {
    direction_upper_right.push_back(xs[i]+2*ys[i]);
    direction_lower_right.push_back(xs[i]-2*ys[i]);
  }
  int lower_left_ind = min_element(direction_upper_right.begin(), direction_upper_right.end())-direction_upper_right.begin();
  int upper_right_ind = max_element(direction_upper_right.begin(), direction_upper_right.end())-direction_upper_right.begin();
  int upper_left_ind = min_element(direction_lower_right.begin(), direction_lower_right.end())-direction_lower_right.begin();
  int lower_right_ind = max_element(direction_lower_right.begin(), direction_lower_right.end())-direction_lower_right.begin();
  // Create edges based on those corners
  Edge left("left"), top("top"), right("right"), bottom("bottom");
  int i = lower_left_ind;
  int max_i = position.size();
  while (i != upper_left_ind) {
    left.add_point(xs[i], ys[i]);
    i = increment_with_rollover(i, max_i);
  }
  while (i != upper_right_ind) {
    top.add_point(xs[i], ys[i]);
    i = increment_with_rollover(i, max_i);
  }
  while (i != lower_right_ind) {
    right.add_point(xs[i], ys[i]);
    i = increment_with_rollover(i, max_i);
  }
  while (i != lower_left_ind) {
    bottom.add_point(xs[i], ys[i]);
    i = increment_with_rollover(i, max_i);
  }
  left.finish(); top.finish(); right.finish(); bottom.finish();
  // Combine them and return
  vector<Edge> ret;
  ret.push_back(left); ret.push_back(top); ret.push_back(right); ret.push_back(bottom);
  return ret;
}

string guess_pump_state(Shape shape) {
  /*shape.left->display();
  shape.top->display();
  shape.right->display();
  shape.bottom->display();*/
  // Full pump
  if (shape.left->vertical() & shape.right->vertical()
    & shape.top->flat() & shape.bottom->flat()) return "full pump";
  // Tubing movement
  else if (shape.top->flat() & shape.bottom->flat()
    & shape.left->slope_up() & shape.left->good_fit()
    & shape.right->slope_up() & shape.right->good_fit()) return "tubing movement";
  // Fluid pound
  else if (shape.top->flat() & shape.bottom->flat()
    & shape.left->vertical()
    & (shape.bottom->length<0.8) & (shape.right->normal_fitted_line.r2>0.015)
    ) return "fluid pound";
  // Gas interference
  else if (shape.top->flat()
    & shape.left->vertical()
    & shape.bottom->flat() & (shape.bottom->length<0.8)) return "gas interference";
  // Pump hitting
  else if (shape.left->vertical() & shape.right->vertical()
    & shape.top->first_half().flat() & shape.bottom->first_half().flat()) return "pump hitting";
  // Bent barrel
  else if (shape.left->vertical() & shape.right->vertical()
    & (shape.bottom->length>0.8) & (shape.top->length>0.8)) return "bent barrel";
  // Worn plunger
  else if (shape.bottom->flat()
    & ~shape.left->vertical()
    & ~shape.right->vertical()
    & (shape.top->length<0.9)) return "worn plunger";
  // Worn standing
  else if (shape.top->flat()
    & ~shape.left->vertical()
    & ~shape.right->vertical()
    & (shape.bottom->length<0.9)) return "worn standing";
  // Worn or split
  else if (shape.bottom->flat()
    & shape.left->vertical()) return "worn or";
  // Fluid friction
  else if (shape.right->vertical()
    & shape.left->vertical()) return "fluid friction";
  // Drag friction
  else if ((shape.right->length>0.7)
    & (shape.left->length>0.7)) return "drag friction";
  else return "other??";
}

string output_json(FileHeader header, string state) {
	string json = "{\n";
	json += "\"well_id\" : \"" + trim(header.well_id_number) + "\", \n";
	json += "\"pump_status\" : \"" + trim(state) + "\", \n";
	json += "\"deviceSerial\" : " + trim(header.deviceSerial_Number) + ", \n";
	json += "\"sensorSerial\" : \"" + trim(header.sensorSerial_Number) + "\", \n";
	json += "\"timestamp\" : " + trim(header.timestamp) + "\n";
	json += "}\n";

	return json;
}

int main(int argc, char *argv[]) {
    // get filename and minimum weight from command line
    double min_acceptable_peak_weight = stod(argv[2]);
    string fname(argv[1]);
	bool isDeviceSerialParamPresent = false;
	string deviceSerialParam = "";
	bool isTimestampParamPresent = false;
	string timestampParam = "";
	if (argc >= 4) {
		isDeviceSerialParamPresent = true;
		deviceSerialParam = argv[3];
	}
	if (argc >= 5) {
		isTimestampParamPresent = true;
		timestampParam = argv[4];
	}
	
    // Read in the file
	FileHeader header;
	bool headerParsed = peek_file(fname, & header );
	// overwrite device serial number and timestamp from command-line parameter
	if (isDeviceSerialParamPresent) {
		header.deviceSerial_Number = deviceSerialParam;
	}
	if (isTimestampParamPresent) {
		header.timestamp = timestampParam;
	}
    vector<vector<double> > position_x_y = parse_file(fname);
    vector<double> position = position_x_y[0];
    vector<double> xs = normalize(position_x_y[1]);
    vector<double> ys = normalize(position_x_y[2]);
    // Diagnose flowing well based on max weight
    int max_ind = max_element(ys.begin(), ys.end())-ys.begin();
    //cout << max_ind << endl;
    //cout << "max val " << position_x_y[2][max_ind] << endl;
    if (position_x_y[2][max_ind]<min_acceptable_peak_weight) {
		cout << output_json(header, "flowing well") << endl;
      //cout << "flowing well" << endl;
      return 0;
    }
    // Otherwise break into edges
    vector<Edge> edges = break_into_edges(position, xs, ys);
    Shape shape(&edges[0], &edges[1], &edges[2], &edges[3]);
    // And classify based on shape
    string state = guess_pump_state(shape);
	cout << output_json(header, state) << endl;

    return 0;
}

/*
g++ classify_pump_state.cpp
./a.out example_data/flowing_well.csv 60.0
*/
