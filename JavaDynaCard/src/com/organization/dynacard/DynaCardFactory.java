package com.organization.dynacard;

import java.util.Collections;
import java.util.List;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.file.FileSystem;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;

import com.organization.dynacard.ICardEdge.Sides;
import com.organization.dynacard.IDynaCard.CardParameters;
import com.organization.dynacard.IDynaCard.CardShape;

public class DynaCardFactory {
	
	public static List<CardParameters> parseFile(String fileName) 
	{
		Path filePath = Paths.get(fileName);
		filePath = filePath.toAbsolutePath();
		
		List<CardParameters> values = new ArrayList<CardParameters>();
		if (!Files.exists(filePath))
			return values;
		
		Charset charset = Charset.forName("US-ASCII");
		try (BufferedReader reader = Files.newBufferedReader(filePath, charset)) {
			String line = null;
			while ((line = reader.readLine()) != null) {
				String trimmed = line.trim();
				if (trimmed.length() == 0 || trimmed.startsWith("#") || !trimmed.matches("^[0-9]+,[0-9]+[.]?[0-9]*,[0-9]+[.]?[0-9]*$"))
					continue;
				String[] parameters = trimmed.split(",");
				int angle = 0;
				double stroke = 0.0;
				double weight = 0.0;
				try {
					angle = Integer.parseInt(parameters[0]);
					stroke = Double.parseDouble(parameters[1]);
					weight = Double.parseDouble(parameters[2]);
				} catch (NumberFormatException e) {
					continue;
				}
				values.add(new CardParameters(angle, stroke, weight));
			}
			
		} catch (IOException e) {
			System.err.format("IOException: %s%n", e);
		} 
		
		
		return values;
	}
	
	public static CardShape getCardShape (List<CardParameters> values, double minimumAcceptableWeight) {
		
		List<Double> weights = new ArrayList<Double>();
		for (int i = 0; i < values.size(); i++) {
			weights.add(values.get(i).pumpWeight);
		}
		double maxWeight = Collections.max(weights);
		
		if (maxWeight < minimumAcceptableWeight)
			return CardShape.Flowing_Well;
		return NewCard(values).getShape();
	}
	
	public static DynaCard NewCard(List<CardParameters> values) {
		List<Integer> pos = new ArrayList<Integer>();
		List<Double> xs = new ArrayList<Double>();
		List<Double> ys = new ArrayList<Double>();
		
		for (int i = 0; i < values.size(); i++) {
			pos.add(values.get(i).anglePositoin);
			xs.add(values.get(i).strokeDisplacement);
			ys.add(values.get(i).pumpWeight);
		}
		
		return NewCard(pos, xs, ys);
	}
	
	public static DynaCard NewCard(List<Integer> pos, List<Double> xs, List<Double> ys) {
		List<ICardEdge> edges = SplitIntoEdges(pos, xs, ys);
		
		return new DynaCard(edges.get(0), edges.get(1), edges.get(2), edges.get(3));
	}
	
	public static List<ICardEdge> SplitIntoEdges (List<Integer> pos, List<Double> xs, List<Double> ys) {
		
        List<Double> nxs = Normalize(xs);
        List<Double> nys = Normalize(ys);

        // Identify indices of the corners of the trapezoid
        List<Double> direction_upper_right = new ArrayList<Double>(), 
        		direction_lower_right = new ArrayList<Double>();

        for (int i = 0; i < pos.size(); i++)
        {
            direction_upper_right.add(nxs.get(i) + 2 * nys.get(i));
            direction_lower_right.add(nxs.get(i) - 2 * nys.get(i));
        }

        double min = Collections.min(direction_upper_right);
        int min_index = direction_upper_right.indexOf(min);
        double max = Collections.max(direction_upper_right);
        int max_index = direction_upper_right.indexOf(max);

        // int lower_left_ind = min_element(direction_upper_right.begin(), direction_upper_right.end()) - direction_upper_right.begin();
        int lower_left_index = min_index;
        // int upper_right_ind = max_element(direction_upper_right.begin(), direction_upper_right.end()) - direction_upper_right.begin();
        int upper_right_index = max_index;

        min = Collections.min(direction_lower_right);
        min_index = direction_lower_right.indexOf(min);
        max = Collections.max(direction_lower_right);
        max_index = direction_lower_right.indexOf(max);

        // int upper_left_ind = min_element(direction_lower_right.begin(), direction_lower_right.end()) - direction_lower_right.begin();
        int upper_left_index = min_index;
        // int lower_right_ind = max_element(direction_lower_right.begin(), direction_lower_right.end()) - direction_lower_right.begin();
        int lower_right_ind = max_index;

        // Create edges based on those corners
        CardEdge left = new CardEdge("left", Sides.Left), 
                top = new CardEdge ("top", Sides.Top),
                right = new CardEdge ("right", Sides.Right),
                bottom = new CardEdge ("bottom", Sides.Bottom);

        // Traveling index
        int index = lower_left_index;
        max_index = pos.size();
        while (index != upper_left_index)
        {
            left.addPoint(nxs.get(index), nys.get(index));
            index = IncrementWithRollOver(index, max_index);
        }
        while (index != upper_right_index)
        {
            top.addPoint(nxs.get(index), nys.get(index));
            index = IncrementWithRollOver(index, max_index);
        }
        while (index != lower_right_ind)
        {
            right.addPoint(nxs.get(index), nys.get(index));
            index = IncrementWithRollOver(index, max_index);
        }
        while (index != lower_left_index)
        {
            bottom.addPoint(nxs.get(index), nys.get(index));
            index = IncrementWithRollOver(index, max_index);
        }

        left.Fit();
        top.Fit();
        right.Fit();
        bottom.Fit();

        List<ICardEdge> ret = new ArrayList<ICardEdge>();
        ret.add(left);
        ret.add(top);
        ret.add(right);
        ret.add(bottom);
        
        return ret;
    }

	
	public static List<CardParameters> FindStrokeCycles (List<CardParameters> values, boolean allStrokeCycles) 
	{
		// find indices of first pos=0 and second pos=0 to find the first cycle
        List<Integer> pos = new ArrayList<Integer> ();
        for (CardParameters param : values) {
        	pos.add(param.anglePositoin);
        }

        int first_zero_index = pos.indexOf(0);
        if (first_zero_index == -1)
            return values;
        
        List<Integer> secondPos = new ArrayList<>();
        for (int i = first_zero_index + 1; i < pos.size(); i ++) {
        	secondPos.add(pos.get(i));
        }
        
        int second_zero_index = pos.indexOf(0); //(0, first_zero_index + 1);
        if (second_zero_index == -1)
            second_zero_index = pos.size() - 1;
      
       
        int count = second_zero_index + 1 + first_zero_index + 1;
        
        List<CardParameters> singleCycle = new ArrayList<CardParameters>();
        for (int i = first_zero_index; i < count; i++) {
        	singleCycle.add(values.get(i));
        }
        
        // return first cycle for now.
        return singleCycle;
	}
	
	public static int IncrementWithRollOver (int i, int max) {
        i++;
        if (i == max)
            i = 0;
        return i;
    }
	
    public static List<Double> Normalize(List<Double> values) 
    {
    	double max = Collections.max(values);
    	double min = Collections.min(values);
    	
    	List<Double> normalized = new ArrayList<Double>();
    	for (Double d: values) {
    		normalized.add((d - min) / (max - min));
    	}
    	
    	return normalized;
    }
}
