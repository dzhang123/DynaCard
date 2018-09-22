using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;

namespace DynaCard
{
    public class DynaCardFactory
    {
        private DynaCardFactory()
        {
        }

        public static CardShape GetCardShape (IEnumerable<(int angle, double stroke, double weight)> values, double minimumAcceptableWeight) {

            double maxWeigth = (from value in values select value.weight).Max();
            if (maxWeigth < minimumAcceptableWeight)
                return CardShape.Flowing_Well;
            return NewCard(values).Shape;
        }

        // Parse the file and extract numerical values in the format of "position, stroke length, weight"
        // return a list of tuple
        public static IEnumerable<(int Position, double Length, double Weight)> ParseFile(string filename)
        {
            List<(int, double, double)> values = new List<(int, double, double)>();

            using (var reader = System.IO.File.OpenText(filename))
            {
                while (!reader.EndOfStream)
                {
                    string line = reader.ReadLine();
                    if (line.Trim().Length == 0 | line.Trim().StartsWith("#"))
                        continue;
                    //if (!Regex.IsMath(line.Trim(), @"^\d+$"))
                    if (!Regex.IsMatch(line.Trim(), @"^\d"))
                    {
                        continue;
                    }
                    string[] vs = line.Split(',');
                    values.Add((Int32.Parse(vs[0]), Double.Parse(vs[1]), Double.Parse(vs[2])));
                }
            }
            return values;
        }

        // TODO
        public static List<double> Normalize(List<double> values)
        {
            #region
            //// do linq later
            //var min = Double.MaxValue;
            //var max = Double.MinValue;
            //foreach (var v in values) {
            //    if (v < min)
            //        min = v;
            //    if (v > max)
            //        max = v;
            //};
            //List<double> normalized = new List<double>();
            //var iter = values.GetEnumerator();
            //while (iter.MoveNext())
            //{
            //    normalized.Add((iter.Current - min) / (max - min));
            //}
            //return normalized;
            #endregion

            double max = values.Max();
            double min = values.Min();

            IEnumerable<double> normalized =
                                        from value in values
                                        let normalized_value = (value - min) / (max - min)
                                        select normalized_value;
            return normalized.ToList();
        }

        // find stroke cycle either the first one or the entire "cycle"s, 
        // in the latter case cycles will be averaged into a single cycle and return to the caller
        public static List<(int position, double length, double weight)>
            FindStrokeCycles(List<(int position, double length, double weight)> values, bool allStrokeCycles = false)
        {
            // find indices of first pos=0 and second pos=0 to find the first cycle

            List<int> positions = (from cycle in values select cycle.position).ToList();

            int first_zero_index = positions.IndexOf(0);
            if (first_zero_index == -1)
                return values;
            int second_zero_index = positions.IndexOf(0, first_zero_index + 1);
            if (second_zero_index == -1)
                second_zero_index = positions.Count() - 1;
            int count = second_zero_index - first_zero_index + 1;
            List<(int, double, double)> singleCycle = values.Skip(first_zero_index).Take(count).ToList();
            // return first cycle for now.
            return singleCycle;
        }

        public static DynaCard NewCard(IEnumerable<(int anglePosition, double strokeDisplacement, double pumpWeight)> points) {

            List<int> pos = (from point in points select point.anglePosition).ToList();
            List<double> xs = (from point in points select point.strokeDisplacement).ToList();
            List<double> ys = (from point in points select point.pumpWeight).ToList();

            return NewCard(pos, xs, ys);
        }

        public static DynaCard NewCard(List<int> anglePositions, List<double> strokeDisplacements, List<double> pumpWeights) {

            (CardEdge left, CardEdge top, CardEdge right, CardEdge bottom) sides =
                SplitIntoEdges(anglePositions, strokeDisplacements, pumpWeights);

            return new DynaCard { Left = sides.left, Top = sides.top, Right = sides.right, Bottom = sides.bottom };
        }

