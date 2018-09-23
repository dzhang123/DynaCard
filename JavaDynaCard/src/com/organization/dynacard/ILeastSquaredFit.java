package com.organization.dynacard;

import java.util.List;

public interface ILeastSquaredFit {
	LeastSquaredLine getLeastSquaredFit(List<Point> points);
}
