package com.organization.dynacard;

public class LeastSquaredLine {
	private double slope;
	private double intercept;
	private double r2;
	
	public double getSlope() {
		return this.slope;
	}
	public void setSlope (double slope) {
		this.slope = slope;
	}
	
	public double getIntercept () {
		return this.intercept;
	}
	public void setIntercept (double intercept) {
		this.intercept = intercept;
	}
	
	public double getR2 () {
		return r2;
	}
	public void setR2 (double r2) {
		this.r2 = r2;
	}
}

