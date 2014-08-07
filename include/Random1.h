// 
//	CONETSIM - Random1.h
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

#ifndef RANDOM1_H
#define RANDOM1_H

#include "Random.h"
#include <vector>
#include <utility>

namespace conet {

//
// A derived class for the random number generator class.
// Uses a simple linear congruential method.
// It would be adequate for less than 10^8 random numbers. 
// It is assumed that the size of 'long' is >= 4 bytes.
// It will produce a number i (0<=i<=2147483647=2^31-1)
//
class Random1 : public Random {
   public :
	// Constructor.
   	Random1(unsigned long s=0) : Random(s&0x7fffffff) {};	// s should be <= rand_max.
	// Destructor.
   	~Random1() {};	
	
	// Maximum value converted to double.
	double rand_max() {return 2.147483647e+9;};	

	// Period of the generator.
	double period() {return 2.147483648e+9;}

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

	// assignment operator for Random1
	Random1 & operator=(const Random1 &rng) {
		randx=rng.show_seed();
		return *this;
	};

	// update operation
	void update_rng(const Random1 &rng) {
		randx=rng.show_seed();
	};

};

// Class of RNG that has the limit for the number of random numbers used.
class Random1L : public Random1 {
   public :
	// Constructor.
   	Random1L(unsigned long s=0) : Random1(s), rn_cnt(0) {};	
	// Destructor.
   	~Random1L() {};	
	
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

	// assignment operator for Random1L
	Random1L & operator=(const Random1 &rng) {
		randx=rng.show_seed();
		return *this;
	};

	Random1L & operator=(const Random1L &rng) {
		randx=rng.show_seed();
		rn_cnt=rng.show_cnt();
		return *this;
	};

	// update operations
	void update_rng(const Random1 &rng) {
		Random1::update_rng(rng);
	};
	void update_rng(const Random1L &rng) {
		randx=rng.show_seed();
		rn_cnt=rng.show_cnt();
	};

   private :
	long rn_cnt;
	static long max_ran; // Maximum number of random numbers that can be generated from this RNG.
};

}; // End of namespace conet.
#endif
