//
//	CONETSIM - Nodes.h
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


#ifndef NODES_H
#define NODES_H

#include "Node.h"
#include "TimeSeq.h"
#include "Types.h"
#include "Errors.h"
#include <istream>
#include <ostream>
#include <iostream>
#include <map>

namespace conet {

// A class that contains all of current node objects with their ID's.
// There can be more than one type of nodes, and each type can have its own state space (represented by a class).
// Also input nodes can exist, and there can be more than one type of inputs (information will be stored in another class, NodeInputs).
class Nodes {
   public:
   	// Constructor.  (Starts with no node, and nodes can be added later.)
   	Nodes(TimeType &t, DelayType md=1): time(t), max_delay(md), nodes_type_idset() {};
   	Nodes(TimeType &t, const std::set<NodeType> &typeset, DelayType md=1): time(t), max_delay(md) {
		for (std::set<NodeType>::const_iterator i=typeset.begin();i!=typeset.end();i++)
			nodes_type_idset[*i]=NodeIDSSet();
	};

	// Virtual destructor.
	virtual ~Nodes() {};

	// Add a type.
	bool add_type(NodeType nt) {
		if (nodes_type_idset.find(nt)!=nodes_type_idset.end()) return false;
		nodes_type_idset[nt]=NodeIDSSet();
		return true;
	};

	// Check if the given type exist.
	bool if_type(NodeType nt) const {
		return (nodes_type_idset.find(nt)!=nodes_type_idset.end());
	};

	// Get the NodeType of the given node.
	virtual NodeType get_type(NodeID id) const { // will be overwritten.
		return NodeType();
	};

   	// Find if the given ID already exists.
   	virtual bool if_ID(NodeID id) const=0;
	
	// Find if the given ID and Type pair exist.
	bool if_ID_type(NodeID id, NodeType nt) const {
		std::map<NodeType,NodeIDSSet>::const_iterator i=nodes_type_idset.find(nt);
		return (i!=nodes_type_idset.end() && (i->second).find(id)!=(i->second).end());
	};

	// Get the set of nodes for the given type.
	NodeIDSSet find_nodes(NodeType nt) const {
		if (if_type(nt)) return nodes_type_idset.find(nt)->second;
		else throw Bad_Node_Type();
	};

	// Get the current time.
	TimeType get_time() const {
		return time;
	};

	// Get the number of NodeType's.
	NodeType get_ntypes() const {
		return nodes_type_idset.size();
	};

	// Get the number of nodes.
	virtual NodeID get_nnodes() const=0;
	NodeID get_nnodes(NodeType nt) const {
		std::map<NodeType,NodeIDSSet>::const_iterator i=nodes_type_idset.find(nt);
		if (i!=nodes_type_idset.end())
			return (i->second).size();
		else throw Bad_Node_Type();
	};

	// Get the max_delay of the whole network.
	DelayType get_max_delay() const {
		return max_delay;
	};

   	// Delete all nodes now.
	virtual void clear() {
		for (std::map<NodeType,NodeIDSSet>::iterator i=nodes_type_idset.begin();i!=nodes_type_idset.end();i++)
			(i->second).clear();
	};
	
	// Advancing the time of all current node objects upto the given time without change(if needed).
	virtual void advance(TimeType t)=0;
	virtual void advance_N(NodeType nt, TimeType t)=0;

	// Adding a node at the current time or the given time.
	// (Default values will be used for node states.)
	virtual bool add_node(NodeID id, NodeType nt, TimeType t)=0;
	virtual bool add_node_generatedID(NodeID &id, NodeType nt, TimeType t)=0; // id represent a new ID

	// Removing a node with the given ID (the chosen node should be erased at (the current time) + max_delay
	// At the current time, the given node will be listed in the set of nodes to be removed, and
	//	later this node and its past information will be erased from here (using 'advance' memeber function).
	virtual bool remove_node(NodeID id, NodeType nt, TimeType t)=0;

	// Set the random seeds if the given NodeType is the derived from NodeR. 
	// (rseed: random seed, max_rn: the maximun number of random nunmbers a node can have).
	void initialize_random_seeds(NodeType nt, unsigned long rseed, long max_rn);

