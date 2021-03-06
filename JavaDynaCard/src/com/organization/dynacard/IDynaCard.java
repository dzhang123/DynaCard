package com.organization.dynacard;

public interface IDynaCard {
	public enum CardShape
    {
        Full_Pump,
        Tubing_Movement,
        Fluid_Pound,
        Gas_Interference,
        Flowing_Well,
        Pump_Hitting,
        Bent_Barrel,
        Worn_Plunger,
        Worn_Standing,
        Worn_Or_Split_Barrel,
        Fluid_Friction,
        Drag_Friction,
        Other
    }
	public CardShape getShape();
	
	CardShape guessState();
	
	ICardEdge getLeft();
	ICardEdge getTop();
	ICardEdge getRight();
	ICardEdge getBottom();
	
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


