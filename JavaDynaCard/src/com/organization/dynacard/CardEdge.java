package com.organization.dynacard;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;

public class CardEdge implements ICardEdge, ILeastSquaredFit {
	private String name;
	private Sides side;
	private LeastSquaredLine leastSquaredLine;
	private LeastSquaredLine inverseLeastSquaredLine;
	private List<Point> points = new ArrayList<Point>();
	private double length;
	
	public CardEdge (String name, Sides side) {
		this.name = name;
		this.side = side;
	}
	public double getLength() {
		return length;
	}
	public Sides getSide () {
		return side;
	}
	@Override
	public LeastSquaredLine getLeastSquaredFit(List<Point> points) {
		return leastSquaredLine;
	}

	@Override
	public boolean Fit() {
		leastSquaredLine = LeastSquaredFit(points);
        
        List<Point> inversePoints = new ArrayList<Point>(points);
        Collections.reverse(inversePoints);
        inverseLeastSquaredLine = LeastSquaredFit(inversePoints);
        
        int count = points.size();
        
        double x2 = Math.pow(points.get(0).x - points.get(count - 1).x, 2);
        double y2 = Math.pow(points.get(0).y - points.get(count - 1).y, 2);
        length = Math.pow(x2 + y2, 0.5);
        return true;
	}

    public LeastSquaredLine LeastSquaredFit(List<Point> xyPoints)
    {
        double xsum = 0, x2sum = 0, ysum = 0, y2sum = 0, xysum = 0, r2 = 0;
        int numberOfPoints = points.size();

        Iterator<Point> iter = xyPoints.iterator();
        while (iter.hasNext())
        {
        	Point next = iter.next();
            xsum += next.x;
            ysum += next.y;
            x2sum += Math.pow(next.x, 2);
            y2sum += Math.pow(next.y, 2);
            xysum += next.x * next.y;
        }

        double slope = (numberOfPoints * xysum - xsum * ysum) / (numberOfPoints * x2sum - xsum * xsum);
        double intercept = (x2sum * ysum - xsum * xysum) / (x2sum * numberOfPoints - xsum * xsum);
        //slope = (xysum + (ysum/numberOfPoints)*xsum + (xsum/numberOfPoints)*ysum + numberOfPoints*(ysum/numberOfPoints)*(xsum/numberOfPoints));
        //intercept = (ysum/numberOfPoints) - slope*(xsum/numberOfPoints);

        // Goodness of fit
        for (int i = 0; i < numberOfPoints; i++)
        {
            double y_pred = slope * xyPoints.get(i).x + intercept;
            r2 += Math.pow(y_pred - xyPoints.get(i).y, 2);
        }
        r2 /= numberOfPoints;

        LeastSquaredLine lsl = new LeastSquaredLine();
        lsl.setSlope(slope);
        lsl.setIntercept(intercept);
        lsl.setR2(r2);
        
        return lsl;
    }
    
	@Override
	public boolean IsGoodFit() {
		return this.leastSquaredLine.getR2() < 0.002 || this.inverseLeastSquaredLine.getR2() < 0.002;
	}

	@Override
	public boolean IsVertical() {
		return IsGoodFit() && Math.abs(inverseLeastSquaredLine.getSlope()) < 0.1;
	}

	@Override
	public boolean IsSlopeUp() {
		return IsGoodFit() && leastSquaredLine.getSlope() > 0.5;
	}

	@Override
	public boolean IsSlopeDown() {
		return IsGoodFit() && leastSquaredLine.getSlope() < -0.5;
	}

	@Override
	public boolean IsFlat() {
		return IsGoodFit() && Math.abs(leastSquaredLine.getSlope()) < 0.1;
	}

	@Override
	public ICardEdge getFirstHalf() {
		ICardEdge halfEdge = new CardEdge("First half of " + name, this.side);
		
        
        for (int i = 0; i < this.getLength(); i++)
        {
            double x2 = Math.pow(points.get(0).x - points.get(i).x, 2);
            double y2 = Math.pow(points.get(0).y - points.get(i).y, 2);
            double distFromStart = Math.pow(x2 + y2, 0.5);
            if (distFromStart <= 0.5 * this.getLength())
            {
                halfEdge.addPoint(points.get(i));
            }
            else
                break;
        }
        halfEdge.Fit();

        return halfEdge;
	}

	@Override
	public ICardEdge getSecondHalf() {
		ICardEdge halfEdge = new CardEdge ("Second half of " + name, side);
		
        for (int i = points.size() - 1; i >= 0; i--)
        {
            double x2 = Math.pow(points.get(points.size() - 1).x - points.get(i).x, 2);
            double y2 = Math.pow(points.get(points.size() - 1).y - points.get(i).y, 2);
            double distFromEnd = Math.pow(x2 + y2, 0.5);
            if (distFromEnd <= 0.5 * getLength())
            {
                halfEdge.addPoint(points.get(i));
            }
            else
                break;
        }
        Collections.reverse(halfEdge.getPoints());
        halfEdge.Fit();

        return halfEdge;
	}

	@Override
	public void addPoint(double x, double y) {
		points.add(new Point(x, y));
	}

	@Override
	public void addPoint(Point p) {
		points.add(p);
	}
	
	@Override
	public List<Point> getPoints() {
		return points;
	}
	@Override
	public LeastSquaredLine getLeastSquaredLine() {
		return leastSquaredLine;
	}
	@Override
	public LeastSquaredLine getInverseLeastSquaredLine() {
		return inverseLeastSquaredLine;
	}
}
