package com.organization.dynacard;

import java.util.List;

import com.organization.dynacard.IDynaCard.CardParameters;
import com.organization.dynacard.IDynaCard.CardShape;

public class DynaCardApp {

	public static void main (String[] args) {
		System.out.println("Hello World!");
		
		if (args.length < 2) {
			System.out.println("Usage: DynaCardApp filename minimumWeight");
		}
		
		List<CardParameters> params = DynaCardFactory.parseFile(args[0]);
		double minWeight = 0;
		try {
			minWeight = Double.parseDouble(args[1]);
		} catch (Exception e) {
			System.out.println("minimum weight is not a number. See usage.");
		}
		
		CardShape shape = DynaCardFactory.getCardShape(params, minWeight);
		
		System.out.format("Card shape is %s%n", shape.toString());
	}
}