	// Get the RNG of the given node (if the node has RNG, i.e. derived from NodeR).
	virtual RNG_Limit & access_rng(NodeID id, NodeType nt)=0;

	// Set the seed of RNG of the given node (if the node has RNG, i.e. derived from NodeR).
	void set_seed(NodeID id, NodeType nt, unsigned long seed) {
		(this->access_rng(id,nt)).seed(seed);
	};

	// Write the nodes info.
	void write(std::ostream &of) const {
		this->write_T(of,time);
	};
	virtual void write_T(std::ostream &of, TimeType t) const {};
	void write_ID(std::ostream &of, NodeID id) const {
		this->write_ID_T(of,id,time);
	};
	virtual void write_ID_T(std::ostream &of, NodeID id, TimeType t) const {};

   protected:
	const TimeType &time;	// Reference for the current time (keeping track of the current time of the whole system).
	
	DelayType max_delay; // Maximum delay for all links.

	std::map<NodeType,NodeIDSSet> nodes_type_idset;	// Sets of nodes for each NodeType.
};

// Subclass of Nodes whose numbers is fixed (NF:Number-Fixed), which means that nodes will not be either created or removed from the network.
// This class can be used with graph classes that starts with GraphF~.
// If some nodes have states, subclasses of this class can be used. If not, this class can be used.
// (Kepp in mind that even if the number of nodes don't change, their states can still change if they have one.)
// Subclasses of this class need to overwrite below member functions
//	clear, advance, advance_N, add_node, access_rng(if needed), write_T, write_ID_T (from Nodes)
class NodesNF : public Nodes {
   public:
   	// Constructor.
   	NodesNF(TimeType &t, DelayType md=1): Nodes(t,md), nodes_id_type() {};
   	NodesNF(TimeType &t, const std::set<NodeType> &typeset, DelayType md=1): Nodes(t,typeset,md), nodes_id_type() {};

	// Virtual destructor.
	~NodesNF() {};
	
	// Get the number of nodes.
	NodeID get_nnodes() const {
		return nodes_id_type.size();
	};

	// Get the NodeType of the given node. (should be redefined in the subclasses).
	NodeType get_type(NodeID id) const {
		if (this->if_ID(id)) return nodes_id_type[id];
		throw Bad_Node_ID();
	};

   	// Find if the given ID already exists.
   	bool if_ID(NodeID id) const {
		return (id>=0 && id<get_nnodes());
	};

   	// Delete all nodes now.
	void clear() {
		Nodes::clear();
		nodes_id_type.clear();
	};
	
	// Advancing the time of all current node objects upto the given time without change(if needed).
	void advance(TimeType t) {};
	void advance_N(NodeType nt, TimeType t) {};

	// Adding a node (add_node should be overwritten if node states have to be changed) (time has to be 0 to add nodes).
	// (only works when time=0 and id=nnodes, which means that id increases by one at a time)
	bool add_node(NodeID id, NodeType nt, TimeType t) {
		if (!if_type(nt)) throw Bad_Node_Type();
		if (t!=0) return false;
		if (id==get_nnodes()) {// ID can be added incrementally only.
			nodes_id_type.resize(get_nnodes()+1);
			nodes_id_type[id]=nt;
			nodes_type_idset[nt].insert(id);
			return true;	
		}
		else throw Bad_Node_ID();
	};
	bool add_node_generatedID(NodeID &id, NodeType nt, TimeType t) { // id represent a new ID
		id=get_nnodes();
		return this->add_node(id,nt,t);
	};

	// Removing a node with the given ID (not allowed here).
	bool remove_node(NodeID id, NodeType nt, TimeType t) {
		return false;
	};

   protected:

	std::vector<NodeType> nodes_id_type;	// array of NodeType. (use only when there are many types)
};

// Subclass of Nodes whose number is varying (NV: Number-Varying), which means that nodes can be eithre created or removed from the network.
// This class can be used with graph classes that starts with GraphV~.
// If some nodes have states, subclasses of this class can be used. If not, this class can be used.
// Subclasses of this class need to overwrite below member functions
//	clear, advance, advance_N, add_node, access_rng(if needed), write_T, write_ID_T (from Nodes)
//	delete_node (from NodesNV)
class NodesNV: public Nodes {
   public:
   	// Constructor.
   	NodesNV(TimeType &t, DelayType md=1): Nodes(t,md), nodes_id_type(),\
		to_be_removed(), to_be_removed_at_time(), max_ID(-1), current_greatest_ID(-1) {};
   	NodesNV(TimeType &t, const std::set<NodeType> &typeset, DelayType md=1): Nodes(t,typeset,md), nodes_id_type(),\
		to_be_removed(), to_be_removed_at_time(), max_ID(-1), current_greatest_ID(-1) {};

