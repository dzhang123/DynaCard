/*
This program was written by Tacocat Labs LLC for Motus Data

It is used to identify important geometric properties of the shape
of surface Dynamocard readings.

The ideal surface card will have the following properties:
- The points lie on a 4-sided shape
- Rotating that shape 180 degrees yields the same shape

This program computes:
- The area of the shape (normalized so that x/ and between 0 and 1)
- The average distance from a datapoint to a FourSidedFigure of best fit
- The average distance from a datapoint to a FourSidedFigure of best fit,
  rotated 180 degrees.

The logical steps of the program are:
- read in the position, length (x) and weight (y) from a file
- parse one complete cycle out of that data.  The xy coordinates of this
  cycle are the shape we will study
- normalize the shape so that the x and the y values scale from 0 to 1.
  this makes it so that we are studying the shape itself, without intterference
  from the units in which measurements are taken
- compute the area of the shape using Stokes' theorem
- fit a FourSidedFigure T to the data and compute the average distance
  of the datapopints from T
- create a version of T that has been rotated 180 degrees around its
  axis of symmetry.  Compute the average distance of the datapoints from the
  rotated T.
These three stats are sent to stdout as a JSON blob.

*/

#include "pch.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include<cmath>

#include <algorithm>

using namespace std;

/*
Identify start/end of a cycle when x<=LENGTH_STARTING_THRESH.
When x moves above LENGTH_STARTED_THRESH the cycle is finished starting -
this is in case x fluctuates around LENGTH_STARTING_THRESH a little bit at first.
*/
const double LENGTH_STARTING_THRESH = 1.0;  // below this starts a cycle
const double LENGTH_STARTED_THRESH = 5.0; // above this the cycle is considered to have started

// Turn file into triple of pos/x/y vectors
vector<vector<double> > parse_file(string fname) {
	// Read each column into its own vector
	vector <double> positionVec;
	vector <double> xVec;
	vector <double> yVec;
	string line;
	vector<double> prsd;
	ifstream ifs(fname);
	while (ifs.good())
	{
		getline(ifs, line, '\n');
		if (line == "position,length,weight") continue;
		if (line[0] == '#') continue;
		stringstream ss(line);
		string number;
		int i = 0;
		while (getline(ss, number, ',')) {
			if (i == 0) positionVec.push_back(stod(number));
			else if (i == 1) xVec.push_back(stod(number));
			else if (i == 2) yVec.push_back(stod(number));
			else throw 20;
			i++;
		}
	}
	vector<vector<double> > to_return;
	to_return.push_back(positionVec); to_return.push_back(xVec); to_return.push_back(yVec);
	return to_return;
}

vector<vector<double> > extract_one_cycle(vector<double> positions, vector<double> xs, vector<double> ys) {
	/* Extract the first full cycle, defined as follows:
	  - Cycle starts at first place where x<=LENGTH_STARTING_THRESH
	  - Cycle is done starting as soon as x>=LENGTH_STARTED_THRESH
	  - Cycle is finished when x<=LENGTH_STARTING_THRESH again
	*/
	vector<double> posReturn, xReturn, yReturn;
	bool cycle_started = false;
	bool cycle_finished_starting = false;
	int n = xs.size();
	double pos, x, y;
	for (int i = 0; i < n; i++) {
		pos = positions[i]; x = xs[i]; y = ys[i];
		if ((x > LENGTH_STARTING_THRESH) & !cycle_started) {
			// Data file starts in middle of a cycle.  Ignore starting numbers
			continue;
		}
		else if ((x <= LENGTH_STARTING_THRESH) & !cycle_started) {
			// Cycle starts first time x<=LENGTH_STARTING_THRESH
			cycle_started = true;
			posReturn.push_back(pos); xReturn.push_back(x); yReturn.push_back(y);
		}
		else if ((x >= LENGTH_STARTED_THRESH) & cycle_started & !cycle_finished_starting) {
			// Cycle finished starting when x>=LENGTH_STARTED_THRESH
			cycle_finished_starting = true;
			posReturn.push_back(pos); xReturn.push_back(x); yReturn.push_back(y);
		}
		else if ((x <= LENGTH_STARTING_THRESH) & cycle_finished_starting) {
			// Cycle ends when x drop back below LENGTH_STARTING_THRESH
			posReturn.push_back(pos); xReturn.push_back(x); yReturn.push_back(y);
			break;
		}
		else {
			// Otherwise just keep these data
			posReturn.push_back(pos); xReturn.push_back(x); yReturn.push_back(y);
		}
	}
	vector<vector<double> > to_return;
	to_return.push_back(posReturn); to_return.push_back(xReturn); to_return.push_back(yReturn);
	return to_return;
}

