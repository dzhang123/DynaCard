package com.organization.dynacard;

public class DynaCard implements IDynaCard {

	private ICardEdge left, top, right, bottom;
	
	public DynaCard (ICardEdge left, ICardEdge top, ICardEdge right, ICardEdge bottom) {
		this.left = left;
		this.top = top;
		this.right = right;
		this.bottom = bottom;
	}
	
	@Override
	public CardShape getShape() {
		return guessState();
	}

	@Override
	public CardShape guessState() {
		// Full pump
        if (getLeft().IsVertical() &&
            getRight().IsVertical() &&
            getTop().IsFlat() &&
            getBottom().IsFlat())
            return CardShape.Full_Pump;
        // Tubing movement
        else if (
            getTop().IsFlat() &
            getBottom().IsFlat() &
            getLeft().IsSlopeUp() &
            getLeft().IsGoodFit() &
            getRight().IsSlopeUp() &
            getRight().IsGoodFit())
            return CardShape.Tubing_Movement;
        // Fluid pound
        else if (
            getTop().IsFlat() &
            getBottom().IsFlat() &
            getLeft().IsVertical() &
            (getBottom().getLength() < 0.8) &
            (getRight().getLeastSquaredLine().getR2() > 0.015))
            return CardShape.Fluid_Pound;
        // Gas interference
        else if (
            getTop().IsFlat() &
            getLeft().IsVertical() &
            getBottom().IsFlat() &
            getBottom().getLength() < 0.8)
            return CardShape.Gas_Interference;
        // Pump hitting
        else if (
            getLeft().IsVertical() &
            getRight().IsVertical() &
            getTop().getFirstHalf().IsFlat() &
            getBottom().getFirstHalf().IsFlat())
            return CardShape.Pump_Hitting;
        // Bent barrel
        else if (
            getLeft().IsVertical() &
            getRight().IsVertical() &
            getBottom().getLength() > 0.8 &
            getTop().getLength() > 0.8)
            return CardShape.Bent_Barrel;
        // Worn plunger
        else if (
            getBottom().IsFlat() &
            !getLeft().IsVertical() &
            !getRight().IsVertical() &
            getTop().getLength() < 0.9)
            return CardShape.Worn_Plunger;
        // Worn standing
        else if (
            getTop().IsFlat() &
            !getLeft().IsVertical() &
            !getRight().IsVertical() &
            getBottom().getLength() < 0.9)
            return CardShape.Worn_Standing;
        // Worn or split
        else if (
            getBottom().IsFlat() &
            getLeft().IsVertical())
            return CardShape.Worn_Or_Split_Barrel;
        // Fluid friction
        else if (
            getRight().IsVertical() &
            getLeft().IsVertical())
            return CardShape.Fluid_Friction;
        // Drag friction
        else if (
            getRight().getLength() > 0.7 &
            getLeft().getLength() > 0.7)
            return CardShape.Drag_Friction;
        else 
            return CardShape.Other;
    }

	@Override
	public ICardEdge getLeft() {
		return left;
	}

	@Override
	public ICardEdge getTop() {
		return top;
	}

	@Override
	public ICardEdge getRight() {
		return right;
	}

	@Override
	public ICardEdge getBottom() {
		return bottom;
	}
}
