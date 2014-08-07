// 
//	CONETSIM - Link.h
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


#ifndef LINK_H
#define LINK_H

#include "Types.h"
#include "TimeSeq.h"
#include <istream>
#include <ostream>
#include <iostream>

namespace conet {

// There are two base classes (LinkB and LinkD) and another pair of template base classes (LinkF and LinkV).
// And another base class (LinkR).
// 
// 	LinkB: for links with delay=1 (homogeneous) (no data stored).
//	LinkD: for links with inhomogeneous delays.
//	
//	LinkF<State>: for links with time-fixed states.
//	LinkV<State>: for links with time-varying states. 
//
//	LinkR: RNG for a link (needed when link has its own stochastic dynamics).
//
// There can be 10 subclasses from above base classes.
//
//	LinkD <- LinkD				(inhomogeneous delays, no state)
//	LinkBR <- LinkB, LinkR			(delay=1, no state, RNG)
//	LinkDR <- LinkD, LinkR			(inhomogeneous delays, no state, RNG)
//
//	LinkBF <- LinkB, LinkF<State>		(delay=1, fixed states)
//	LinkBV <- LinkB, LinkV<State>		(delay=1, varying states)
//	LinkDF <- LinkD, LinkF<State>		(inhomogeneous delays, fixed states)
//	LinkDV <- LinkD, LinkV<State>		(inhomogeneous delays, varying states)
//	LinkBFR <- LinkB, LinkF<State>, LinkR	(delay=1, fixed states, RNG)
//	LinkBVR <- LinkB, LinkV<State>, LinkR	(delay=1, varying states, RNG)
//	LinkDFR <- LinkD, LinkF<State>, LinkR	(inhomogeneous delays, fixed states, RNG)
//	LinkDVR <- LinkD, LinkV<State>, LinkR	(inhomogeneous delays, varying states, RNG)
//
//
// Now we can choose one out of 10 choices given above to represent a specific link type.



// A base class that represents links with delay 1 with no state (only to be used as a base class).
// Since all links have the same delay, delay value doesn't need to be stored.
// If links with a given type have inhomogeneous delays, then LinkD should be inherited instead of this class.
// In other words, there are two choices of base classes for subclasses: LinkB or LinkD. 
class LinkB {
   public:
   	// constructor
	LinkB() {};
	// destructor.
	~LinkB() {};

	// copy the object.
	void copy(const LinkB &link) {};

	// Get the delay.
	DelayType get_delay() const {
		return 1;
	};

	// Set the delay (cannot be done in this class).
	void set_delay(DelayType d) {};

	// Write and read delay.
	void write_delay(std::ostream &of) const {
		of << 1;
	};
	bool read_delay(std::istream &iif) {
		return false; // cannot be done in this class.
	};
};

// A base class that represents a link with inhomogeneous delays (without states) (can be used for certain types of links).
// There are two choices of base classes for subclasses: LinkB or LinkD. 
class LinkD {
   public :
   	// Constructors.
	LinkD(long d=1): delay(d) {};
	LinkD(const LinkD &link): delay(link.get_delay()) {};

	// Destructor.
   	~LinkD() {};
	
	// copy the object.
	void copy(const LinkD &link) {
		delay=link.get_delay();
	};

	// Get the delay.
	DelayType get_delay() const {
		return delay;
	};

	// Set the delay.
	void set_delay(DelayType d) {
		delay=d;
	};

	// Write and read delay.
	void write_delay(std::ostream &of) const {
		of << delay;
	};
	bool read_delay(std::istream &iif) {
		return (iif >> delay);
	};

	// Show the link type (This should be redefined in subclasses.)
	static LinkType get_type() {
		return type;
	};

	// Set the link type (This should be redefined in subclasses.)
	static void set_type(LinkType lt) {
		type=lt;
	};

   private:
   	// Static data member that represent the link type. (This should be redefined in subclasses.)
	static LinkType type;

