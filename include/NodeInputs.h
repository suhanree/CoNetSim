// 
//	CONETSIM - NodeInputs.h
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


#ifndef NODEINPUTS_H
#define NODEINPUTS_H

#include "NodeInput.h"
#include "Types.h"
#include "Errors.h"
#include <istream>
#include <ostream>
#include <iostream>

namespace conet {

// Class that can be used when there is no input node in the network.
class NodeInputs {
   public:
   	// Constructor.
	NodeInputs() {};

	// Destructor.
	~NodeInputs() {};

	// Get the number of input types.
	virtual InputType get_ntypes() const {
		return 0;
	};

	// Get the type of the input given the node.
	virtual InputType get_type(NodeID id) const {
		return -1;
	};

	// Returns true if the given node is an input node.
	virtual bool if_input(NodeID id) const {
		return false;
	};

	// Returns true if the given node is an input node and the given type is correct type.
	virtual bool if_input_type(NodeID id, InputType it) const {
		return false;
	};

	// Get all input nodes in this type.
	virtual NodeIDSSet get_input_nodes(InputType it) {
		throw Bad_Input_Type();
	};

	// Clear all inputs.
	virtual void clear() {};

	// Add an input node (needed when an input node of this type has been created).
	virtual bool add_input_node(NodeID id, InputType it) {
		throw Bad_Input_Type();
	};

	// Remove an input node (needed when an input node of this type has been removed)..
	virtual bool remove_input_node(NodeID id, InputType it, TimeType t=0) {
		throw Bad_Input_Type();
	};

