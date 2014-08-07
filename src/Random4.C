// 
//	CONETSIM - Random4.C
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

#include "Random4.h"
#include "Errors.h"
#include "Utilities.h"
#include <vector>
#include <cmath>

namespace conet {

using namespace std;

// Constants for the generator.
static const unsigned long m1=2147483563, m2=2147483399;
static const unsigned long a1=40014, a2=40692;
static const unsigned long q1=53668, q2=52774;
static const unsigned long r1=12211, r2=3791;
// Number of shuffling bins.
static const int nb=32;
static const unsigned long nb_unsigned=32;

// Constructor.
Random4::Random4(unsigned long s)
		: Random(s&0x7fffffff), v(nb) {
	if (randx==0 || randx>2147383562) randx=1;
	randx2=randx;
	for (int i=nb+7;i>=0;i--) {
		unsigned long t1, t2;
		t1=a1*(randx%q1);
		t2=r1*(randx/q1);
		randx=(t1>=t2 ? t1-t2 : m1-t2+t1);
		if (i<nb) v[i]=randx;
	};
	y=v[0];
};
	
// Provides a seed.
void Random4::seed(const unsigned long &s) {
	randx=s&0x7fffffff;
	if (randx==0 || randx>2147383562) randx=1;
	randx2=randx;
	for (int i=nb+7;i>=0;i--) {
		unsigned long t1, t2;
		t1=a1*(randx%q1);
		t2=r1*(randx/q1);
		randx=(t1>=t2 ? t1-t2 : m1-t2+t1);
		if (i<nb) v[i]=randx;
	};
	y=v[0];
};	

// Draw function.
long Random4::draw() {
	unsigned long t1, t2;
	int i;
	t1=a1*(randx%q1);
	t2=r1*(randx/q1);
	randx=(t1>=t2 ? t1-t2 : m1-t2+t1);
	t1=a2*(randx2%q2);
	t2=r2*(randx2/q2);
	randx2=(t1>=t2 ? t1-t2 : m2-t2+t1);
	// Produces a number in the range 0 to nb-1.
	i=(int) y/((m1-1)/nb_unsigned +1);	
	y=(v[i]>randx2 ? v[i]-randx2 : m1-1-(randx2-v[i]));
	v[i]=randx;
	return y; // y is supposed to be 0<y<m1
};

long Random4::ndraw(const vector<double> &pdf) {
	long size=pdf.size();
	if (size<=1) return 0; // Trivial cases.
	vector<double> cdf(size);
	find_cdf(pdf,cdf);
	// Drawing a number with the cumulative distribution.
	long min=0, max=size;
	long div2=size/2;
	// Find the right bin. (fdraw() will give a double between 0 and 1)
	double yy=fdraw();
	while (min!=div2) {
		if(cdf[div2]<yy) min=div2;
		else 	max=div2;
		div2=min+(max-min)/2;
	};
	return min;
};

long Random4::ndraw(const vector<long> &pdf) {
	long size=pdf.size();
	if (size<=1) return 0; // Trivial cases.
	vector<double>  cdf(size);
	find_cdf(pdf,cdf);
	// Drawing a number with the cumulative distribution.
	long min=0, max=size;
	long div2=size/2;
	// Find the right bin. (fdraw() will give a double between 0 and 1)
	double yy=fdraw();
	while (min!=div2) {
		if(cdf[div2]<yy) min=div2;
		else 	max=div2;
		div2=min+(max-min)/2;
	};
	return min;
};

// Draw a double with the gaussian dist. with average and standard deviation.
pair<double,double>  Random4::gaussian_draw(double ave, double sd) {
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

// assignment operator for Random4
Random4 & Random4::operator=(const Random4 &rng) {
	randx=rng.show_seed();
	randx2=rng.show_seed2();
	rng.get_v(v);
	y=rng.get_y();
	return *this;
};
	
// update operation
void Random4::update_rng(const Random4 &rng) {
	randx=rng.show_seed();
	randx2=rng.show_seed2();
	rng.get_v(v);
	y=rng.get_y();
};

double Random4L::max_ran=2.0e+18;

long Random4L::draw() {
	if (++rn_cnt>max_ran) {
		MAX_RN_REACHED m;
		throw m;
	};
	unsigned long t1, t2;
	int i;
	t1=a1*(randx%q1);
	t2=r1*(randx/q1);
	randx=(t1>=t2 ? t1-t2 : m1-t2+t1);
	t1=a2*(randx2%q2);
	t2=r2*(randx2/q2);
	randx2=(t1>=t2 ? t1-t2 : m2-t2+t1);
	// Produces a number in the range 0 to nb-1.
	i=(int) y/((m1-1)/nb_unsigned +1);
	y=(v[i]>randx2 ? v[i]-randx2 : m1-1-(randx2-v[i]));
	v[i]=randx;
	return y;
};

long Random4L::ndraw(const vector<double> &pdf) {
	long size=pdf.size();
	if (size<=1) return 0; // Trivial cases.
	vector<double> cdf(size);
	find_cdf(pdf,cdf);
	// Drawing a number with the cumulative distribution.
	long min=0, max=size;
	long div2=size/2;
	// Find the right bin. (fdraw() will give a double between 0 and 1)
	double yy=fdraw();
	while (min!=div2) {
		if(cdf[div2]<yy) min=div2;
		else 	max=div2;
		div2=min+(max-min)/2;
	};
	return min;
};

long Random4L::ndraw(const vector<long> &pdf) {
	long size=pdf.size();
	if (size<=1) return 0; // Trivial cases.
	vector<double>  cdf(size);
	find_cdf(pdf,cdf);
	// Drawing a number with the cumulative distribution.
	long min=0, max=size;
	long div2=size/2;
	// Find the right bin. (fdraw() will give a double between 0 and 1)
	double yy=fdraw();
	while (min!=div2) {
		if(cdf[div2]<yy) min=div2;
		else 	max=div2;
		div2=min+(max-min)/2;
	};
	return min;
};

// assignment operator for Random4
Random4L & Random4L::operator=(const Random4 &rng) {
	randx=rng.show_seed();
	randx2=rng.show_seed2();
	rng.get_v(v);
	y=rng.get_y();
	return *this;
};

Random4L & Random4L::operator=(const Random4L &rng) {
	randx=rng.show_seed();
	randx2=rng.show_seed2();
	rng.get_v(v);
	y=rng.get_y();
	rn_cnt=rng.show_cnt();
	return *this;
};

// update operation
void Random4L::update_rng(const Random4L &rng) {
	randx=rng.show_seed();
	randx2=rng.show_seed2();
	rng.get_v(v);
	y=rng.get_y();
	rn_cnt=rng.show_cnt();
};

// Draw a double with the gaussian dist. with average and standard deviation.
pair<double,double>  Random4L::gaussian_draw(double ave, double sd) {
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