vector<double> normalize(vector<double> inVec) {
	// Normalize a vector to range from 0.0 to 1.0
	vector<double> outVec;
	double min_val = inVec[0];
	double max_val = inVec[0];
	double current_val = 0.0;
	for (int i = 0; i < inVec.size(); i++) {
		current_val = inVec[i];
		if (current_val < min_val) min_val = current_val;
		if (current_val > max_val) max_val = current_val;
	}
	for (int i = 0; i < inVec.size(); i++)
		outVec.push_back((inVec[i] - min_val) / (max_val - min_val));
	return outVec;
}

struct FittedLine {
	double slope;
	double intercept;
	double r2;
};

FittedLine fit_a_line(vector<double> xs, vector<double> ys) {
	// Fit a line for y as a function of x, using least squares
	// It would be nice to use perpendicular offset as described at
	//   http://mathworld.wolfram.com/LeastSquaresFittingPerpendicularOffsets.html
	// but that is a pain to code up.
	double xsum = 0, x2sum = 0, y2sum = 0, ysum = 0, xysum = 0, r2 = 0;
	int n_points = xs.size();
	for (int i = 0; i < n_points; i++) {
		xsum += xs[i];
		ysum += ys[i];
		x2sum += pow(xs[i], 2);
		y2sum += pow(ys[i], 2);
		xysum += xs[i] * ys[i];
	}
	double slope = (n_points*xysum - xsum * ysum) / (n_points*x2sum - xsum * xsum);
	double intercept = (x2sum*ysum - xsum * xysum) / (x2sum*n_points - xsum * xsum);
	// Goodness of fit
	for (int i = 0; i < n_points; i++) {
		double y_pred = slope * xs[i] + intercept;
		r2 += pow(y_pred - ys[i], 2);
	}
	r2 /= n_points;
	FittedLine fit;
	fit.slope = slope; fit.intercept = intercept; fit.r2 = r2;
	return fit;
}

/*
An Line holds a collection of points and a line that has
been fitted to them.  It is NOT the same as a LineSegment.
*/
#define LARGE_NUMBER 1000 // dummy large number
class Line {
public:
	string name;
	FittedLine normal_fitted_line;
	FittedLine inverse_fitted_line;
	// Raw data
	int n_points;
	vector<double> xs, ys;
	double min_x, max_x, min_y, max_y;
	// Fitted data
	double slope, intercept, r2;
	double length;
	Line(string nm) {
		name = nm;
		n_points = 0;
		min_x = LARGE_NUMBER; max_x = -1 * LARGE_NUMBER; min_y = LARGE_NUMBER; max_y = -1 * LARGE_NUMBER;
	}
	void display() {
		// Useful in debugging.
		cout << "Name: " << name << endl
			<< "  n points " << n_points << endl
			<< "  slope " << slope << endl
			<< "  length " << length << endl
			<< "  r2 " << r2 << endl
			<< "  x range " << min_x << "  " << max_x << endl
			<< "  y range " << min_y << "  " << max_y << endl
			<< "  inv r2 " << inverse_fitted_line.r2 << endl;
	}
	void add_point(double x, double y) {
		xs.push_back(x);
		ys.push_back(y);
		n_points++;
		if (x < min_x) min_x = x;
		if (x > max_x) max_x = x;
		if (y < min_y) min_y = y;
		if (y > max_y) max_y = y;
	}
	void finish() {
		if (n_points == 0) {
			return;
		}
		normal_fitted_line = fit_a_line(xs, ys);
		inverse_fitted_line = fit_a_line(ys, xs);
		slope = normal_fitted_line.slope;
		intercept = normal_fitted_line.intercept;
		r2 = normal_fitted_line.r2;
		double x_diff_sqr = pow(xs[0] - xs[n_points - 1], 2);
		double y_diff_sqr = pow(ys[0] - ys[n_points - 1], 2);
		length = pow(x_diff_sqr + y_diff_sqr, 0.5);
	}
};

