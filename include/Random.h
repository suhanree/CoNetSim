//
//	CONETSIM - Random.h
//
//
//	Copyright (C) 2011	Suhan Ree
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

// The random number generators, inherited from this base class,
// are basically using
// the linear congruential method of Park and Miller with Schrage's
// algorithm, and Bays and Durham's shuffling algorithm.
// (Ref. Numerical Recipes in C++).

// Obtained the idea of this structure of the classes from
// "C++ Complete handbook" by Straustrub.

#ifndef RANDOM_H
#define RANDOM_H

#include <iostream>

namespace conet {

//
// A base class for the random number generator class.
//
class Random {
   public :
	// Constructor.
	Random(unsigned long s=1) : randx(s) {};	
	// Destructor.
	~Random() {};	
	// Show the seed.
	unsigned long show_seed() const {return randx;};	
	void print_seed() const{std::cout << randx << std::endl;};

	// Provides a seed.
	void seed(const unsigned long &s) {randx=s&0x7fffffff;};	

   protected :
	// A seed for the random number generator (faster when randx is used
	// directly in "draw" functions -> protected).
	unsigned long randx;	
};

}; // End of namespace conet.
#endif
