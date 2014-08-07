// 
//	CONETSIM - Random2.h
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

#ifndef RANDOM2_H
#define RANDOM2_H

#include "Random.h"
#include <vector>
#include <utility>

namespace conet {

//
// A derived class for the random number generator class.
// Using Park and Miller with Schrage's algorithm. (RAN0 in NR)
// It would be adequate for less than 10^8 random numbers. 
// (portable).
// It will produce a number i (1<=i<=2147483646=2^31-2)

class Random2 : public Random {
   public :
	// Constructor.
   	Random2(unsigned long s=0) : Random(s&0x7fffffff) {};	

	// Destructor.
   	~Random2() {};	

	// Maximum value converted to double.
	double rand_max() {return 2.147483646e+9;};	

	// Period of the generator.
	double period() {return 2.147483646e+9;}

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

	// assignment operator for Random2
	Random2 & operator=(const Random2 &rng) {
		randx=rng.show_seed();
		return *this;
	};

	// update operation
	void update_rng(const Random2 &rng) {
		randx=rng.show_seed();
	};

};

// Class of RNG that has the limit for the number of random numbers used.
class Random2L : public Random2 {
   public :
	// Constructor.
   	Random2L(unsigned long s=0) : Random2(s&0x7fffffff), rn_cnt(0) {};	
	// Destructor.
   	~Random2L() {};	
	
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
	long show_cnt() const {
		return rn_cnt;
	};

	// Set the rn_cnt.
	void set_cnt(long cnt) {
		rn_cnt=cnt;
	};

	// Get the maximum number of random numbers one object can use.
	static long get_max_ran() {
		return max_ran;
	};

	// Set the maximum number of random numbers one object can use.
	static void set_max_ran(long m) {
		max_ran=m;
	};

	// assignment operator for Random2L
	Random2L & operator=(const Random2 &rng) {
		randx=rng.show_seed();
		return *this;
	};

	Random2L & operator=(const Random2L &rng) {
		randx=rng.show_seed();
		rn_cnt=rng.show_cnt();
		return *this;
	};

	// update operations
	void update_rng(const Random2 &rng) {
		Random2::update_rng(rng);
	};
	void update_rng(const Random2L &rng) {
		randx=rng.show_seed();
		rn_cnt=rng.show_cnt();
	};

   private :
	long rn_cnt;
	static long max_ran;
};

}; // End of namespace conet.
#endif
