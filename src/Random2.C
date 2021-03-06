// 
//	CONETSIM - Random2.C
//
//
//	Copyright (C) 2014	Suhan Ree
//
//	This program is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//	
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//	
//	You should have received a copy of the GNU General Public License
//	along with this program.  If not, see <http://www.gnu.org/licenses/>.
//	
//	Author: suhanree@yahoo.com (Suhan Ree)

#include "Random2.h"
#include "Errors.h"
#include "Utilities.h"
#include <vector>
#include <cmath>

namespace conet {

using namespace std;

// Constants for the generator.
static const unsigned long a=16807, m=2147483647;
static const unsigned long r=2836, q=127773;
static const unsigned long mask=123459876;

long Random2::draw() {
	unsigned long t1, t2;

	// When the seed (randx) becomes 0, it will produce 0 from then on,
	// so randx will be replaced by mask whenever randx becomes 0.
	if (randx==0) randx=mask;	
	t1=a*(randx%q);
	t2=r*(randx/q);
	randx=(t1>=t2 ? t1-t2 : m-t2+t1);
	return long(randx);
};

long Random2::ndraw(const vector<double> &pdf) {
	long size=pdf.size();
	if (size<=1) return 0; // Trivial cases.
	vector<double> cdf(size);
	find_cdf(pdf,cdf);
	// Drawing a number with the cumulative distribution.
	long min=0, max=size;
	long div2=size/2;
	// Find the right bin. (fdraw() will give a double between 0 and 1)
	double y=fdraw();
	while (min!=div2) {
		if(cdf[div2]<y) min=div2;
		else 	max=div2;
		div2=min+(max-min)/2;
	};
	return min;
};

long Random2::ndraw(const vector<long> &pdf) {
	long size=pdf.size();
	if (size<=1) return 0; // Trivial cases.
	vector<double>  cdf(size);
	find_cdf(pdf,cdf);
	// Drawing a number with the cumulative distribution.
	long min=0, max=size;
	long div2=size/2;
	// Find the right bin. (fdraw() will give a double between 0 and 1)
	double y=fdraw();
	while (min!=div2) {
		if(cdf[div2]<y) min=div2;
		else 	max=div2;
		div2=min+(max-min)/2;
	};
	return min;
};

// Draw a double with the gaussian dist. with average and standard deviation.
pair<double,double>  Random2::gaussian_draw(double ave, double sd) {
	double fac,rsq,v1,v2;
	do {
		v1=fdraw(-1.0,1.0);
		v2=fdraw(-1.0,1.0);
		rsq=v1*v1+v2*v2;
	} while (rsq >= 1.0 || rsq==0.0);
	fac=sd*sqrt(-2.0*log(rsq)/rsq);
	pair<double,double> temp(v1*fac+ave,v2*fac+ave);
	return temp;
};

long Random2L::max_ran=400000000;

long Random2L::draw() {
	if (++rn_cnt>max_ran) {
		MAX_RN_REACHED m;
		throw m;
	};

	unsigned long t1, t2;

	// When the seed (randx) becomes 0, it will produce 0 from then on,
	// so randx will be replaced by mask whenever randx becomes 0.
	if (randx==0) randx=mask;	
	t1=a*(randx%q);
	t2=r*(randx/q);
	
	randx=(t1>=t2 ? t1-t2 : m-t2+t1);
	return long(randx);
};

long Random2L::ndraw(const vector<double> &pdf) {
	long size=pdf.size();
	if (size<=1) return 0; // Trivial cases.
	vector<double> cdf(size);
	find_cdf(pdf,cdf);
	// Drawing a number with the cumulative distribution.
	long min=0, max=size;
	long div2=size/2;
	// Find the right bin. (fdraw() will give a double between 0 and 1)
	double y=fdraw();
	while (min!=div2) {
		if(cdf[div2]<y) min=div2;
		else 	max=div2;
		div2=min+(max-min)/2;
	};
	return min;
};

long Random2L::ndraw(const vector<long> &pdf) {
	long size=pdf.size();
	if (size<=1) return 0; // Trivial cases.
	vector<double>  cdf(size);
	find_cdf(pdf,cdf);
	// Drawing a number with the cumulative distribution.
	long min=0, max=size;
	long div2=size/2;
	// Find the right bin. (fdraw() will give a double between 0 and 1)
	double y=fdraw();
	while (min!=div2) {
		if(cdf[div2]<y) min=div2;
		else 	max=div2;
		div2=min+(max-min)/2;
	};
	return min;
};

// Draw a double with the gaussian dist. with average and standard deviation.
pair<double,double>  Random2L::gaussian_draw(double ave, double sd) {
	double fac,rsq,v1,v2;
	do {
		v1=fdraw(-1.0,1.0);
		v2=fdraw(-1.0,1.0);
		rsq=v1*v1+v2*v2;
	} while (rsq >= 1.0 || rsq==0.0);
	fac=sd*sqrt(-2.0*log(rsq)/rsq);
	pair<double,double> temp(v1*fac+ave,v2*fac+ave);
	return temp;
};

};