	// Virtual destructor.
	~NodesNV() {};
	
	// Get the number of nodes.
	NodeID get_nnodes() const {
		return nodes_id_type.size();
	};

	// Get the NodeType of the given node. (should be redefined in the subclasses).
	NodeType get_type(NodeID id) const {
		if (this->if_ID(id)) {
			std::map<NodeID,NodeType>::const_iterator i=nodes_id_type.find(id);
			if (i!=nodes_id_type.end()) return i->second;
		};
		throw Bad_Node_ID();
	};

   	// Find if the given ID already exists.
   	bool if_ID(NodeID id) const {
		return (nodes_id_type.find(id)!=nodes_id_type.end());
	};

   	// Delete all nodes now.
	void clear() {
		Nodes::clear();
		nodes_id_type.clear();
		to_be_removed.clear();
		to_be_removed_at_time.clear();
		max_ID=-1;
		current_greatest_ID=-1;
	};
	
	// Advancing the time of all current node objects upto the given time.
	void advance(TimeType t) {
		this->delete_nodes(t); // Delete nodes to be deleted at t
	};
	void advance_N(NodeType nt, TimeType t) {
		this->delete_nodes_N(nt,t); // Delete nodes to be deleted at t
	};

	// Adding a node.
	bool add_node(NodeID id, NodeType nt, TimeType t) {
		if (!if_type(nt)) throw Bad_Node_Type();
		if (this->if_ID(id)) return false;
		else {
			nodes_id_type[id]=nt;
			nodes_type_idset[nt].insert(id);
			if (current_greatest_ID<id) current_greatest_ID=id;
			return true;	
		};
	};
	bool add_node_generatedID(NodeID &id, NodeType nt, TimeType t) { // id represent a new ID
		NodeID new_id=find_new_ID();
		if (new_id>=0) {
			id=new_id;
			return this->add_node(new_id,nt,t);
		}
		else	false;
	};

	// Removing a node with the given ID (the chosen node should be erased at (the current time) + max_delay
	// At the current time, the given node will be listed in the set of nodes to be removed, and
	//	later this node and its past information will be erased from here (using 'advance' memeber function).
	bool remove_node(NodeID id, NodeType nt, TimeType t) {
		if (if_ID_type(id,nt) && to_be_removed.find(id)==to_be_removed.end()) {
			to_be_removed.insert(id);
			to_be_removed_at_time[time+max_delay].insert(id);
			nodes_id_type.erase(id);
			nodes_type_idset[nt].erase(id);
			return true;
		}
		else return false;
	};

   protected:
	// Delete given nodes now ("delete" means wiping all data about this node from here).
	//	While "remove" means nodes disappear from the network at the given time,
	//	keep in mind that the past states of nodes can be used even after given nodes has been removed.
	void delete_nodes(TimeType t) {
		if (to_be_removed_at_time.find(t)!=to_be_removed_at_time.end()) return; // If there is no node to delete, return.
		for (NodeIDSSet::iterator i=to_be_removed_at_time[t].begin();i!=to_be_removed_at_time[t].end();i++)
			this->delete_node(*i);
		to_be_removed_at_time.erase(t);
	};
	void delete_nodes_N(NodeType nt, TimeType t) {
		if (to_be_removed_at_time.find(t)!=to_be_removed_at_time.end()) return; // If there is no node to delete, return.
		for (NodeIDSSet::iterator i=to_be_removed_at_time[t].begin();i!=to_be_removed_at_time[t].end();i++)
			if (this->get_type(*i)==nt) {
				this->delete_node(*i);
				to_be_removed_at_time[t].erase(i);
			};
		if (to_be_removed_at_time[t].size()==0) to_be_removed_at_time.erase(t); // If there is no node left to delete at time t, delete it.
	};
	virtual void delete_node(NodeID id) {
		nodes_type_idset[nodes_id_type[id]].erase(id);
		nodes_id_type.erase(id);
	};

