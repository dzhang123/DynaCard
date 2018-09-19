using System;
namespace DynaCard
{
    public enum CardType {
        Full_Pump,
        Tubing_Movement,
        Fluid_Pound,
        Gas_Interference,
        Other
    }

    public class DynaCard
    {
        //public CardType Type { get; set; } 

        public ICardEdge Edge { get; set; }

        public CardType Fit() {
            return CardType.Other;
        }

    }
}
