using System;
using System.Collections.Generic;
using System.IO;

namespace DynaCard
{
    class DynaCardApp
    {
        public static void Main(string[] args)
        {
            Console.WriteLine("Hello World!");
            if (args.Length < 1)
                System.Console.WriteLine("Usage: DynaCardApp filename")
        }


        //TODO: Create doc info
        // position, length, weight
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
                    string[] vs = line.Split(',');
                    values.Add((Int32.Parse(vs[0]), Double.Parse(vs[1]), Double.Parse(vs[2])));
                }
            }
            return values;
        }
    }
}
