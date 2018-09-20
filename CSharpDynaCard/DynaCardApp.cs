using System;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;

namespace DynaCard
{
    class DynaCardApp
    {
        public static void Main(string[] args)
        {
            Console.WriteLine("Hello World!");
            if (args.Length < 1)
                System.Console.WriteLine("Usage: DynaCardApp filename");
            List<(int, double, double)> values = (List<(int, double, double)>)(new DynaCardApp()).ParseFile(args[0]);
            foreach ((int pos, double len, double weight) in values) {
                System.Console.WriteLine($"{pos}, {len}, {weight}");
            }
        }

        //TODO: Create doc info
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
    }
}
