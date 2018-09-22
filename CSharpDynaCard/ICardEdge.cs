using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;

namespace DynaCard
{
    #region Side
    public enum Sides
    {
        Left,
        Top,
        Right,
        Bottom
    }
    #endregion
    public struct Least_Squared_Line
    {
        public double Slope { get; set; }
        public double Intercept { get; set; }
        public double R2 { get; set; }
    }

    public interface ICardEdge
    {
        string Name { get; set; }
        Sides Side { get; set; }
        Least_Squared_Line LeastSquaredFitLine { get; }
        Least_Squared_Line InverseLeastSquaredFitLine { get; }
        List<(double x, double y)> Points { get; set; }
        double Length { get; }

        bool Fit();
        bool IsGoodFit();
        bool IsVertical();
        bool IsSlopeUp();
        bool IsSlopeDown();
        bool IsFlat();

        ICardEdge FirstHalf();
        ICardEdge SecondHalf();
        void AddPoint(double x, double y);
        void AddPoint(ValueTuple<double, double> p);
    }

    public interface ILeastSquaredFit
    {
        Least_Squared_Line LeastSquaredFit(IEnumerable<(double x, double y)> points);
    }

    public class CardEdge : ICardEdge, ILeastSquaredFit
    {
        public string Name { get; set; }
        public Sides Side { get; set; }
        public Least_Squared_Line LeastSquaredFitLine { get; private set; }
        public Least_Squared_Line InverseLeastSquaredFitLine { get; private set; }
        public List<(double x, double y)> Points { get; set; }
        public double Length { get; private set; }


        public bool Fit()
        {
            LeastSquaredFitLine = LeastSquaredFit(Points);
            IEnumerable<(double y, double x)> inversePoints = from point in Points select (point.y, point.x);
            InverseLeastSquaredFitLine = LeastSquaredFit(inversePoints);
            double x2 = Math.Pow(Points.First().x - Points.Last().x, 2);
            double y2 = Math.Pow(Points.First().y - Points.Last().y, 2);
            Length = Math.Pow(x2 + y2, 0.5);
            return true;
        }

        public bool IsGoodFit()
        {
            return (LeastSquaredFitLine.R2 < 0.002) || (InverseLeastSquaredFitLine.R2 < 0.002);
        }

        public bool IsVertical()
        {
            return IsGoodFit() && Math.Abs(InverseLeastSquaredFitLine.Slope) < 0.1;
        }

        public bool IsSlopeUp()
        {
            return IsGoodFit() && LeastSquaredFitLine.Slope > 0.5;
        }

        public bool IsSlopeDown()
        {
            return IsGoodFit() && LeastSquaredFitLine.Slope < -0.5;
        }

        public bool IsFlat()
        {
            return IsGoodFit() && Math.Abs(LeastSquaredFitLine.Slope) < 0.1;
        }

        public Least_Squared_Line LeastSquaredFit(IEnumerable<(double x, double y)> xyPoints)
        {
            double xsum = 0, x2sum = 0, ysum = 0, y2sum = 0, xysum = 0, r2 = 0;
            int numberOfPoints = Points.Count;

            IEnumerator<(double x, double y)> iter = xyPoints.GetEnumerator();
            while (iter.MoveNext())
            {
                xsum += iter.Current.x;
                ysum += iter.Current.y;
                x2sum += Math.Pow(iter.Current.x, 2);
                y2sum += Math.Pow(iter.Current.y, 2);
                xysum += iter.Current.x * iter.Current.y;
            }

            double slope = (numberOfPoints * xysum - xsum * ysum) / (numberOfPoints * x2sum - xsum * xsum);
            double intercept = (x2sum * ysum - xsum * xysum) / (x2sum * numberOfPoints - xsum * xsum);
            //slope = (xysum + (ysum/numberOfPoints)*xsum + (xsum/numberOfPoints)*ysum + numberOfPoints*(ysum/numberOfPoints)*(xsum/numberOfPoints));
            //intercept = (ysum/numberOfPoints) - slope*(xsum/numberOfPoints);

            // Goodness of fit
            for (int i = 0; i < numberOfPoints; i++)
            {
                double y_pred = slope * xyPoints.ToList()[i].x + intercept;
                r2 += Math.Pow(y_pred - xyPoints.ToList()[i].y, 2);
            }
            r2 /= numberOfPoints;

            return new Least_Squared_Line { Slope = slope, Intercept = intercept, R2 = r2 };
        }

        public ICardEdge FirstHalf()
        {
            ICardEdge halfEdge = 
                new CardEdge { Name = "First half of " + this.Name, Side = this.Side, Points = new List<(double x, double y)>()};
            List<(double x, double y)> halfPoints = new List<(double x, double y)>();
            for (int i = 0; i < Points.Count; i++)
            {
                double x2 = Math.Pow(Points[0].x - Points[i].x, 2);
                double y2 = Math.Pow(Points[0].y - Points[i].y, 2);
                double distFromStart = Math.Pow(x2 + y2, 0.5);
                if (distFromStart <= 0.5 * this.Length)
                {
                    halfPoints.Add(Points[i]);
                }
                else
                    break;
            }
            halfEdge.Points = halfPoints;
            halfEdge.Fit();

            return halfEdge;
        }

        public ICardEdge SecondHalf()
        {
            ICardEdge halfEdge = new CardEdge { Name = "Second half of " + this.Name, Side = this.Side, Points = new List<(double x, double y)>() };
            List<(double x, double y)> halfPoints = new List<(double x, double y)>();
            for (int i = Points.Count - 1; i >= 0; i--)
            {
                double x2 = Math.Pow(Points.Last().x - Points[i].x, 2);
                double y2 = Math.Pow(Points.Last().y - Points[i].y, 2);
                double distFromEnd = Math.Pow(x2 + y2, 0.5);
                if (distFromEnd <= 0.5 * this.Length)
                {
                    halfPoints.Add(Points[i]);
                }
                else
                    break;
            }
            halfPoints.Reverse();

            halfEdge.Points = halfPoints;
            halfEdge.Fit();

            return halfEdge;
        }
    
        public void AddPoint(double x, double y) {
            Points.Add(new ValueTuple<double, double>(x, y));
        }
        public void AddPoint(ValueTuple<double, double> p) 
        {
            Points.Add(p);
        }
    }
}