        public static (CardEdge left, CardEdge top, CardEdge right, CardEdge bottom) 
            SplitIntoEdges(IEnumerable<(int anglePosition, double strokeDisplacement, double pumpWeight)> points) 
        {
            List<int> pos = (from point in points select point.anglePosition).ToList();
            List<double> xs = (from point in points select point.strokeDisplacement).ToList();
            List<double> ys = (from point in points select point.pumpWeight).ToList();

            return SplitIntoEdges(pos, xs, ys);
        }
        public static (CardEdge left, CardEdge top, CardEdge right, CardEdge bottom) 
            SplitIntoEdges(IEnumerable<int> anglePositions, IEnumerable<double> strokeDisplacements, IEnumerable<double> pumpWeights) {

            // temperary
            List<int> pos = anglePositions.ToList();
            List<double> xs = strokeDisplacements.ToList();
            List<double> ys = pumpWeights.ToList();

            xs = Normalize(xs);
            ys = Normalize(ys);

            // Identify indices of the corners of the trapezoid
            List<double> direction_upper_right = new List<double>(), direction_lower_right = new List<double>();

            for (int i = 0; i < pos.Count; i++)
            {
                direction_upper_right.Add(xs[i] + 2 * ys[i]);
                direction_lower_right.Add(xs[i] - 2 * ys[i]);
            }

            double min = direction_upper_right.Min();
            int min_index = direction_upper_right.IndexOf(min);
            double max = direction_upper_right.Max();
            int max_index = direction_upper_right.IndexOf(max);

            // int lower_left_ind = min_element(direction_upper_right.begin(), direction_upper_right.end()) - direction_upper_right.begin();
            int lower_left_index = min_index;
            // int upper_right_ind = max_element(direction_upper_right.begin(), direction_upper_right.end()) - direction_upper_right.begin();
            int upper_right_index = max_index;

            min = direction_lower_right.Min();
            min_index = direction_lower_right.IndexOf(min);
            max = direction_lower_right.Max();
            max_index = direction_lower_right.IndexOf(max);

            // int upper_left_ind = min_element(direction_lower_right.begin(), direction_lower_right.end()) - direction_lower_right.begin();
            int upper_left_index = min_index;
            // int lower_right_ind = max_element(direction_lower_right.begin(), direction_lower_right.end()) - direction_lower_right.begin();
            int lower_right_ind = max_index;

            // Create edges based on those corners
            CardEdge left = new CardEdge { Name = "left", Side = Sides.Left, Points = new List<(double x, double y)>()},
                    top = new CardEdge { Name = "top", Side = Sides.Top, Points = new List<(double x, double y)>() },
                    right = new CardEdge { Name = "right", Side = Sides.Right, Points = new List<(double x, double y)>()},
                    bottom = new CardEdge { Name = "bottom", Side = Sides.Bottom, Points = new List<(double x, double y)>()};

            // Traveling index
            int index = lower_left_index;
            max_index = pos.Count;
            while (index != upper_left_index)
            {
                left.AddPoint(xs[index], ys[index]);
                index = IncrementWithRollOver(index, max_index);
            }
            while (index != upper_right_index)
            {
                top.AddPoint(xs[index], ys[index]);
                index = IncrementWithRollOver(index, max_index);
            }
            while (index != lower_right_ind)
            {
                right.AddPoint(xs[index], ys[index]);
                index = IncrementWithRollOver(index, max_index);
            }
            while (index != lower_left_index)
            {
                bottom.AddPoint(xs[index], ys[index]);
                index = IncrementWithRollOver(index, max_index);
            }

            left.Fit();
            top.Fit();
            right.Fit();
            bottom.Fit();

            return (left, top, right, bottom);
        }

        public static int IncrementWithRollOver (int i, int max) {
            i++;
            if (i == max)
                i = 0;
            return i;
        }
    }
}