	// Read the input from an input file for a given time (format: time ID state).
	virtual bool read_input_from_file(InputType it, TimeType t) {
		throw Bad_Input_Type();
	};
};

// Template class that contains information of all node input when there is only one input type.
template<class State> class NodeInputs1S: public NodeInputs {
   public:
   	// Constructor.
	NodeInputs1S(NodeInputBase<State> &ni): NodeInputs(), inputs(ni) {};

	// Destructor.
	~NodeInputs1S() {};

	// Get the number of input types.
	InputType get_ntypes() const {
		return 1;
	};

	// Get the type of the input given the node.
	InputType get_type(NodeID id) const {
		if (inputs.if_ID(id)) return inputs.get_type();
		return -1;
	};

	// Returns true if the given node is an input node.
	bool if_input(NodeID id) const {
		return inputs.if_ID(id);
	};

	// Returns true if the given node is an input node and the given type is correct type.
	bool if_input_type(NodeID id, InputType it) const {
		return (inputs.if_ID(id) && inputs.get_type()==it);
	};

	// Get all input nodes in this type.
	NodeIDSSet get_input_nodes(InputType it) {
		if (it==inputs.get_type()) return inputs.get_input_nodes();
		throw Bad_Input_Type();
	};

	// Clear all inputs.
	void clear() {
		inputs.clear();
	};

	// Get input for a given node at a given time.
	State get_input(NodeID id, InputType it, TimeType t) {
		if (it==get_type(id)) return inputs.get_input(id,t);
		throw Bad_Input_Type();
	};

	// Add an input node (needed when an input node of this type has been created).
	bool add_input_node(NodeID id, InputType it) {
		if (it==inputs.get_type()) return inputs.add_input_node(id);
		throw Bad_Input_Type();
	};
	virtual bool add_input_node_TS(NodeID id, InputType it, const State &st, TimeType t) {
		if (it==inputs.get_type()) return inputs.add_input_node_TS(id,st,t);
		throw Bad_Input_Type();
	};

	// Remove an input node (needed when an input node of this type has been removed).
	bool remove_input_node(NodeID id, InputType it, TimeType t=0) {
		if (it==get_type(id)) return inputs.remove_input_node(id,t);
		throw Bad_Input_Type();
	};

	// Add the input (to be used when inputs can be added).
	virtual bool add_input(NodeID id, InputType it, const State &st, TimeType t=0) {
		if (it==inputs.get_type() && inputs.if_ID(id)) return inputs.add_input(id,st,t);
		throw Bad_Input_Type();
	};

	// Read the input from an input file for a given time (format: time ID state).
	bool read_input_from_file(InputType it, TimeType t) {
		if (it==inputs.get_type()) return inputs.read_input_from_file(t);
		throw Bad_Input_Type();
	};

   protected:
   	NodeInputBase<State> &inputs;
};

// Template class that contains information of all node input when there are two input types.
template<class State1, class State2> class NodeInputs2S: public NodeInputs1S<State1> {
   public:
   	// Constructor.
	NodeInputs2S(NodeInputBase<State1> &ni1, NodeInputBase<State2> &ni2): NodeInputs1S<State1>(ni1), inputs2(ni2) {};

	// Destructor.
	~NodeInputs2S() {};

	// Get the number of input types.
	InputType get_ntypes() const {
		return 2;
	};

	// Get the type of the input given the node.
	InputType get_type(NodeID id) const {
		if (NodeInputs1S<State1>::if_input(id)) return NodeInputs1S<State1>::get_type();
		if (inputs2.if_ID(id)) return inputs2.get_type();
		return -1;
	};

	// Returns true if the given node is an input node.
	bool if_input(NodeID id) const {
		return (NodeInputs1S<State1>::if_input(id) || inputs2.if_ID(id));
	};

	// Returns true if the given node is an input node and the given type is correct type.
	bool if_input_type(NodeID id, InputType it) const {
		return (NodeInputs1S<State1>::if_input_type(id,it) || (inputs2.if_ID(id) && inputs2.get_type()==it));
	};

	// Get all input nodes in this type.
	NodeIDSSet get_input_nodes(InputType it) {
		if (it==NodeInputs1S<State1>::get_type()) return NodeInputs1S<State1>::get_input_nodes();
		if (it==inputs2.get_type()) return inputs2.get_input_nodes();
		throw Bad_Input_Type();
	};

	// Clear all inputs.
	void clear() {
		NodeInputs1S<State1>::clear();
		inputs2.clear();
	};

	// Get input for a given node at a given time.
	virtual State2 get_input2(NodeID id, InputType it, TimeType t) {
		if (it==inputs2.get_type() && inputs2.if_ID(id)) return inputs2.get_input(id,t);
		else throw Bad_Input_Type();
	};

	// Add an input node (needed when an input node of this type has been created).
	bool add_input_node(NodeID id, InputType it) {
		if (it==NodeInputs1S<State1>::get_type()) return NodeInputs1S<State1>::add_input_node(id);
		if (it==inputs2.get_type()) return inputs2.add_input_node(id);
		throw Bad_Input_Type();
	};
	virtual bool add_input_node_TS2(NodeID id, InputType it, const State2 &st, TimeType t) {
		if (it==inputs2.get_type()) return inputs2.add_input_node_TS(id,st,t);
		throw Bad_Input_Type();
	};

	// Remove an input node (needed when an input node of this type has been removed)..
	bool remove_input_node(NodeID id, InputType it, TimeType t=0) {
		if (it==NodeInputs1S<State1>::get_type()) return NodeInputs1S<State1>::remove_input_node(id,t);
		if (it==inputs2.get_type(id)) return inputs2.remove_input_node(id,t);
		throw Bad_Input_Type();
	};

	// Add the input (to be used when inputs can be added).
	virtual bool add_input2(NodeID id, InputType it, const State2 &st, TimeType t=0) {
		if (it==inputs2.get_type() && inputs2.if_ID(id)) return inputs2.add_input(id,st,t);
		throw Bad_Input_Type();
	};

	// Read the input from an input file for a given time (format: time ID state).
	bool read_input_from_file(InputType it, TimeType t) {
		if (it==NodeInputs1S<State1>::get_type()) return NodeInputs1S<State1>::read_input_from_file(t);
		if (it==inputs2.get_type()) return inputs2.read_input_from_file(t);
		throw Bad_Input_Type();
	};

   protected:
   	NodeInputBase<State2> &inputs2;
};

// Example: when there is one type of inputs of double.
typedef NodeInputs1S<InputStateDouble> NodeInputsDouble;


}; // End of namespace conet.
#endif