   	// Finding a new ID (new ID should not already exist).
	// Returns -1, if there is no available ID.
   	NodeID find_new_ID() {
		// Before the current_greates_ID reaches the maximum value for the given NodeID type.
		if (max_ID==-1) { // Maximum possible value hasn't been reached
			if (++current_greatest_ID<0)  // Maximum possible value has been reached here.
				max_ID=current_greatest_ID--; 	// Max_ID is set as the maximum possible value.
			else return current_greatest_ID;
		}
		// After the max_ID has been reached, we have to find an unused ID.
		//	(Expensive, it can be better.)
		else 
			for (NodeID id=0;id<=max_ID;id++)
				if (!(this->if_ID(id))) return id; 
		return -1;
	};


   protected:
	std::map<NodeID,NodeType> nodes_id_type;	// map of NodeID and NodeType. (use only when there are many types)

	NodeIDSSet to_be_removed;			// Set of nodes to be removed (after max_delay).
	std::map<TimeType,NodeIDSSet> to_be_removed_at_time;	// Set of nodes to be removed at given times in the future.

	NodeID max_ID;	// maximum value for the NodeID type.
	NodeID current_greatest_ID;	// current greatest ID.

};

// Template class that represents fixed number of nodes with one Node state involved.
// For convenience, we will call the class that represents Node State as "State".
// Subclasses of this class need to overwrite below member functions
//	clear, advance, advance_N, add_node (if needed), access_rng(if needed), write_T, write_ID_T (from Nodes)
//	add_node_S, get_state, access_state, set_state (from NodeNF1S)
template<class State> class NodesNF1S: public NodesNF {
   public:
   	// Constructor.
   	NodesNF1S(TimeType &t, DelayType md=1, NodeType nt=0): NodesNF(t,md), type(nt) {
		add_type(nt);
	};
   	NodesNF1S(TimeType &t, std::set<NodeType> &typeset, DelayType md=1, NodeType nt=0): NodesNF(t,typeset,md), type(nt) {
		add_type(nt);
	};

	// Virtual destructor.
	~NodesNF1S() {};
	
	// Get the type of the associated state.
	NodeType get_type_S() const {
		return type;
	};

	// Adding a node at the current time.
	// (To provide the specific state of the node, define the member function 
	//	in the subclasses of NodesNF1S, using the appropriate Node subclasses.)
	bool add_node(NodeID id, NodeType nt, TimeType t) {
		if (nt==type) return this->add_node_S(id,nt,t,State());
		return NodesNF::add_node(id,nt,t);
	};
	virtual bool add_node_S(NodeID id, NodeType nt, TimeType t, const State &s)=0;
	bool add_node_S_generatedID(NodeID &id, NodeType nt, TimeType t, const State &s) { // id represent a new ID
		if (nt!=type) throw Bad_Node_Type();
		id=get_nnodes();
		return this->add_node_S(id,nt,t,s);
	};

	// Get the state of the given node.
	virtual State get_state(NodeID id, NodeType nt, TimeType t) const=0;

	// Access the state of the given node.
	virtual State & access_state(NodeID id, NodeType nt, TimeType t)=0;

	// Set the Node state of the given node (should be overwritten in subclasses).
	virtual bool set_state(NodeID id, NodeType nt, TimeType t, const State &s)=0;

   protected:
   	NodeType type; // Node type with the given state (there can be other types without states).
};

// Template class for nodes, whose number is fixed, with two Node states involved.
// For convenience, we will call the class that represents Node States as "State1" and "State2".
// Subclasses of this class need to overwrite below member functions
//	clear, advance, advance_N, add_node (if needed), access_rng(if needed), write_T, write_ID_T (from Nodes)
//	add_node_S, get_state, set_state (from NodeNF1S)
//	add_node_S2, get_state2, set_state2 (from NodeNF2S)
template<class State1, class State2> class NodesNF2S: public NodesNF1S<State1> {
   public:
   	// Constructor.
   	NodesNF2S(TimeType &t, DelayType md=1, NodeType nt=0, NodeType nt2=1): NodesNF1S<State1>(t,md,nt), type2(nt2) {
		if (type2==NodesNF1S<State1>::type) throw Bad_Node_Type();
		NodesNF1S<State1>::add_type(nt2);
	};
   	NodesNF2S(TimeType &t, std::set<NodeType> &typeset, DelayType md=1, NodeType nt=0, NodeType nt2=1): \
		NodesNF1S<State1>(t,typeset,md,nt), type2(nt2) {
		if (type2==NodesNF1S<State1>::type) throw Bad_Node_Type();
		NodesNF1S<State1>::add_type(nt2);
	};

	// Virtual destructor.
	~NodesNF2S() {};
	
	// Get the type of the associated state.
	NodeType get_type_S2() const {
		return type2;
	};

	// Adding a node at the current time.
	// (To provide the specific state of the node, define the member function 
	//	in the subclasses of NodesNF1S, using the appropriate Node subclasses.)
	bool add_node(NodeID id, NodeType nt, TimeType t) {
		if (nt==NodesNF1S<State1>::type) return this->add_node_S(id,nt,t,State1());
		if (nt==type2) return this->add_node_S2(id,nt,t,State2());
		return NodesNF::add_node(id,nt,t);
	};
	virtual bool add_node_S2(NodeID id, NodeType nt, TimeType t, const State2 &s)=0;
	bool add_node_S2_generatedID(NodeID &id, NodeType nt, TimeType t, const State2 &s) { // id represent a new ID
		if (nt!=type2) throw Bad_Node_Type();
		id=NodesNF1S<State1>::get_nnodes();
		return this->add_node_S2(id,nt,t,s);
	};

	// Get the state of the given node.
	virtual State2 get_state2(NodeID id, NodeType nt, TimeType t) const=0;

	// Access the state of the given node.
	virtual State2 & access_state2(NodeID id, NodeType nt, TimeType t)=0;

	// Set the Node state of the given node (should be overwritten in subclasses).
	virtual bool set_state2(NodeID id, NodeType nt, TimeType t, const State2 &s)=0;

   protected:
   	NodeType type2; // Node type with the given state (there can be other types without states).
};

// Template class that represents varying number of nodes with one Node state involved.
// For convenience, we will call the class that represents Node State as "State".
// Subclasses of this class need to overwrite below member functions
//	clear, advance, advance_N, add_node (if needed), access_rng(if needed), write_T, write_ID_T (from Nodes)
//	delete_node (from NodesNV)
//	add_node_S, get_state, set_state (from NodeNV1S)
template<class State> class NodesNV1S: public NodesNV {
   public:
   	// Constructor.
   	NodesNV1S(TimeType &t, DelayType md=1, NodeType nt=0): NodesNV(t,md), type(nt) {
		add_type(nt);
	};
   	NodesNV1S(TimeType &t, std::set<NodeType> &typeset, DelayType md=1, NodeType nt=0): NodesNV(t,typeset,md), type(nt) {
		add_type(nt);
	};

	// Virtual destructor.
	~NodesNV1S() {};
	
	// Get the type of the associated state.
	NodeType get_type_S() const {
		return type;
	};

	// Adding a node at the current time.
	// (To provide the specific state of the node, define the member function 
	//	in the subclasses of NodesNF1S, using the appropriate Node subclasses.)
	bool add_node(NodeID id, NodeType nt, TimeType t) {
		if (nt==type) return this->add_node_S(id,nt,t,State());
		return NodesNV::add_node(id,nt,t);
	};
	virtual bool add_node_S(NodeID id, NodeType nt, TimeType t, const State &s)=0;
	bool add_node_S_generatedID(NodeID &id, NodeType nt, TimeType t, const State &s) { // id represent a new ID
		if (nt!=type) throw Bad_Node_Type();
		NodeID new_id=NodesNV::find_new_ID();
		if (new_id>=0) {
			id=new_id;
			return this->add_node_S(new_id,nt,t,s);
		}
		else	false;
	};

	// Get the state of the given node.
	virtual State get_state(NodeID id, NodeType nt, TimeType t) const=0;

	// Access the state of the given node.
	virtual State & access_state(NodeID id, NodeType nt, TimeType t)=0;

	// Set the Node state of the given node (should be overwritten in subclasses).
	virtual bool set_state(NodeID id, NodeType nt, TimeType t, const State &s)=0;

   protected:
   	NodeType type; // Node type with the given state (there can be other types without states).
};

// Template class for nodes, whose number is varying, with two Node states involved.
// For convenience, we will call the class that represents Node States as "State1" and "State2".
// Subclasses of this class need to overwrite below member functions
//	clear, advance, advance_N, add_node (if needed), access_rng(if needed), write, write_T, write_ID, write_ID_T (from Nodes)
//	delete_node (from NodesNV)
//	add_node_S, get_state, set_state (from NodeNV1S)
//	add_node_S2, get_state2, set_state2 (from NodeNV2S)
template<class State1, class State2> class NodesNV2S: public NodesNV1S<State1> {
   public:
   	// Constructor.
   	NodesNV2S(TimeType &t, DelayType md=1, NodeType nt=0, NodeType nt2=1): NodesNV1S<State1>(t,md,nt), type2(nt2) {
		if(type2==NodesNV1S<State1>::type) throw Bad_Node_Type();
		NodesNV1S<State1>::add_type(nt2);
	};
   	NodesNV2S(TimeType &t, std::set<NodeType> &typeset, DelayType md=1, NodeType nt=0, NodeType nt2=1): \
		NodesNV1S<State1>(t,typeset,md,nt), type2(nt2) {
		if(type2==NodesNV1S<State1>::type) throw Bad_Node_Type();
		NodesNV1S<State1>::add_type(nt2);
	};

	// Virtual destructor.
	~NodesNV2S() {};
	
	// Get the type of the associated state.
	NodeType get_type_S2() const {
		return type2;
	};

	// Adding a node at the current time.
	// (To provide the specific state of the node, define the member function 
	//	in the subclasses of NodesNF1S, using the appropriate Node subclasses.)
	bool add_node(NodeID id, NodeType nt, TimeType t) {
		if (nt==NodesNV1S<State1>::type) return this->add_node_S(id,nt,t,State1());
		if (nt==type2) return this->add_node_S2(id,nt,t,State2());
		return NodesNV::add_node(id,nt,t);
	};
	virtual bool add_node_S2(NodeID id, NodeType nt, TimeType t, const State2 &s)=0;
	bool add_node_S2_generatedID(NodeID &id, NodeType nt, TimeType t, const State2 &s) { // id represent a new ID
		if (nt!=type2) throw Bad_Node_Type();
		NodeID new_id=NodesNV::find_new_ID();
		if (new_id>=0) {
			id=new_id;
			return this->add_node_S2(new_id,nt,t,s);
		}
		else	false;
	};

	// Get the state of the given node.
	virtual State2 get_state2_T(NodeID id, NodeType nt, TimeType t) const=0;

	// Access the state of the given node.
	virtual State2 & access_state2_T(NodeID id, NodeType nt, TimeType t)=0;

	// Set the Node state of the given node (should be overwritten in subclasses).
	virtual bool set_state2_T(NodeID id, NodeType nt, TimeType t, const State2 &s)=0;

   protected:
   	NodeType type2; // Node type with the given state (there can be other types without states).
};

// Example1:
// Nodes that have only one type (NodeR) with varying number (no state, with RNG)
class NodesNV_R: public NodesNV {
   public:
   	// Constructor.
   	NodesNV_R(TimeType &t, DelayType md=1, NodeType nt=0): NodesNV(t,md), nodeset() {
		add_type(nt);
	};

