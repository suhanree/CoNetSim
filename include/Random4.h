// 
//	CONETSIM - Random4.h
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

#ifndef RANDOM4_H
#define RANDOM4_H

#include "Random.h"
#include <vector>
#include <valarray>
#include <utility>
#include <iostream>

namespace conet {

//
// A derived class for the random number generator class.
// Using L'Ecuyer's algorithm. (RAN2 in NR)
// The period is about 2x10^18.
// It will produce a number i (1<=i<=2147483562=2^31-86)
//

class Random4 : public Random {
   public :
	// Constructor.
   	Random4(unsigned long s=0);

	// Destructor.
   	~Random4() {};	
	
	// Provides a seed. (v and randx2 have to be changed too)
	void seed(const unsigned long &s);	

	// Maximum value converted to double.
	double rand_max() {return 2.147483562e+9;};	

	// Period of the generator (approximate value).
	double period() {return 2.0e+18;}

	// Drawing a long number between 1 and rand_max().
	long draw();	

	// Draw a long number between nmin and nmax.
	long ndraw(long nmin, long nmax) {
		return nmin+long((double(nmax)-nmin+1)*draw()/(rand_max()+1.0));
	};

	// Draw a long number using the given PDF represented by a vector.
	long ndraw(const std::vector<double> &pdf);
	long ndraw(const std::vector<long> &pdf);

	// Draw a double between fmin(default=0) and fmax(default=1)
	double fdraw(double fmin=0.0, double fmax=1.0) {
		return fmin+(fmax-fmin)*draw()/rand_max();
	};

	// Draw a double with the gaussian dist. with average and standard deviation.
	// Returns two values as a pair of doubles.
	std::pair<double,double> gaussian_draw(double ave=0.0, double sd=1.0);

	// operator version of 'draw()' function
	long operator() () {return draw();};

	// assignment operator for Random4
	Random4 & operator=(const Random4 &rng);

	// update operation
	void update_rng(const Random4 &rng);

	// Show values of data members.
	unsigned long show_seed2() const {
		return randx2;
	};

	void get_v(std::valarray<unsigned long> &v1) const {
		v1=v;
	};

	unsigned long get_y() const {
		return y;
	};

   protected :
   	unsigned long randx2;	// The second seed.
	std::valarray<unsigned long> v;
	unsigned long y;
};

// Class of RNG that has the limit for the number of random numbers used.
class Random4L : public Random4 {
   public :
	// Constructor.
   	Random4L(unsigned long s=0): Random4(s&0x7fffffff), rn_cnt(0) {};	
   	Random4L(const Random4L &r): Random4(), rn_cnt() {
		randx=r.show_seed();
		randx2=r.show_seed2();
		r.get_v(v);
		y=r.get_y();
		rn_cnt=r.show_cnt();
	};	

	// Destructor.
   	~Random4L() {};	
	
	// Drawing a long number between 0 and rand_max().
	long draw();	

	// Draw a long number between nmin and nmax.
	long ndraw(long nmin, long nmax) {
		return nmin+long((double(nmax)-nmin+1)*draw()/(rand_max()+1.0));
	};

	// Draw a long number using the given PDF represented by a vector.
	long ndraw(const std::vector<double> &pdf);
	long ndraw(const std::vector<long> &pdf);

	// Draw a double between fmin(default=0) and fmax(default=1)
	double fdraw(double fmin=0.0, double fmax=1.0) {
		return fmin+(fmax-fmin)*draw()/rand_max();
	};

	// Draw a double with the gaussian dist. with average and standard deviation.
	// Returns two values as a pair of doubles.
	std::pair<double,double> gaussian_draw(double ave=0.0, double sd=1.0);

	// operator version of 'draw()' function
	long operator() () {return draw();};

	// Show the rn_cnt.
	double show_cnt() const {
		return rn_cnt;
	};

	// Set the rn_cnt.
	void set_cnt(double cnt) {
		rn_cnt=cnt;
	};

	// Get the maximum number of random numbers one object can use.
	static double get_max_ran() {
		return max_ran;
	};

	// Set the maximum number of random numbers one object can use.
	static void set_max_ran(double m) {
		max_ran=m;
	};

	// assignment operator for Random4L
	Random4L & operator=(const Random4 &rng);
	Random4L & operator=(const Random4L &rng);

	// update operations
	void update_rng(const Random4 &rng) {
		Random4::update_rng(rng);
	};
	void update_rng(const Random4L &rng);

   private :
	double rn_cnt;
	static double max_ran;
};

}; // End of namespace conet.
#endif