/*
Geometric line segment defined by two endpoints.
A fitted FourSidedFigure consists of 4 of these.
*/
class LineSegment {
public:
	double x1, x2, y1, y2;
	double length;
	LineSegment(double xx1, double xx2, double yy1, double yy2) {
		x1 = xx1; x2 = xx2; y1 = yy1; y2 = yy2;
		length = pow(pow(y2 - y1, 2) + pow(x2 - x1, 2), 0.5);
	}
	double dist(double x, double y) {
		// Distance from the line segment
		double dot_prod = (y2 - y1)*(y - y1) / length + (x2 - x1)*(x - x1) / length;  // direction from pt1 to pt2
		if (dot_prod < 0) {
			// (x, y) is closest to endpoint 1
			double sqrd = pow(x - x1, 2) + pow(y - y1, 2);
			return pow(sqrd, 0.5);
		}
		else if (dot_prod > 1) {
			// (x, y) is closest to endpoint 2
			double sqrd = pow(x - x2, 2) + pow(y - y2, 2);
			return pow(sqrd, 0.5);
		}
		else {
			// (x, y) is closest to the Line itself between the endpoints
			double dist_to_pt1_sqrd = pow(x - x1, 2) + pow(y - y1, 2);
			return pow(dist_to_pt1_sqrd - pow(dot_prod, 2), 0.5);
		}
	}
};

