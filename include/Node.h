// 
//	CONETSIM - Node.h
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


#ifndef NODE_H
#define NODE_H

#include "TimeSeq.h"
#include "Types.h"
#include <istream>
#include <ostream>
#include <iostream>

namespace conet {

// There are a pair of template base classes (NodeF and NodeV).
// And another base class (LinkR).
// 
//	NodeF<State>: for nodes with time-fixed states.
//	NodeV<State>: for nodes with time-varying states. 
//
//	NodeR: RNG for a node (needed when link has its own stochastic dynamics).
//
// There can be 5 combinations of subclasses from above base classes.
//
//	NodeR				(no state, RNG)
//
//	NodeF <- NodeF<State>		(fixed states)
//	NodeV <- NodeV<State>		(varying  states)
//	NodeFR <- NodeF<State>, NodeR	(fixed states, RNG)
//	NodeVR <- NodeV<State>, NodeR	(varying  states, RNG)


// Now we can choose one out of 10 choices given above to represent a specific link type.
// A template class that represents a node with a state (state is not chagning).
// This class contains the T class for the state variables for the given node type (which is not yet defined here).
// If the given type has a state, the state class should be defined, and this should be instantiated with State class.
// For State class, default constructor should be given, operator= (or copy constructor) and operator<<, operator>> should be overloaded, 
//	(less than operation can be handy when we store sets of states, too).
template<class State> class NodeF {
   public:
   	// Constructor.
	NodeF(): state() {};
	NodeF(const State &val): state(val) {};
	NodeF(const NodeF<State> &node): state(node.access_state()) {};

	// Destructor.
	~NodeF() {};

	// copy the object.
	void copy(const NodeF<State> &node) {
		state=node.access_state();
	};

	// Get the state
	State get_state() const {
		return state;
	};
	State get_state_T(TimeType t) const {
		return get_state();
	};

	// Access the state
	State & access_state() const {
		return state;
	};
	State & access_state_T(TimeType t) const {
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

	// Show the NodeType.
	static NodeType get_type() {
		return type;
	};

	// Set the NodeType (This should be redefined in subclasses.)
	static void set_type(NodeType nt) {
		type=nt;
	};

   private:
   	// Static data member that represent the node type. (This should be redefined in subclasses.)
	static NodeType type;

   protected:
   	State state; // State for the node (it can be user-defined class)

};

// A template class that represents a node with a time-varying state, so time sequence of the given state has to be stored.
// This class contains the State class for the state variables for the given node type (which is not yet defined here).
// If the given type has a state, the state class should be defined, and this should be instantiated with State class.
// For State class, default constructor should be given, operator= should be overloaded, 
//	and print() function is assumed to exist (operator less than can be handy, too).
template<class State> class NodeV {
   public:
   	// Constructor.
	NodeV(): states(NodeV::get_default_size(),State(),0) {};
	NodeV(TimeType ss, const State &val, TimeType ft=0): states(ss,val,ft) {};
	NodeV(const NodeV<State> &node): states(NodeV::get_default_size(),State(),node.get_first_time()) {
		for (TimeType t=get_first_time();t<=get_last_time();t++)
			states.add_value(t,node.access_state_T(t));
	};

	// Destructor.
	~NodeV() {};

	// copy the object.
	void copy(const NodeV<State> &node) {
		reset(node.get_size(),node.get_first_time());
		for (TimeType t=get_first_time();t<=get_last_time();t++)
			states.add_value(t,node.access_state_T(t));
	};

	// Get the state
	State get_state() const {
		return states.get_last_value(); // Not well-defined (do not use it).
	};
	State get_state_T(TimeType t) const {
		if (states.range(t)) return states(t);
	};

	// Access the state
	State & access_state() const {
		return states.get_last_value(); // Not well-defined (do not use it).
	};
	State & access_state_T(TimeType t) const {
		if (states.range(t)) return states(t);
	};

	// Set the state
	void set_state(const State &val) { 
		states.add_value(states.get_last_time(),val); // Not well-defined (do not use it).
	};
	void set_state_T(const State &val, TimeType t) {
		if (t<=get_last_time()+1) states.add_value(t,val); // when t=get_last_time()+1, it is updating the state at the next step.
	};

	// Read the node info.
	bool read_state(std::istream &iif) const {
		return (iif >> states(get_last_time())); // Not well-defined (do not use it).
	};
	bool read_state_T(std::istream &iif, TimeType t) const {
		if (states.range(t)) return iif >> states(t);
		else false;
	};

	// Write the node info.
	void write_state(std::ostream &of) const {
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
	
   	// Get the size of the stored time sequence.
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

   	// Advance time without change of state.
	void advance_without_change() {
		states.advance_without_change();
	}; 

	// Show the NodeType.
	static NodeType get_type() {
		return type;
	};

	// Set the NodeType (This should be redefined in subclasses.)
	static void set_type(NodeType nt) {
		type=nt;
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
	static NodeType type;
	static DelayType default_size;

   protected:
   	TimeSeq<State> states; // Time sequence of states for a given node. 

};

// A class that represents nodes with RNG (stochastic dynamics for nodes).
// In this class, its own RNG will be provided, and nodes with RNG can be derived from this class.
// Usage : If the object is called "node", 
//	node.set_seed(seed) will set the seed for the RNG.
//	node.draw() will return the next random number.
// 	node.ndraw(nmin,nmax) will draw an integer between nmin and nmax.
// 	node.ndraw(vector<double> &prob) will draw an integer using the pdf.
// 	node.fdraw(fmin=0,fmax=1) will draw a double between fmin and fmax.
class NodeR {
   public:
	// Constructor.
   	NodeR(unsigned long s=1): rng(s) {};
	NodeR(const RNG_Limit &r): rng(r) {};
	NodeR(const NodeR &node): rng(node.get_rng()) {};

	// Destructor.
   	~NodeR() {};	
	
	// copy the object.
	void copy(const NodeR &node) {
		set_rng(node.get_rng());
	};

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

	// Get the RNG (See Types.h for the definition of RNG.)
	RNG_Limit get_rng() const {
		return rng;
	};

	// Access the RNG (returns the reference of the RNG to use it).
	RNG_Limit & access_rng() {
		return rng;
	};
	
	// Set operations
	void set_rng(const RNG_Limit &r) {
		rng.update_rng(r);
	};

	// Show the NodeType.
	static NodeType get_type() {
		return type;
	};

	// Set the NodeType (This should be redefined in subclasses.)
	static void set_type(NodeType nt) {
		type=nt;
	};

   private:
   	// Static data member that represent the node type and the size of the time sequence. (This should be redefined in subclasses.)
	static NodeType type;

   protected:
	RNG_Limit rng; // Random number generator.
   
};

// A class that represents nodes with RNG (stochastic dynamics for nodes) (states are time-fixed).
template<class State> class NodeFR: public NodeF<State>, public NodeR {
   public:
	// Constructor.
   	NodeFR(unsigned long s=1): NodeF<State>(State()), NodeR(s) {};
	NodeFR(const State &val, unsigned long s=1): NodeF<State>(val), NodeR(s) {};
	NodeFR(const NodeFR<State> &node): NodeF<State>(node.access_state()), NodeR(node.get_rng()) {};

	// Destructor.
   	~NodeFR() {};	
	
	// copy the object.
	void copy(const NodeFR<State> &node) {
		NodeF<State>::copy(node);
		NodeR::copy(node);
	};

	// Show the NodeType.
	static NodeType get_type() {
		return type;
	};

	// Set the NodeType (This should be redefined in subclasses.)
	static void set_type(NodeType nt) {
		type=nt;
	};

   private:
   	// Static data member that represent the node type and the size of the time sequence. (This should be redefined in subclasses.)
	static NodeType type;
};

// A class that represents nodes with RNG (stochastic dynamics for nodes) (states are time-varying).
template<class State> class NodeVR: public NodeV<State>, public NodeR {
   public:
	// Constructor.
   	NodeVR(unsigned long s=1): NodeV<State>(NodeVR<State>::get_default_size(),State(),0), NodeR(s) {};
	NodeVR(TimeType ss, const State &val, TimeType ft=0, unsigned long s=1): NodeV<State>(ss,val,ft), NodeR(s) {};
	NodeVR(const NodeVR<State> &node): NodeV<State>(node.get_size(),State(),node.get_first_time()), NodeR(node.get_rng()) {
		for (TimeType t=NodeV<State>::get_first_time();t<=NodeV<State>::get_last_time();t++)
			NodeV<State>::states.add_value(t,node.access_state_T(t));
	};

	// Destructor.
   	~NodeVR() {};	
	
	// copy the object.
	void copy(const NodeVR<State> &node) {
		NodeV<State>::copy(node);
		NodeR::copy(node);
	};

	// Show the NodeType.
	static NodeType get_type() {
		return type;
	};

	// Set the NodeType (This should be redefined in subclasses.)
	static void set_type(NodeType nt) {
		type=nt;
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
	static NodeType type;
	static DelayType default_size;
};

template<class State> NodeType NodeF<State>::type;
template<class State> NodeType NodeV<State>::type;
template<class State> NodeType NodeFR<State>::type;
template<class State> NodeType NodeVR<State>::type;

template<class State> DelayType NodeV<State>::default_size;
template<class State> DelayType NodeVR<State>::default_size;

}; // End of namespace conet.
#endif
