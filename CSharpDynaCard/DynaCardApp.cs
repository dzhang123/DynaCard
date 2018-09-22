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
            if (args.Length < 2)
                System.Console.WriteLine("Usage: DynaCardApp filename minimumWeight");

            List<(int, double, double weight)> values = DynaCardFactory.ParseFile(args[0]).ToList();

            double minWeight = 0;
            double.TryParse(args[1], out minWeight);
            CardShape shape = DynaCardFactory.GetCardShape(values, minWeight);
            System.Console.WriteLine($"card shape is {shape.ToString()}");
        }
    }
}
