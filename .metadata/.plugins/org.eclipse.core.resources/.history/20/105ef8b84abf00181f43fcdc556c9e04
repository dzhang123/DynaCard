package com.organization.dynacard;

public interface ICardEdge {
	public enum Sides {
		Left,
		Top,
		Right,
		Bottom
	}

	public boolean Fit();
	public boolean IsGoodFit();
	public boolean IsVertical();
	public boolean IsSlopeUp();
	public boolean IsSlopeDown();
	public boolean IsFlat();
	
	public ICardEdge getFirstHalf();
	public ICardEdge getSecondHalf();
	public void AddPoint(double x, double y);
	public void AddPoint(Point p);
	
}