/*
A 4-sided shape fit to the data, which is ideally a trapezoid.

Note the conventions:
- Leftmost point is Vertex 1
- Vertices 2, 3 and 4 are clockwise from Vertex 1
- The line segment immediately clockwise from Vertex 1 is LineSegment 1,
  and so on clockwise

Note that I'm abusing terminology here.  Technically a geometric FourSidedFigure must
have opposite sides of equal length.  The FourSidedFigure class here can have sides
of any length.  Technically I should probably call it a Quadrilateral?
*/
class FourSidedFigure {
public:
	double x1, x2, x3, x4, y1, y2, y3, y4;
	LineSegment *s1, *s2, *s3, *s4;
	FourSidedFigure(vector<double> x_coords, vector<double> y_coords) {
		// Input: coordinates of vertices
		x1 = x_coords[0]; x2 = x_coords[1]; x3 = x_coords[2]; x4 = x_coords[3];
		y1 = y_coords[0]; y2 = y_coords[1]; y3 = y_coords[2]; y4 = y_coords[3];
		s1 = new LineSegment(x1, x2, y1, y2); s2 = new LineSegment(x2, x3, y2, y3);
		s3 = new LineSegment(x3, x4, y3, y4); s4 = new LineSegment(x4, x1, y4, y1);
	}
	FourSidedFigure(Line e1, Line e2, Line e3, Line e4) {
		// Input: an Line for te line segments of the FourSidedFigure
		x1 = x_of_intersection(e1, e4); y1 = y_of_intersection(e1, e4);
		x2 = x_of_intersection(e1, e2); y2 = y_of_intersection(e1, e2);
		x3 = x_of_intersection(e2, e3); y3 = y_of_intersection(e2, e3);
		x4 = x_of_intersection(e3, e4); y4 = y_of_intersection(e3, e4);
		s1 = new LineSegment(x1, x2, y1, y2); s2 = new LineSegment(x2, x3, y2, y3);
		s3 = new LineSegment(x3, x4, y3, y4); s4 = new LineSegment(x4, x1, y4, y1);
	}
	FourSidedFigure* rotate180deg() {
		double new_x4 = x3 - (x2 - x1);
		double new_y4 = y3 - (y2 - y1);
		double new_x2 = x3 - (x4 - x1);
		double new_y2 = y3 - (y4 - y1);
		vector<double> new_xs;
		new_xs.push_back(x1); new_xs.push_back(new_x2); new_xs.push_back(x3); new_xs.push_back(new_x4);
		vector<double> new_ys;
		new_ys.push_back(y1); new_ys.push_back(new_y2); new_ys.push_back(y3); new_ys.push_back(new_y4);
		return new FourSidedFigure(new_xs, new_ys);
	}
	void display() {
		cout << "Point1:  " << x1 << "  " << y1 << endl
			<< "Point2:  " << x2 << "  " << y2 << endl
			<< "Point3:  " << x3 << "  " << y3 << endl
			<< "Point4:  " << x4 << "  " << y4 << endl;
	}
	double dist(double x, double y) {
		// Distance from the FourSidedFigure
		double d1 = s1->dist(x, y);
		double d2 = s2->dist(x, y);
		double d3 = s3->dist(x, y);
		double d4 = s4->dist(x, y);
		if (d1 <= d2 & d1 <= d3 & d1 <= d4) return d1;
		else if (d2 <= d3 & d2 <= d4) return d2;
		else if (d3 <= d4) return d3;
		else return d4;
	}
	// Find points of intersection between the fitted Lines.
	// These intersections will be the vertices of the fitted
	// FourSidedFigure, i.e the endpoints of the 4 line segments that comprise it
	double x_of_intersection(Line eA, Line eB) {
		return (eB.intercept - eA.intercept) / (eA.slope - eB.slope);
	}
	double y_of_intersection(Line eA, Line eB) {
		double x = x_of_intersection(eA, eB);
		return eA.intercept + eA.slope*x;
	}
};

/*
To fit a FourSidedFigure to one cycle worth of data:
- Identify vertices 1 and 3 by finding the min/max x values
- The line between them is the axis of symmetry
- Identify vertices 2 and 4 as the farthest ones from the axis of symmetry
- fit a Line to the points from 1 to 2, 2 to 3, etc
- the intersections of these lines will be the coordinates of the fitted FourSidedFigure
*/
FourSidedFigure* fit_FourSidedFigure(vector<double> xs, vector<double> ys) {
	// Input: coordinates of a polygon in clockwise direction
	// Breaks it into 4 Lines
	int n = xs.size();
	// Axis of symmetry goes from point of min X to point of max X.
	// Find indices of the 4 corners
	int ind_of_corner_1 = 0;
	int ind_of_max_x = max_element(xs.begin(), xs.end()) - xs.begin();
	int ind_of_corner_3 = ind_of_max_x;
	double slope_of_symmetry_axis = (ys[ind_of_corner_3] - ys[ind_of_corner_1]) /
		(xs[ind_of_corner_3] - xs[ind_of_corner_1]);
	/*
	Calculate displacement using dot product.  Symmetry axis runs along
	vector <1, slope_of_symmetry_axis>.  Perpendicular to this is
	the vector v=<slope_of_symmetry_axis, -1>.  The "displacement"
	from the symmetry axis is the dot product of it with v.
	The axis of symmetry will in general have non-zero displacement.  The points
	whose displacements are furthest from the displacement of the line
	are vertices 2 and 3.
	*/
	vector<double> displacements_from_symmetry_axis;
	double disp_of_axis = xs[ind_of_corner_1] * slope_of_symmetry_axis - ys[ind_of_corner_1];
	for (int i = 0; i < n; i++) {
		double disp = xs[i] * slope_of_symmetry_axis - ys[i];
		displacements_from_symmetry_axis.push_back(abs(disp - disp_of_axis));
	}
	int ind_of_corner_2 = max_element(displacements_from_symmetry_axis.begin() + 1,
		displacements_from_symmetry_axis.begin() + ind_of_max_x - 1) - displacements_from_symmetry_axis.begin();
	int ind_of_corner_4 = max_element(displacements_from_symmetry_axis.begin() + ind_of_max_x + 1,
		displacements_from_symmetry_axis.end() - 1) - displacements_from_symmetry_axis.begin();
	// Create Lines, clockwise from further left point
	Line e1("l1"), e2("l2"), e3("l3"), e4("l4");
	int i = 0;
	for (i = 0; i != ind_of_corner_2; i++) e1.add_point(xs[i], ys[i]);
	for (i = ind_of_corner_2; i != ind_of_corner_3; i++) e2.add_point(xs[i], ys[i]);
	for (i = ind_of_corner_3; i != ind_of_corner_4; i++) e3.add_point(xs[i], ys[i]);
	for (i = ind_of_corner_4; i != xs.size(); i++) e4.add_point(xs[i], ys[i]);
	e1.finish(); e2.finish(); e3.finish(); e4.finish();
	//e1.display(); e2.display(); e3.display(); e4.display();
	FourSidedFigure* trap = new FourSidedFigure(e1, e2, e3, e4);
	return trap;
}