   protected:
	DelayType delay;
};

// A class that represents links with RNG (stochastic dynamics for links involved) (can be used for certain types of links).
// In this class, its own RNG will be provided, and links with RNG can be derived from this class.
// This class can be added as a base class if links need RNG or not.
class LinkR {
   public:
	// Constructor.
   	LinkR(unsigned long s=1): rng(s) {};	
	LinkR(const RNG_Limit &r): rng(r) {};
	LinkR(const LinkR &link): rng(link.get_rng()) {};

	// Destructor.
   	~LinkR() {};	
	
	// copy the object.
	void copy(const LinkR &link) {
		set_rng(link.get_rng());
	};

    	// Member functions that use RNG are defined below 
   	//(they will only be used by member functions of this class or its subclasses).
	// Drawing a long number between 0 and rand_max().
	long draw() {
		return rng.draw();
	};

	// Draw a long number between nmin and nmax.
	long ndraw(long nmin, long nmax) {
		return rng.ndraw(nmin,nmax);
	};

	// Draw a long number using the given PDF represented by a vector.
	long ndraw(const std::vector<double> &pdf) {
		return rng.ndraw(pdf);
	};

	long ndraw(const std::vector<long> &pdf) {
		return rng.ndraw(pdf);
	};

	// Draw a double between fmin(default=0) and fmax(default=1)
	double fdraw(double fmin=0.0, double fmax=1.0) {
		return rng.fdraw(fmin,fmax);
	};

	// Get the seed.
	unsigned long get_seed() const {
		return rng.show_seed();
	};

	// Set the seed.
	void set_seed(unsigned long s) {
		return rng.seed(s);
	};

	// Show the rn_cnt.
	double get_cnt() const {
		return rng.show_cnt();
	};

	// Set the rn_cnt.
	void set_cnt(double cnt) {
		rng.set_cnt(cnt);
	};

	// Get RNG.
	RNG_Limit get_rng() const {
		return rng;
	};

	// Set RNG
	void set_rng(const RNG_Limit &r) {
		rng.update_rng(r);
	};

   protected:
  	// Random number generator.
	RNG_Limit rng;
   
};

// Class that represents links with delay=1, no state, and RNG.
class LinkBR: public LinkB, public LinkR {
   public :
   	// Constructors.
	LinkBR(unsigned long s=1): LinkB(), LinkR(s) {};	
	LinkBR(const LinkBR &link): LinkB(), LinkR(link.get_rng()) {};	

	// Destructor.
   	~LinkBR() {};

	// copy the object.
	void copy(const LinkBR &link) {
		LinkB::copy(link);
		LinkR::copy(link);
	};

	// Show the link type (This should be redefined in subclasses.)
	static LinkType get_type() {
		return type;
	};

	// Set the link type (This should be redefined in subclasses.)
	static void set_type(LinkType lt) {
		type=lt;
	};

   private:
   	// Static data member that represent the link type. (This should be redefined in subclasses.)
	static LinkType type;
};

// Class that represents links with inhomogeneous delays, no state, and RNG.
class LinkDR: public LinkD, public LinkR {
   public :
   	// Constructors.
	LinkDR(DelayType d=1, unsigned long s=1): LinkD(d), LinkR(s) {};	
	LinkDR(const LinkDR &link): LinkD(link.get_delay()), LinkR(link.get_rng()) {};	

	// Destructor.
   	~LinkDR() {};

	// copy the object.
	void copy(const LinkDR &link) {
		LinkD::copy(link);
		LinkR::copy(link);
	};

	// Show the link type (This should be redefined in subclasses.)
	static LinkType get_type() {
		return type;
	};

	// Set the link type (This should be redefined in subclasses.)
	static void set_type(LinkType lt) {
		type=lt;
	};

