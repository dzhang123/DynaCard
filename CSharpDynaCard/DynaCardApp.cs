using System;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;
using System.Linq;


namespace DynaCard
{
    class DynaCardApp
    {
        public static void Main(string[] args)
        {
            Console.WriteLine("Hello World!");
            if (args.Length < 1)
                System.Console.WriteLine("Usage: DynaCardApp filename");
            List<(int, double, double weight)> values = (List<(int, double, double)>)(new DynaCardApp()).ParseFile(args[0]);


            List<(int pos, double len, double w)> first = FindStrokeCycles(values);

            foreach ((int pos, double len, double weight) in first) {
                System.Console.WriteLine($"{pos}, {len}, {weight}");
            }

            System.Console.WriteLine();
            System.Console.WriteLine();
            System.Console.WriteLine();




            // test normalize
            List<double> weights = (from w in first
                                    select w.weight).ToList();
            List<double> nweights = Normalize(weights);
            foreach(var nw in nweights)
            {
                System.Console.WriteLine(nw);
            }
        }

        // Parse the file and extract numerical values in the format of "position, stroke length, weight"
        // return a list of tuple
        public IEnumerable<(int Position, double Length, double Weight)> ParseFile(string filename)
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
            // 1. trim the values to eliminate incomplete cycles at both ends
            IEnumerable<(int position, double, double)> cycles = values.SkipWhile(v => v.position != 0);
            
            cycles = cycles.Reverse().SkipWhile(v => v.position != 360).Reverse();
            if (cycles.Count() == 0)
                return cycles.ToList();

            List<int> positions = (from cycle in cycles select cycle.position).ToList();
            
            int cycleStartIndex = positions.IndexOf(0);
            int cycleEndIndex = positions.IndexOf(360);
            int count = cycleEndIndex - cycleStartIndex + 1;
            List<(int, double, double)> singleCycle = cycles.Take(count).ToList();
            // return first cycle for now.
            return singleCycle;
        }
    }
}
