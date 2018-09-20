using System;
namespace DynaCard
{
    public enum Orientation {
        Vertical,
        Horizontal
    }

    public interface IOriented {
        Orientation GetOrientation();
    }
    public interface ICardEdge
    {
        // TODO: need to make this a lambda function or at least a lambda like 
        // parameter.
        void Fit();
    }

    public abstract class CardEdge : ICardEdge {
        public abstract void Fit();
    }

    public class CardLeftEdge : ICardEdge, IOriented
    {
        public void Fit()
        {
            throw new NotImplementedException();
        }

        public Orientation GetOrientation()
        {
            return Orientation.Vertical;
        }
    }

    public class CardTopEdge : ICardEdge, IOriented
    {
        public void Fit()
        {
            throw new NotImplementedException();
        }

        public Orientation GetOrientation()
        {
            return Orientation.Horizontal;
        }
    }

    public class CardRightEdge : ICardEdge, IOriented
    {
        public void Fit()
        {
            throw new NotImplementedException();
        }

        public Orientation GetOrientation()
        {
            return Orientation.Vertical;
        }
    }

    public class CardBottomEdge : ICardEdge, IOriented
    {
        public void Fit()
        {
            throw new NotImplementedException();
        }

        public Orientation GetOrientation()
        {
            return Orientation.Horizontal;
        }
    }
}
