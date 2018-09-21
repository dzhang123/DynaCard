using System;
using System.Collections.Generic;

namespace DynaCard
{
    public enum Side {
        Left,
        Top,
        Right,
        Bottom
    }


    public struct Least_Squared_Line
    {
        double slop;
        double intercept;
        double r2;
    }

    public class Edge
    {
        public string Name { get;set;}
        public Side Position { get; set; }
        public Least_Squared_Line LeastSquaredFit { get; private set; }
        public Least_Squared_Line InverseLeastSquaredFit { get; private set; }
        public List<(double x, double y)> points { get; set; }

        public bool Fit()
        {

        }
    }



    public interface ICardEdge
    {
        void Fit();
        Side GetSide();
    }

    public class CardEdge : ICardEdge {
        public void Fit();
        public Side GetSide();
    }

    public class CardLeftEdge : ICardEdge 
    {
        public void Fit()
        {
            throw new NotImplementedException();
        }

    }

    public class CardTopEdge : ICardEdge
    {
        public void Fit()
        {
            throw new NotImplementedException();
        }

    }

    public class CardRightEdge : ICardEdge
    {
        public void Fit()
        {
            throw new NotImplementedException();
        }

    }

    public class CardBottomEdge : ICardEdge
    {
        public void Fit()
        {
            throw new NotImplementedException();
        }
    }
}