   private:
   	// Static data member that represent the link type. (This should be redefined in subclasses.)
	static LinkType type;
};

// A template class that represents a link with a state (fixed with time)  and delay=1 for all.
// This class contains the State class for the state variables for the given link type (which is not yet defined here).
// If the given type has a state, the state class should be defined, and this should be instantiated with State class.
// For State class, default constructor should be given, operator= (copy constructor), operator<<, operator>>
//	should be overloaded (operator less than can be handy, too).
// There are two choices for subclasses: LinkF or LinkV (depending on whether states change with time or not).
template<class State> class LinkF {
   public:
   	// Constructor.
	LinkF(): state() {};
	LinkF(const State &val): state(val) {};
	LinkF(const LinkF<State> &link) {
		state=link.access_state();
	};

	// Destructor.
	~LinkF() {};

	// copy the object.
	void copy(const LinkF<State> &link) {
		state=link.access_state();
	};

	// Get the state
	State get_state() const {
		return state;
	};
	State get_state_T(TimeType t) const {
		return get_state();
	};

	// Access the state
	State & access_state() {
		return state;
	};
	State & access_state_T(TimeType t) {
		return access_state();
	};

	// Set the state
	void set_state(const State &val) {
		state=val;
	};
	void set_state_T(const State &val, TimeType t) {
		set_state(val);
	};

	// Read the node info.
	bool read_state(std::istream &iif) const {
		return (iif >> state);
	};
	bool read_state_T(std::istream &iif, TimeType t) const {
		return (read_state(iif));
	};

	// Write the node info.
	void write_state(std::ostream &of) const {
		of << state;
	};
	void write_state_T(std::ostream &of, TimeType t) const {
		write_state(of);
	};

   protected:
   	State state; // State for the link (it can be user-defined class)

};

// A template class that represents a link with a time-varying states.
// Finite-size time sequence of the given state has to be stored.
// This class contains the State class for the state variables for the given link type (which is not yet defined here).
// If the given type has a state, the state class should be defined, and this should be instantiated with State class.
// For State class, default constructor should be given, operator= (copy constructor), operator<<, operator>>
//	should be overloaded (operator less than can be handy, too).
// There are two choices for subclasses: LinkF or LinkV (depending on whether states change with time or not).
template<class State> class LinkV {
   public:
   	// Constructor.
	LinkV(): states(LinkV<State>::get_default_size(),State(),0) {};
	LinkV(TimeType ss, const State &val, TimeType ft=0): states(ss,val,ft) {};
	LinkV(const LinkV<State> &link): states(link.get_size(),State(),link.get_first_time()) {
		for (TimeType t=get_first_time();t<=get_last_time();t++)
			states.add_value(t,link(t));
	};

	// Destructor.
	~LinkV() {};

	// copy the object.
	void copy(const LinkV<State> &link) {
		reset(link.get_size(),link.get_first_time());
		for (TimeType t=get_first_time();t<=get_last_time();t++)
			states.add_value(t,link(t));
	};

	// Get the state
	State get_state() const {
		return states.get_last_value(); // Not well-defined (do not use it).;
	};
	State get_state_T(TimeType t) const {
		if (states.range(t)) return states(t);
		else throw TimeSeq_Range_Error();
	};

	// Access the state
	State & access_state() {
		return states.get_last_value(); // Not well-defined (do not use it).;
	};
	State & access_state_T(TimeType t) {
		if (states.range(t)) return states(t);
		else throw TimeSeq_Range_Error();
	};

	// Set the state
	void set_state(const State &val) { 
		states.add_value(states.get_last_time(),val); // Not well-defined (do not use it).
	};
	void set_state_T(const State &val, TimeType t) {
		states.add_value(t,val);
	};

	// Read the node info.
	bool read_state(std::istream &iif) const {
		return (iif >> states(get_last_time())); // Not well-defined (do not use it).
	};
	bool read_state_T(std::istream &iif, TimeType t) const {
		if (states.range(t)) return (iif >> states(t));
		else return false;
	};

	// Write the node info.
	void write_state(std::ostream &of) const { // write all states stored with time data.
		for (TimeType t=get_first_time();t<=get_last_time();t++)
			of << t << '\t' << states(t) << '\n';
	};
	void write_state_T(std::ostream &of, TimeType t) const {
		if (states.range(t)) of << states(t);
	};

   	// Reset the first_time and size of time sequence (can be used right after the default constructor has been called.)
	void reset(TimeType size, TimeType ft) {
		states.reset(size,ft);
	};
	
   	// Set the first_time (can be used right after the default constructor has been called.)
	void set_first_time(TimeType ft) {
		states.set_first_time(ft);
	};
	
   	// Get the size of the stored time sequence. (It can be different from 'default_size' (static member of this class).)
	//	(See constructors.)
	TimeType get_size() const {
		return states.get_size();
	}; 
   	// Get the first time of the stored time sequence.
	TimeType get_first_time() const {
		return states.get_first_time();
	}; 

   	// Get the last time of the stored time sequence.
	TimeType get_last_time() const {
		return states.get_last_time();
	}; 

   	// Check if the time is in the range of sequence.
	bool range(TimeType t) const {
		return states.range(t);
	}; 

   	// Advance time without change of the weight.
	void advance_without_change() {
		states.advance_without_change();
	}; 

   protected:
   	TimeSeq<State> states; // Time sequence of states for a given node. 
};

// Class that represents links with delay=1 and time-fixed states.
template<class State> class LinkBF: public LinkB, public LinkF<State> {
   public:
   	// Constructor.
	LinkBF(): LinkB(), LinkF<State>(State()) {};
	LinkBF(const State &val): LinkB(), LinkF<State>(val) {};
	LinkBF(const LinkBF<State> &link): LinkB(), LinkF<State>(link.access_state()) {};

	// Destructor.
	~LinkBF() {};

	// copy the object.
	void copy(const LinkBF<State> &link) {
		LinkF<State>::copy(link);
	};

	// Show the link type (This should be redefined in subclasses.)
	static LinkType get_type() {
		return type;
	};

	// Set the link type (This should be redefined in subclasses.)
	static void set_type(LinkType lt) {
		type=lt;
	};

   private:
   	// Static data member that represent the link type. (This should be redefined in subclasses.)
	static LinkType type;

};

// Class that represents links with delay=1 and time-varying states.
template<class State> class LinkBV: public LinkB, public LinkV<State> {
   public:
   	// Constructor.
	LinkBV(): LinkB(), LinkV<State>(LinkBV<State>::get_default_size(),State(),0) {};
	LinkBV(TimeType ss, const State &val, TimeType ft=0): LinkB(), LinkV<State>(ss,val,ft) {};
	LinkBV(const LinkBV<State> &link): LinkB(), LinkV<State>(link.get_size(),State(),link.get_first_time()) {
		for (TimeType t=LinkV<State>::get_first_time();t<=LinkV<State>::get_last_time();t++)
			LinkV<State>::states.add_value(t,link.access_state_T(t));
	};

	// Destructor.
	~LinkBV() {};
	
	// copy the object.
	void copy(const LinkBV<State> &link) {
		LinkV<State>::copy(link);
	};

	// Show the link type (This should be redefined in subclasses.)
	static LinkType get_type() {
		return type;
	};

	// Set the link type (This should be redefined in subclasses.)
	static void set_type(LinkType lt) {
		type=lt;
	};

	// Set the time size (This should be redefined in subclasses.)
	static void set_default_size(DelayType ts) {
		default_size=ts;
	};

	// Get the time size (This should be redefined in subclasses.)
	static DelayType get_default_size() {
		return default_size;
	};

   private:
   	// Static data member that represent the node type and the size of the time sequence. (This should be redefined in subclasses.)
	static LinkType type;
	static DelayType default_size;
};

// Class that represents links with inhomogeneous delays and time-fixed states.
template<class State> class LinkDF: public LinkD, public LinkF<State> {
   public:
   	// Constructor.
	LinkDF(): LinkD(1), LinkF<State>(State()) {};
	LinkDF(const State &val, DelayType d=1): LinkD(d), LinkF<State>(val) {};
	LinkDF(const LinkDF<State> &link): LinkD(link.get_delay()), LinkF<State>(link.access_state()) {};

	// Destructor.
	~LinkDF() {};

	// copy the object.
	void copy(const LinkDF<State> &link) {
		LinkD::copy(link);
		LinkF<State>::copy(link);
	};

	// Show the link type (This should be redefined in subclasses.)
	static LinkType get_type() {
		return type;
	};

	// Set the link type (This should be redefined in subclasses.)
	static void set_type(LinkType lt) {
		type=lt;
	};

   private:
   	// Static data member that represent the link type. (This should be redefined in subclasses.)
	static LinkType type;

};

// Class that represents links with inhomogenous delays and time-varying states.
template<class State> class LinkDV: public LinkD, public LinkV<State> {
   public:
   	// Constructor.
	LinkDV(): LinkD(1), LinkV<State>(LinkDV<State>::get_default_size(),State(),0) {};
	LinkDV(TimeType ss, const State &val, TimeType ft=0, DelayType d=1): LinkD(d), LinkV<State>(ss,val,ft) {}; // ss; time sequence size
	LinkDV(const LinkDV<State> &link): LinkD(link.get_delay()), LinkV<State>(link.get_size(),State(),link.get_first_time()) {
		for (TimeType t=LinkV<State>::get_first_time();t<=LinkV<State>::get_last_time();t++)
			LinkV<State>::states.add_value(t,link.access_state_T(t));
	};

	// Destructor.
	~LinkDV() {};
	
	// copy the object.
	void copy(const LinkDV<State> &link) {
		LinkD::copy(link);
		LinkV<State>::copy(link);
	};

	// Show the link type (This should be redefined in subclasses.)
	static LinkType get_type() {
		return type;
	};

	// Set the link type (This should be redefined in subclasses.)
	static void set_type(LinkType lt) {
		type=lt;
	};

	// Set the time size (This should be redefined in subclasses.)
	static void set_default_size(DelayType ts) {
		default_size=ts;
	};

	// Get the time size (This should be redefined in subclasses.)
	static DelayType get_default_size() {
		return default_size;
	};

   private:
   	// Static data member that represent the node type and the size of the time sequence. (This should be redefined in subclasses.)
	static LinkType type;
	static DelayType default_size;
};

// Class that represents the links with delay=1, time-fixed states and  RNG.
template<class State> class LinkBFR: public LinkBF<State>, public LinkR {
   public:
   	// Constructor.
	LinkBFR(): LinkBF<State>(State()), LinkR(1) {};
	LinkBFR(const State &val, unsigned long s=1): LinkBF<State>(val), LinkR(s) {};
	LinkBFR(const LinkBFR<State> &link): LinkBF<State>(link.access_state()), LinkR(link.get_rng()) {};

	// copy the object.
	void copy(const LinkBFR<State> &link) {
		LinkBF<State>::copy(link);
		LinkR::copy(link);
	};

	// Destructor.
	~LinkBFR() {};

	// Show the link type (This should be redefined in subclasses.)
	static LinkType get_type() {
		return type;
	};

	// Set the link type (This should be redefined in subclasses.)
	static void set_type(LinkType lt) {
		type=lt;
	};

   private:
   	// Static data member that represent the node type and the size of the time sequence. (This should be redefined in subclasses.)
	static LinkType type;

};

// Class that represents the links with delay=1, time-varying states and  RNG.
template<class State> class LinkBVR: public LinkBV<State>, public LinkR {
   public:
   	// Constructor.
	LinkBVR(): LinkBV<State>(LinkBVR<State>::get_default_size(),State(),0), LinkR(1) {};
	LinkBVR(TimeType ss, const State &val, TimeType ft=0, unsigned long s=1): LinkBV<State>(ss,val,ft), LinkR(s) {};
	LinkBVR(const LinkBVR<State> &link): LinkBV<State>(link.get_size(),State(),link.get_first_time()), LinkR(link.get_rng()) {
		for (TimeType t=LinkV<State>::get_first_time();t<=LinkV<State>::get_last_time();t++)
			LinkV<State>::states.add_value(t,link.access_state_T(t));
	};

	// copy the object.
	void copy(const LinkBVR<State> &link) {
		LinkBV<State>::copy(link);
		LinkR::copy(link);
	};

	// Destructor.
	~LinkBVR() {};

	// Show the link type (This should be redefined in subclasses.)
	static LinkType get_type() {
		return type;
	};

	// Set the link type (This should be redefined in subclasses.)
	static void set_type(LinkType lt) {
		type=lt;
	};

	// Set the time size (This should be redefined in subclasses.)
	static void set_default_size(DelayType ts) {
		default_size=ts;
	};

	// Get the time size (This should be redefined in subclasses.)
	static DelayType get_default_size() {
		return default_size;
	};

   private:
   	// Static data member that represent the node type and the size of the time sequence. (This should be redefined in subclasses.)
	static LinkType type;
	static DelayType default_size;

};

// Class that represents the links with inhomogeneous delays, time-fixed states and  RNG.
template<class State> class LinkDFR: public LinkDF<State>, public LinkR {
   public:
   	// Constructor.
	LinkDFR(): LinkDF<State>(LinkDFR::get_default_size(),State(),0), LinkR(1) {};
	LinkDFR(const State &val, unsigned long s=1): LinkDF<State>(val), LinkR(s) {};
	LinkDFR(const LinkDFR<State> &link): LinkDV<State>(link.access_state()), LinkR(link.get_rng()) {};

	// Destructor.
	~LinkDFR() {};

	// copy the object.
	void copy(const LinkDFR<State> &link) {
		LinkDF<State>::copy(link);
		LinkR::copy(link);
	};

	// Show the link type (This should be redefined in subclasses.)
	static LinkType get_type() {
		return type;
	};

	// Set the link type (This should be redefined in subclasses.)
	static void set_type(LinkType lt) {
		type=lt;
	};

   private:
   	// Static data member that represent the node type and the size of the time sequence. (This should be redefined in subclasses.)
	static LinkType type;
};

// Class that represents the links with inhomogeneous delays, time-varying states and  RNG.
template<class State> class LinkDVR: public LinkDV<State>, public LinkR {
   public:
   	// Constructor.
	LinkDVR(): LinkDV<State>(LinkDVR::get_default_size(),State(),0), LinkR(1) {};
	LinkDVR(TimeType ss, const State &val, TimeType ft=0, DelayType d=1, unsigned long s=1): LinkDV<State>(ss,val,ft,d), LinkR(s) {};
	LinkDVR(const LinkDVR<State> &link): LinkDV<State>(link.get_size(),State(),link.get_first_time(),link.get_delay()), LinkR(link.get_rng()) {
		for (TimeType t=LinkV<State>::get_first_time();t<=LinkV<State>::get_last_time();t++)
			LinkV<State>::states.add_value(t,link.access_state_T(t));
	};

	// Destructor.
	~LinkDVR() {};

	// copy the object.
	void copy(const LinkDVR<State> &link) {
		LinkDV<State>::copy(link);
		LinkR::copy(link);
	};

	// Show the link type (This should be redefined in subclasses.)
	static LinkType get_type() {
		return type;
	};

	// Set the link type (This should be redefined in subclasses.)
	static void set_type(LinkType lt) {
		type=lt;
	};

	// Set the time size (This should be redefined in subclasses.)
	static void set_default_size(DelayType ts) {
		default_size=ts;
	};

	// Get the time size (This should be redefined in subclasses.)
	static DelayType get_default_size() {
		return default_size;
	};

   private:
   	// Static data member that represent the node type and the size of the time sequence. (This should be redefined in subclasses.)
	static LinkType type;
	static DelayType default_size;
};

// Defining static data members for above template classes here. (It should be defined again after a class has been instantiated.)
template<class State> LinkType LinkBF<State>::type;
template<class State> LinkType LinkBV<State>::type;
template<class State> LinkType LinkDF<State>::type;
template<class State> LinkType LinkDV<State>::type;
template<class State> LinkType LinkBFR<State>::type;
template<class State> LinkType LinkBVR<State>::type;
template<class State> LinkType LinkDFR<State>::type;
template<class State> LinkType LinkDVR<State>::type;

template<class State> DelayType LinkBV<State>::default_size;
template<class State> DelayType LinkDV<State>::default_size;
template<class State> DelayType LinkBVR<State>::default_size;
template<class State> DelayType LinkDVR<State>::default_size;

}; // End of namespace conet.
#endif
