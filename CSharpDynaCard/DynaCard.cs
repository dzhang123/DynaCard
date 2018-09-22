using System;
namespace DynaCard
{
    public enum CardType {
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
        public CardType Type { get; set; } 

        public ICardEdge LeftEdge { get; set; }
        public ICardEdge TopEdge { get; set; }
        public ICardEdge RightEdge { get; set; }
        public ICardEdge BottomEdge { get; set; }

        public CardType Fit() {
            return CardType.Other;
        }
    }
}