	// Virtual destructor.
	~NodesNV_R() {};
	
   	// Delete all nodes now.
	void clear() {
		NodesNV::clear();
		nodeset.clear();
	};
	
	// Advancing the time of all current node objects upto the given time (no need here).
	void advance(TimeType t) {};
	void advance_N(NodeType nt, TimeType t) {};

	// Add a node 
	bool add_node(NodeID id, NodeType nt, TimeType t) {
		if (NodesNV::add_node(id,nt,t)) {
			nodeset.insert(std::map<NodeID,NodeR>::value_type(id,NodeR()));
			return true;
		}
		else return false;
	};

	// Get the RNG of the given node (if the node has RNG, i.e. derived from NodeR).
	RNG_Limit &access_rng(NodeID id, NodeType nt) {
		if (if_type(nt)) return nodeset[id].access_rng();
		else throw Bad_Node_Type();
	};

	void write_T(std::ostream &of, TimeType t) const {};
	void write_ID_T(std::ostream &of, NodeID id, TimeType t) const {};

   protected:
	// Delete a node
	void delete_node(NodeID id) {
		NodesNV::delete_node(id);
		nodeset.erase(id);
	};

   protected:
	std::map<NodeID,NodeR> nodeset;
};

// Example2:
// Nodes that have only one type with the varying state (NodeVR<State>) with the fixed number (state, with RNG)
template<class State> class NodesNF_VR: public NodesNF1S<State> {
   public:
   	// Constructor.
   	NodesNF_VR(TimeType &t, NodeID nnodes=0, DelayType md=1, NodeType nt=0): NodesNF1S<State>(t,md,nt), nodeset() {
		for (NodeID id=0;id<nnodes;id++) add_node(id,nt,NodesNF1S<State>::time);
	};

	// Virtual destructor.
	~NodesNF_VR() {};
	
   	// Delete all nodes now.
	void clear() {
		NodesNF::clear();
		nodeset.clear();
	};
	
	// Advancing the time of all current node objects upto the reference time (if needed).
	void advance(TimeType t) {
		if (t<NodesNF1S<State>::time) return;
		for (NodeID id=0;id<nodeset.size();id++) {
			if (nodeset[id].get_last_time()<t) { // outside the range of the time sequence
				while (nodeset[id].get_last_time()<t)
					nodeset[id].advance_without_change();
			}
			else { 	// If the time is still inside the range of the time sequence.
				// Assumes that time advances with 1 and 
				// that the state value is kept from the previous valule.
				nodeset[id].set_state_T(nodeset[id].access_state_T(t-1),t);
			};
		};
	};
	void advance_N(NodeType nt, TimeType t) {
		if (this->get_type_S()==nt) this->advance(t);
	};

	// Add a node (RNG seed has to be set separately, too)
	bool add_node_S(NodeID id, NodeType nt, TimeType t, const State &s) {
		if (nt!=NodesNF1S<State>::get_type_S()) throw Wrong_Node_Type();
		if (NodesNF::add_node(id,nt,t)) {
			nodeset.resize(NodesNF1S<State>::get_nnodes());
			nodeset[id]=NodeVR<State>(NodesNF1S<State>::max_delay+1,s,t);
			return true;
		}
		else return false;
	};

	// Get the state of the given node.
	State get_state(NodeID id, NodeType nt, TimeType t) const {
		if (nt!=NodesNF1S<State>::get_type_S()) throw Wrong_Node_Type();
		if (NodesNF1S<State>::if_ID(id) && nodeset[id].range(t)) return nodeset[id].get_state_T(t);
		else throw Bad_Node_ID();
	};

	// Access the state of the given node.
	State & access_state(NodeID id, NodeType nt, TimeType t) {
		if (nt!=NodesNF1S<State>::get_type_S()) throw Wrong_Node_Type();
		if (!NodesNF1S<State>::if_ID(id)) throw Bad_Node_ID();
		if (nodeset[id].range(t)) return nodeset[id].access_state_T(t);
		else throw TimeSeq_Range_Error();
	};

	// Set the Node state of the given node.
	bool set_state(NodeID id, NodeType nt, TimeType t, const State &s) {
		if (nt!=NodesNF1S<State>::get_type_S()) throw Wrong_Node_Type();
		if (NodesNF1S<State>::if_ID(id)) {
			nodeset[id].set_state_T(s,t);
			return true;
		}
		else return false;
	};

	// Write the nodes info.
	void write_T(std::ostream &of, TimeType t) const {
		for (NodeID id=0;id<nodeset.size();id++) {
			write_ID_T(of,id,t);
			of << '\n';
		};
	};
	void write_ID_T(std::ostream &of, NodeID id, TimeType t) const {
		if (NodesNF1S<State>::if_ID(id) && nodeset[id].range(t)) of << nodeset[id].access_state_T(t);
	};

	// Get the RNG of the given node (if the node has RNG, i.e. derived from NodeR).
	RNG_Limit & access_rng(NodeID id, NodeType nt) {
		if (NodesNF1S<State>::if_type(nt)) return nodeset[id].access_rng();
		else throw Bad_Node_Type();
	};

   protected:
	std::vector<NodeVR<State> > nodeset;
};

// Example3:
// Nodes that have only one type with the varying state (NodeV<State>) with the fixed number (state, without RNG)
template<class State> class NodesNF_V: public NodesNF1S<State> {
   public:
   	// Constructor.
   	NodesNF_V(TimeType &t, NodeID nnodes=0, DelayType md=1, NodeType nt=0): NodesNF1S<State>(t,md,nt), nodeset() {
		for (NodeID id=0;id<nnodes;id++) add_node(id,nt,NodesNF1S<State>::time);
	};

	// Virtual destructor.
	~NodesNF_V() {};
	
   	// Delete all nodes now.
	void clear() {
		NodesNF::clear();
		nodeset.clear();
	};
	
	// Advancing the time of all current node objects upto the given time without change (if needed).
	void advance(TimeType t) {
		if (t<NodesNF1S<State>::time) return;
		for (NodeID id=0;id<nodeset.size();id++) {
			if (nodeset[id].get_last_time()<t) { // outside the range of the time sequence
				while (nodeset[id].get_last_time()<t)
					nodeset[id].advance_without_change();
			}
			else { 	// If the time is still inside the range of the time sequence.
				// Assumes that time advances with 1 and 
				// that the state value is kept from the previous valule.
				nodeset[id].set_state_T(nodeset[id].access_state_T(t-1),t);
			};
		};
	};
	void advance_N(NodeType nt, TimeType t) {
		if (this->get_type_S()==nt) this->advance(t);
	};

	// Add a node 
	bool add_node_S(NodeID id, NodeType nt, TimeType t, const State &s) {
		if (nt!=NodesNF1S<State>::get_type_S()) throw Wrong_Node_Type();
		if (NodesNF::add_node(id,nt,t)) {
			nodeset.resize(NodesNF1S<State>::get_nnodes());
			nodeset[id]=NodeV<State>(NodesNF1S<State>::max_delay+1,s,t);
			return true;
		}
		else return false;
	};

	// Get the state of the given node.
	State get_state(NodeID id, NodeType nt, TimeType t) const {
		if (nt!=NodesNF1S<State>::get_type_S()) throw Wrong_Node_Type();
		if (NodesNF1S<State>::if_ID(id) && nodeset[id].range(t)) return nodeset[id].get_state_T(t);
		else throw Bad_Node_ID();
	};

	// Access the state of the given node.
	State & access_state(NodeID id, NodeType nt, TimeType t) {
		if (nt!=NodesNF1S<State>::get_type_S()) throw Wrong_Node_Type();
		if (!NodesNF1S<State>::if_ID(id)) throw Bad_Node_ID();
		if (nodeset[id].range(t)) return nodeset[id].access_state_T(t);
		else throw TimeSeq_Range_Error();
	};

	// Set the Node state of the given node.
	bool set_state(NodeID id, NodeType nt, TimeType t, const State &s) {
		if (nt!=NodesNF1S<State>::get_type_S()) return false;
		if (NodesNF1S<State>::if_ID(id)) {
			nodeset[id].set_state_T(s,t);
			return true;
		}
		else return false;
	};

	// Write the nodes info.
	void write_T(std::ostream &of, TimeType t) const {
		for (NodeID id=0;id<nodeset.size();id++) {
			write_ID_T(of,id,t);
			of << '\n';
		};
	};
	void write_ID_T(std::ostream &of, NodeID id, TimeType t) const {
		if (NodesNF1S<State>::if_ID(id) && nodeset[id].range(t)) of << nodeset[id].access_state_T(t);
	};

	// Get the RNG of the given node (if the node has RNG, i.e. derived from NodeR).
	RNG_Limit get_rng(NodeID id, NodeType nt) {
		throw Bad_Node_Type();
	};

   protected:
	std::vector<NodeV<State> > nodeset;
};

}; // End of namespace conet.
#endif
