// 
//	CONETSIM - Random3.C
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

#include "Random3.h"
#include "Errors.h"
#include "Utilities.h"
#include <vector>
#include <cmath>

namespace conet {

using namespace std;

// Number of shuffling bins.
static const int nb=32;
static const unsigned long nb_unsigned=32;
// Constants for the generator.
static const unsigned long a=16807, m=2147483647;
static const unsigned long r=2836, q=127773;

// Constructor.
Random3::Random3(unsigned long s)
	: Random(s&0x7fffffff), v(nb) {
	if (randx==0 || randx>2147383646) randx=1;
	for (int i=nb+7;i>=0;i--) {
		unsigned long t1, t2;
		t1=a*(randx%q);
		t2=r*(randx/q);
		randx=(t1>=t2 ? t1-t2 : m-t2+t1);
		if (i<nb) v[i]=randx;
	};
	y=v[0];
};
	
// Provides a seed.
void Random3::seed(const unsigned long &s) {
	randx=s&0x7fffffff;
	if (randx==0 || randx>2147383646) randx=1;
	for (int i=nb+7;i>=0;i--) {
		unsigned long t1, t2;
		t1=a*(randx%q);
		t2=r*(randx/q);
		randx=(t1>=t2 ? t1-t2 : m-t2+t1);
		if (i<nb) v[i]=randx;
	};
	y=v[0];
};	

// Draw function.
long Random3::draw() {
	unsigned long t1, t2;
	int i;
	t1=a*(randx%q);
	t2=r*(randx/q);
	randx=(t1>=t2 ? t1-t2 : m-t2+t1);
	// Produces a number in the range 0 to nb-1.
	i=(int) y/((m-1)/nb_unsigned +1);	
	y=v[i];
	v[i]=randx;
	return y; // 0<y<m
};

long Random3::ndraw(const vector<double> &pdf) {
	long size=pdf.size();
	if (size<=1) return 0; // Trivial cases.
	vector<double> cdf(size);
	find_cdf(pdf,cdf);
	cdf[0]=0;
	for (long i=1;i<size;i++) {
		cdf[i]=0.0;
		for (long j=0; j<i; j++) cdf[i]+=pdf[j];
	};
	// Normalize.
	for (long i=1;i<size;i++) cdf[i]/=(cdf[size-1]+pdf[size-1]); 
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

long Random3::ndraw(const vector<long> &pdf) {
	long size=pdf.size();
	if (size<=1) return 0; // Trivial cases.
	vector<double>  cdf(size);
	find_cdf(pdf,cdf);
	cdf[0]=0;
	for (long i=1;i<size;i++) {
		cdf[i]=0.0;
		for (long j=0; j<i; j++) cdf[i]+=pdf[j];
	};
	// Normalize.
	for (long i=1;i<size;i++) cdf[i]/=(cdf[size-1]+pdf[size-1]); 
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
pair<double,double>  Random3::gaussian_draw(double ave, double sd) {
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

// assignment operator for Random3
Random3 & Random3::operator=(const Random3 &rng) {
	randx=rng.show_seed();
	rng.get_v(v);
	y=rng.get_y();
	return *this;
};

// update operation
void Random3::update_rng(const Random3 &rng) {
	randx=rng.show_seed();
	rng.get_v(v);
	y=rng.get_y();
};
	
long Random3L::max_ran=400000000;

long Random3L::draw() {
	if (++rn_cnt>max_ran) {
		MAX_RN_REACHED m;
		throw m;
	};
	unsigned long t1, t2;
	int i;
	t1=a*(randx%q);
	t2=r*(randx/q);
	randx=(t1>=t2 ? t1-t2 : m-t2+t1);
	// Produces a number in the range 0 to nb-1.
	i=(int) y/((m-1)/nb_unsigned +1);	
	y=v[i];
	v[i]=randx;
	return y;
};

long Random3L::ndraw(const vector<double> &pdf) {
	long size=pdf.size();
	if (size<=1) return 0; // Trivial cases.
	vector<double> cdf(size);
	find_cdf(pdf,cdf);
	cdf[0]=0;
	for (long i=1;i<size;i++) {
		cdf[i]=0.0;
		for (long j=0; j<i; j++) cdf[i]+=pdf[j];
	};
	// Normalize.
	for (long i=1;i<size;i++) cdf[i]/=(cdf[size-1]+pdf[size-1]); 
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

long Random3L::ndraw(const vector<long> &pdf) {
	long size=pdf.size();
	if (size<=1) return 0; // Trivial cases.
	vector<double>  cdf(size);
	find_cdf(pdf,cdf);
	cdf[0]=0;
	for (long i=1;i<size;i++) {
		cdf[i]=0.0;
		for (long j=0; j<i; j++) cdf[i]+=pdf[j];
	};
	// Normalize.
	for (long i=1;i<size;i++) cdf[i]/=(cdf[size-1]+pdf[size-1]); 
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

// assignment operator for Random3
Random3L & Random3L::operator=(const Random3 &rng) {
	randx=rng.show_seed();
	rng.get_v(v);
	y=rng.get_y();
	return *this;
};

Random3L & Random3L::operator=(const Random3L &rng) {
	randx=rng.show_seed();
	rng.get_v(v);
	y=rng.get_y();
	rn_cnt=rng.show_cnt();
	return *this;
};

// update operation
void Random3L::update_rng(const Random3L &rng) {
	randx=rng.show_seed();
	rng.get_v(v);
	y=rng.get_y();
	rn_cnt=rng.show_cnt();
};

// Draw a double with the gaussian dist. with average and standard deviation.
pair<double,double>  Random3L::gaussian_draw(double ave, double sd) {
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
