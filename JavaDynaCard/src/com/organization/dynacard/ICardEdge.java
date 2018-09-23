package com.organization.dynacard;
import java.util.List;

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
	public void addPoint(double x, double y);
	public void addPoint(Point p);
	public List<Point> getPoints();
	public double getLength();
	public LeastSquaredLine getLeastSquaredLine();
	public LeastSquaredLine getInverseLeastSquaredLine();
	
	public static class CardParameters {
		public int anglePositoin; //degree
		public double strokeDisplacement;
		public double pumpWeight;
		
		public CardParameters(int angle, double stroke, double weight) {
			anglePositoin = angle;
			strokeDisplacement = stroke;
			pumpWeight = weight;
		}
	}
}
