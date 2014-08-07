// 
//	CONETSIM - NodeInput.h
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


#ifndef NODEINPUT_H
#define NODEINPUT_H

#include "Types.h"
#include "Errors.h"
#include <fstream>
#include <istream>
#include <ostream>
#include <iostream>
#include <string>

namespace conet {

// A template class for a type of input states that has random inputs.
// A template base class for a type of input states
// The state class should be defined, and this should be instantiated with State class.
// For State class, default constructor should be given, operator= (or copy constructor) and operator<<, operator>> should be overloaded, 
//	(less than operation can be handy when we store sets of states, too).
template<class State> class NodeInputBase {
   public:
   	// Constructor.
	NodeInputBase(InputType it=0): type(it) {};

	// Destructor.
	virtual ~NodeInputBase() {};

	// Get the InputType.
	InputType get_type() {
		return type;
	};

	// Set the InputType.
	void set_type(InputType it) {
		type=it;
	};

	// Get all input nodes in this type.
	virtual NodeIDSSet get_input_nodes() {
		return NodeIDSSet();
	};

	// Clear all inputs.
	virtual void clear() {};

	// Get input for a given node at a given time.
	virtual State get_input(NodeID id, TimeType t) {
		return State();
	};

	// Add an input node (needed when an input node of this type has been created).
	virtual bool add_input_node(NodeID id) {
		return false;
	};
	virtual bool add_input_node_TS(NodeID id, const State &st, TimeType t) {
		return false;
	};

	// Remove an input node (needed when an input node of this type has been removed)..
	virtual bool remove_input_node(NodeID id, TimeType t=0) {
		return false;
	};

	// Add the input (to be used when inputs can be added).
	virtual bool add_input(NodeID id, const State &st, TimeType t=0) {
		return false;
	};

	// Read the input from an input file for a given time (format: time ID state).
	virtual bool read_input_from_file(TimeType t) {
		return false;
	};

	// Find if the given node has the given input type.
	virtual bool if_ID(NodeID id) {
		return false;
	};

   protected:
   	InputType type;
};

// A template class for a type of input states that has random inputs.
// The state class should be defined, and this should be instantiated with State class.
// For State class, default constructor should be given, operator= (or copy constructor) and operator<<, operator>> should be overloaded, 
//	(less than operation can be handy when we store sets of states, too).
// It is also assumed that the input State has the "random_state(RNG)" member function defined.
template<class State> class NodeInputRandom: public NodeInputBase<State> {
   public:
   	// Constructor.
	NodeInputRandom(unsigned long s=1, InputType it=0): NodeInputBase<State>(it), rng(s), inputs_idset() {};
	NodeInputRandom(const RNG &r, InputType it=0): NodeInputBase<State>(it), rng(r), inputs_idset() {};

	// Destructor.
	~NodeInputRandom() {};

	// Clear all inputs.
	void clear() {
		inputs_idset.clear();
	};

	// Get all input nodes in this type.
	NodeIDSSet get_input_nodes() {
		return inputs_idset;
	};

	// Get input for a given node at a given time.
	State get_input(NodeID id, TimeType t) {
		if (if_ID(id)) {
			return State::random_state(rng);	// random_state(RNG) should be defined before using this class.
		};
		throw Bad_Input_ID();
	};

	// Add an input node (needed when an input node of this type has been created).
	bool add_input_node(NodeID id) {
		if (!if_ID(id)) {
			inputs_idset.insert(id);
			return true;
		};
		return false;
	};
	bool add_input_node_TS(NodeID id, const State &st, TimeType t) {
		return add_input_node(id);
	};

	// Remove an input node (needed when an input node of this type has been removed)..
	bool remove_input_node(NodeID id, TimeType t=0) {
		if (if_ID(id)) {
			inputs_idset.erase(id);
			return true;
		};
		return false;
	};

	// Find if the given node has the given input type.
	bool if_ID(NodeID id) {
		return (inputs_idset.find(id)!=inputs_idset.end());
	};

   protected:
	RNG rng;	// Random Number Generator for Inputs.
   	NodeIDSSet inputs_idset;	// A set of nodes for this input type.
};

// A template base classes for a type of input states that has their inputs in a file.
// The state class should be defined, and this should be instantiated with State class.
// For State class, default constructor should be given, operator= (or copy constructor) and operator<<, operator>> should be overloaded, 
//	(less than operation can be handy when we store sets of states, too).
template<class State> class NodeInputFile: public NodeInputBase<State> {
   public:
   	// Constructor.
	NodeInputFile(InputType it=0): NodeInputBase<State>(it), current_input_time(), filename(), seq_size(), inputs_id_state(), inputs_id_seq_states() {};
	NodeInputFile(const std::string &fn, TimeType ss, InputType it=0): NodeInputBase<State>(it), current_input_time(), filename(fn), seq_size(ss), inputs_id_state(), inputs_id_seq_states() {
		ifile.open(filename.c_str(),std::ios::in);
		if (!ifile) throw Bad_Input_File(filename);
	}; // If filename is not empty, inputs are from the given file.

	// Destructor.
	~NodeInputFile() {
		ifile.close();
	};

	// Open the given file.
	void open(std::string fn) {
		filename=fn;
		if (ifile.is_open()) ifile.close();
		ifile.open(filename.c_str(),std::ios::in);
		if (!ifile) throw Bad_Input_File(filename);
	};

	// Set the sequence size.
	void set_seq_size(TimeType ss) {
		seq_size=ss;
	};

	// Set the current time.
	void set_current_input_time(TimeType ct) {
		current_input_time=ct;
	};

	// Get all input nodes at the current time in this type.
	NodeIDSSet get_input_nodes() {
		NodeIDSSet idset;
		if (seq_size==1) 
			for (typename std::map<NodeID,State>::const_iterator i=inputs_id_state.begin();i!=inputs_id_state.end();i++) idset.insert(i->first);
		else
			for (typename std::map<NodeID,TimeSeq<State> >::const_iterator i=inputs_id_seq_states.begin();i!=inputs_id_seq_states.end();i++) idset.insert(i->first);
		return idset;
	};

	// Clear all inputs.
	void clear() {
		if (seq_size==1)
			inputs_id_state.clear();
		else
			inputs_id_seq_states.clear();
	};

	// Get input for a given node at a given time.
	State get_input(NodeID id, TimeType t) {
		if (if_ID(id)) {
			if (seq_size==1) {
				if (t!=current_input_time) throw Bad_Input_Time();
				return inputs_id_state[id];
			}
			else
				if (inputs_id_seq_states[id].range(t)) return inputs_id_seq_states[id](t);
				else throw TimeSeq_Range_Error();
		};
		throw Bad_Input_ID();
	};

	// Add an input node (needed when an input node of this type has been created).
	bool add_input_node(NodeID id) {
		return add_input_node_TS(id, State(), current_input_time);
	};
	bool add_input_node_TS(NodeID id, const State &st, TimeType t) {
		if (!if_ID(id)) {
			if (seq_size==1)
				inputs_id_state[id]=st;
			else
				inputs_id_seq_states[id]=TimeSeq<State>(seq_size,st,t);
			return true;
		};
		return false;
	};

	// Remove an input node (needed when an input node of this type has been removed)..
	bool remove_input_node(NodeID id, TimeType t=0) {
		if (if_ID(id)) {
			if (seq_size==1)
				inputs_id_state.erase(id);
			else
				inputs_id_seq_states.erase(id);
			return true;
		};
		return false;
	};

	// Add the input (to be used when inputs can be added) at the current input time.
	bool add_input(NodeID id, const State &st, TimeType t) {
		if (t!=current_input_time) throw Bad_Input_Time();
		if (if_ID(id)) {
			if (seq_size==1)
				inputs_id_state[id]=st;
			else
				inputs_id_seq_states[id].add_value(t,st);
			return true;
		};
		return false;
	};

	// Read the input from an input file for a given time (format: time ID state).
	// Returns false when ID doesn't exist, or it fails to read data.
	// If existing node(s) doesn't have an input value, we assume that it keeps the previous value (or a default value).
	// Also assumes that 't' can increase one step at a time.
	bool read_input_from_file(TimeType t) {
		if (t!=current_input_time && t!=current_input_time+1) throw Bad_Input_Time();
		current_input_time=t;
		TimeType time;
		NodeID id;
		State st;
		// For Time sequence (seq_size>1), advance the time, so that (given time) <= (the last time for the sequences).
		if (seq_size>1)
			for (typename std::map<NodeID,TimeSeq<State> >::iterator i=inputs_id_seq_states.begin();i!=inputs_id_seq_states.end();i++)
				while ((i->second).get_last_time() < current_input_time) 
					(i->second).advance_with_this_value(State());
		if (peek_int(ifile,t)) {
			do {
				if (ifile >> time >> id >> st && if_ID(id)) {
					if (seq_size==1)
						inputs_id_state[id]=st;
					else {
						inputs_id_seq_states[id].add_value(time,st);
					};
				}
				else return false;
			} while (peek_int(ifile,t));
			return true;
		}
		else return false;
	};

	// Find if the given node has the given input type.
	bool if_ID(NodeID id) {
		if (seq_size==1)
			return (inputs_id_state.find(id)!=inputs_id_state.end());
		else
			return (inputs_id_seq_states.find(id)!=inputs_id_seq_states.end());
	};

   protected:
	// Peek the int from a file (not changing the file pointer, assuming inputs are formatted integers).
	bool peek_int(std::ifstream & iif, long num) {
		std::streampos pos=iif.tellg();
		if (pos>=0) {
			long temp;
			if (iif >> temp && temp==num) {
				iif.seekg(pos);
				return true;
			}
			else {
				iif.seekg(pos);
				return false;
			}
		}
		else return false;
	};
		
   protected:
   	TimeType current_input_time;	// Current time for input.
	std::string filename; // Name of the input file (to be used when inpus are given with a file). 
				// Format: time id state
	TimeType seq_size;	// Size of the time sequence of inputs.
	std::ifstream ifile; // File input stream (to be used when inputs are given with a file).
	
   	std::map<NodeID,TimeSeq<State> > inputs_id_seq_states; // Storage for the time sequence of inputs. (when seq_size>1)
   	std::map<NodeID,State> inputs_id_state; // Storage for the inputs for just one time step. (when seq_size=1)
};

// Example: input state with a double
struct InputStateDouble {
	double state;

	InputStateDouble(double st=0): state(st) {};
	static InputStateDouble random_state(RNG &rng) {
		return InputStateDouble(rng.fdraw(0,1));
	};
};

typedef NodeInputRandom<InputStateDouble> InputDoubleRandom;
typedef NodeInputFile<InputStateDouble> InputDoubleFile;

}; // End of namespace conet.

inline std::istream & operator>>(std::istream &iif, conet::InputStateDouble & s) {
	return (iif >> s.state);
};
inline std::ostream & operator<<(std::ostream &of, const conet::InputStateDouble & s) {
	return (of << s.state);
};

#endif
