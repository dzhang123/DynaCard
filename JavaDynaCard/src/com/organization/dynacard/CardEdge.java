package com.organization.dynacard;

import java.util.List;

import com.organization.dynacard.ICardEdge.Sides;

public class CardEdge implements ICardEdge, ILeastSquaredFit {
	private String name;
	private Sides side;
	private LeastSquaredLine leastSquaredLine;
	private LeastSquaredLine inverseLeastSquaredLine;
	private List<Point> points;
	private int length;
	
	@Override
	public LeastSquaredLine getLeastSquaredFit(List<Point> points) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public boolean Fit() {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public boolean IsGoodFit() {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public boolean IsVertical() {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public boolean IsSlopeUp() {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public boolean IsSlopeDown() {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public boolean IsFlat() {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public ICardEdge getFirstHalf() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public ICardEdge getSecondHalf() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void AddPoint(double x, double y) {
		// TODO Auto-generated method stub

	}

	@Override
	public void AddPoint(Point p) {
		// TODO Auto-generated method stub

	}

}
