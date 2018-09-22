using System;
namespace DynaCard
{
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

    public class DynaCard
    {
        public CardShape Shape => GuessState();

        public ICardEdge Left { get; set; }
        public ICardEdge Top { get; set; }
        public ICardEdge Right { get; set; }
        public ICardEdge Bottom { get; set; }

        protected CardShape GuessState() 
        {
            // Full pump
            if (Left.IsVertical() &
                Right.IsVertical() &
                Top.IsFlat() &
                Bottom.IsFlat())
                return CardShape.Full_Pump;
            // Tubing movement
            else if (
                Top.IsFlat() &
                Bottom.IsFlat() &
                Left.IsSlopeUp() &
                Left.IsGoodFit() &
                Right.IsSlopeUp() &
                Right.IsGoodFit())
                return CardShape.Tubing_Movement;
            // Fluid pound
            else if (
                Top.IsFlat() &
                Bottom.IsFlat() &
                Left.IsVertical() &
                (Bottom.Length < 0.8) &
                (Right.LeastSquaredFitLine.R2 > 0.015))
                return CardShape.Fluid_Pound;
            // Gas interference
            else if (
                Top.IsFlat() &
                Left.IsVertical() &
                Bottom.IsFlat() &
                Bottom.Length < 0.8)
                return CardShape.Gas_Interference;
            // Pump hitting
            else if (
                Left.IsVertical() &
                Right.IsVertical() &
                Top.FirstHalf().IsFlat() &
                Bottom.FirstHalf().IsFlat())
                return CardShape.Pump_Hitting;
            // Bent barrel
            else if (
                Left.IsVertical() &
                Right.IsVertical() &
                Bottom.Length > 0.8 &
                Top.Length > 0.8)
                return CardShape.Bent_Barrel;
            // Worn plunger
            else if (
                Bottom.IsFlat() &
                !Left.IsVertical() &
                !Right.IsVertical() &
                Top.Length < 0.9)
                return CardShape.Worn_Plunger;
            // Worn standing
            else if (
                Top.IsFlat() &
                !Left.IsVertical() &
                !Right.IsVertical() &
                Bottom.Length < 0.9)
                return CardShape.Worn_Standing;
            // Worn or split
            else if (
                Bottom.IsFlat() &
                Left.IsVertical())
                return CardShape.Worn_Or_Split_Barrel;
            // Fluid friction
            else if (
                Right.IsVertical() &
                Left.IsVertical())
                return CardShape.Fluid_Friction;
            // Drag friction
            else if (
                Right.Length > 0.7 &
                Left.Length > 0.7)
                return CardShape.Drag_Friction;
            else 
                return CardShape.Other;
        }
    }
}