double compute_area(vector<double> xs, vector<double> ys) {
	// Input: coordinates of a polygon in clockwise direction
	// Computes area as described at:
	//   https://math.blogoverflow.com/2014/06/04/greens-theorem-and-area-of-polygons/
	double area = 0.0, avgx, dy;
	double delt;
	int i, n = xs.size();
	for (i = 0; i < n - 1; i++) {
		avgx = (xs[i + 1] + xs[i]) / 2;
		dy = (ys[i] - ys[i + 1]);
		delt = avgx * dy;
		area += delt;
	}
	delt = (xs[0] + xs[n - 1])*(ys[n - 1] - ys[0]) / 2;
	area += delt;
	return area;
}

int main(int argc, char *argv[]) {
	string fname(argv[1]);
	// Read in the file
	vector<vector<double> > raw_position_x_y = parse_file(fname);
	vector<double> raw_position = raw_position_x_y[0];
	vector<double> raw_xs = raw_position_x_y[1];
	vector<double> raw_ys = raw_position_x_y[2];

	// Extract a cycle and normalize the x/y to go between 0 and 1
	vector<vector<double> > position_x_y = extract_one_cycle(raw_position, raw_xs, raw_ys);
	vector<double> xs = normalize(position_x_y[1]);
	vector<double> ys = normalize(position_x_y[2]);
	//vector<double> xs = position_x_y[1];
	//vector<double> ys = position_x_y[2];

	int n = xs.size();
	//for (int i=0; i<n; i++) cout << xs[i] << "  " << ys[i] << endl;

	// Compute the area of the shape
	double area = compute_area(xs, ys);

	// Fit a FourSidedFigure and report how close the data is to it
	FourSidedFigure* trap = fit_FourSidedFigure(xs, ys);
	double distances_sum = 0.0;
	for (int i = 0; i < n; i++) {
		double d = trap->dist(xs[i], ys[i]);
		distances_sum += d;
	}
	double distance_from_shape = distances_sum / n;

	// Rotate the FourSidedFigure and report how close the data is to it
	FourSidedFigure* rotated_trap = trap->rotate180deg();
	distances_sum = 0.0;
	for (int i = 0; i < n; i++) {
		double d = rotated_trap->dist(xs[i], ys[i]);
		distances_sum += d;
	}
	double distance_from_rotated_shape = distances_sum / n;

	// Output results in JSON format
	cout << "{\"area\":" << area
		<< ", \"distance_from_shape\":" << distance_from_shape
		<< ", \"distance_from_rotated_shape\":" << distance_from_rotated_shape
		<< "}";

	return 0;

}
