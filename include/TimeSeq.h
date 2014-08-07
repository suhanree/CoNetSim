// 
//	CONETSIM - TimeSeq.h
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

#ifndef TIMESEQ_H
#define TIMESEQ_H

#include "Types.h"
#include "Graph.h"
#include "Errors.h"

#include <iostream>
#include <ostream>
#include <vector>

namespace conet {

// TimeSeq template class.
// 	Will be used to store recent sequence of a certain size (N)
// 	Times: first_time, first_time+1, ..., first_time+N-1 (first_time can increase)
//	If we add a value at t=first_time+N, it will contain data at (first_time+1, first_time+2, ..., first_time+N)
// 	To make the structure efficient when retrieving the data, simple C-style array has been used.
//
// 	(State class should have operator= and operator<< overloaded)
//
template<class State> class TimeSeq {
   public:
	// Constructors.
	TimeSeq(): size(2), first_time(0), first_index(0) {
		seq=new State [size];
	};
	TimeSeq(long s, const State &val, TimeType ft): size(s), first_time(ft), first_index(0) {
		seq=new State [size];
		for (long i=0;i<size;i++) seq[i]=val;
	};
	TimeSeq(const TimeSeq<State> &r) {
  		size=r.get_size();
		first_index=0;
		first_time=r.get_first_time();
		seq=new State [size];
		for (long i=0;i<size;i++)
	  		seq[i]=r(first_time+i);
	};
	
	// Destructor
	~TimeSeq() {
		delete [] seq;
	};

	// Get functions.
	long get_size() const {
		return size;
	};
	TimeType get_first_time() const {
		return first_time;
	};
	TimeType get_last_time() const {
		return first_time+size-1;
	};
	long get_first_index() const {
		return first_index;
	};
	State get_first_value() const {
		return seq[get_first_index()];
	};
	State get_last_value() const {
		return seq[(get_first_index()+size-1)%size];
	};

	// return the index when t is given
	long get_index(TimeType t) const {
		if (range(t))  
			return (first_index+t-first_time)%size;
		else {
			TimeSeq_Range_Error er;
			throw er;
		};
	};

	// Range check
	bool range(TimeType t) const {
		return (t>=first_time && t<first_time+size);
	};

	// Set the first_time (everything else is the same as before; can be used right after the default initialization).
	void set_first_time(TimeType ft) {
		first_time=ft;
	};

	// Reset the size with new size and the same first_time (can be used right after the default initialization).
	void reset(long size1) {
		delete [] seq;
		size=size1;
		first_index=0;
		seq=new State [size];
		for (long i=0;i<size;i++)
	  		seq[i]=State();
	};
	// Reset the size with new size and first_time.
	void reset(long size1, TimeType first_time1) {
		delete [] seq;
		size=size1;
		first_index=0;
		first_time=first_time1;
		seq=new State [size];
		for (long i=0;i<size;i++)
	  		seq[i]=State();
	};

	// To add a value
	// 	when t is in range, replace the existing value
	// 	when t >= first_time+size, value is added at t (there is no existing value).
	// 		if t is not first_time+size, in-between values are assumed to be 0.
	// 	when otherwise, an error is thrown.
	void add_value(TimeType t, const State &val) {
		if (t<first_time) {
			TimeSeq_Range_Error er;
			throw er;
		}
		else if (t<first_time+size)
			seq[(first_index+t-first_time)%size]=val;
		else if (t<first_time+2*size) {
			long index_t=(first_index+t-first_time)%size;
			if (index_t-first_index>0)
				for (long i=first_index;i<index_t;i++)
					seq[i]=State();
			else if (index_t-first_index<0) {
				for (long i=first_index;i<size;i++)
					seq[i]=State();
				for (long i=0;i<index_t;i++)
					seq[i]=State();
			};	
			first_time=t-size+1;
			first_index=(index_t+1)%size;
			seq[index_t]=val;
		}
		else {
			for (long i=0;i<size-1;i++) seq[i]=State();
			first_index=0;
			first_time=t-size+1;
			seq[size-1]=val;
		};
	};
	void replace_first(const State &val) {
		seq[first_index]=val;
	};
	
	// Used when time progresses with the given value or without any change
	void advance_with_this_value(const State &st) {
		long temp_index=first_index;
		first_time++;
		first_index=(first_index+1)%size;
		seq[temp_index]=st;
	};
	void advance_without_change() {
		long temp_index=first_index;
		State temp_value=get_last_value();
		first_time++;
		first_index=(first_index+1)%size;
		seq[temp_index]=temp_value;
	};

	// Easy way to retrieve a value using t.
	// It returns the reference.
	State & operator() (TimeType t) const {
		return seq[get_index(t)];
	};

	// Assignment operator.
	TimeSeq<State> & operator=(const TimeSeq<State> &r1) {
  		size=r1.get_size();
		delete [] seq;
		first_index=0;
		first_time=r1.get_first_time();
		seq=new State [size];
		for (long i=0;i<size;i++)
	  		seq[i]=r1(first_time+i);
		return *this;
	};

	// Print time and value pairs to stdout.
	void print_first() const {
		std::cout << "time:\t" << first_time << std::endl;
		std::cout << seq[first_index] << std::endl;
	};
	void print_last() const {
		std::cout << "time:\t" << first_time+size-1 << std::endl;
		std::cout << seq[get_index(first_time+size-1)] << std::endl;
	};
	void print_all() const {
  		long t=first_time;
  		while (t<first_time+size) {
			std::cout << "time:\t" << t << std::endl;
			std::cout << seq[get_index(t)] << std::endl;
			t++;
		};
	};
	void write(std::ostream &of) const {
		of << first_time+size-1 << '\n';
		of << seq[get_index(first_time+size-1)] << '\n';
	};

   private:
	// size of the structure
	long size;
	// time value of the first element
	TimeType first_time;
	// index of the first element in the seq.
	long first_index;
	// data structure that contains the elements of size.
	State *seq;
};

// Time sequence for Graph classes.
class TimeSeqGraphs {
   public:
	// Constructors.
	// Default with the sequence size 2 and Graph objects should be linked using assign_graphs function.
	TimeSeqGraphs(): size(2), seq(2), first_time(0), first_index(0) {};
	// Only the size of the time sequence is given, and Graph objects should be linked using assign_graphs function.
	TimeSeqGraphs(long s, TimeType ft=0): size(s), seq(s), first_time(ft), first_index(0) {};
	// Assuming pointers in s is already pointing appropriate Graph objects. 
	TimeSeqGraphs(const std::vector<Graph *>  &s, TimeType ft=0): size(s.size()), seq(s), first_time(ft), first_index(0) {};
	
	// Destructor
	~TimeSeqGraphs() {};

	// Assign pointers of Graph objects to seq vector.
	bool assign_graphs(const std::vector<Graph *> &grs) {
		if (size!=grs.size()) return false;
		seq=grs;
		return true;
	};

	// Get functions.
	long get_size() const {
		return size;
	};
	TimeType get_first_time() const {
		return first_time;
	};
	TimeType get_last_time() const {
		return first_time+size-1;
	};
	long get_first_index() const {
		return first_index;
	};
	Graph *get_first_value() const {
		return seq[get_first_index()];
	};
	Graph *get_last_value() const {
		return seq[(get_first_index()+size-1)%size];
	};

	void clear() {
		//for (long i=0;i<seq.size();i++)
		//	delete seq[i];
		seq.clear();
	};
		
	// Reset the size with new size and first_time.
	// Pointers will become null, and Graph objects should be linked using assign_graphs function.
	void reset(long size1, TimeType first_time1=0) {
		clear();
		size=size1;
		first_index=0;
		first_time=first_time1;
		seq.resize(size);
	};

	// return the index when t is given
	long get_index(TimeType t) const {
		if (range(t))  
			return (first_index+t-first_time)%size;
		else {
			TimeSeq_Range_Error er;
			throw er;
		};
	};

	// Range check
	bool range(TimeType t) const {
		return (t>=first_time && t<first_time+size ? true : false);
	};

	// To add a value
	// 	when t is in range, replace the existing value
	// 	when t >= first_time+size, value is added at t (there is no existing value).
	// 		if t is not first_time+size, in-between values are assumed to be the same as the given value.
	// 	when otherwise, an error is thrown.
	void add_value(TimeType t, const Graph &gr) {
		if (t<first_time) {
			TimeSeq_Range_Error er;
			throw er;
		}
		else if (t<first_time+size)
			seq[(first_index+t-first_time)%size]->copy(gr);
		else if (t<first_time+2*size) {
			long index_t=(first_index+t-first_time)%size;
			if (index_t-first_index>0)
				for (long i=first_index;i<index_t;i++)
					seq[i]->copy(gr);
			else if (index_t-first_index<0) {
				for (long i=first_index;i<size;i++)
					seq[i]->copy(gr);
				for (long i=0;i<index_t;i++)
					seq[i]->copy(gr);
			};	
			first_time=t-size+1;
			first_index=(index_t+1)%size;
			seq[index_t]->copy(gr);
		}
		else {
			for (long i=0;i<size-1;i++) seq[i]->copy(gr);
			first_index=0;
			first_time=t-size+1;
			seq[size-1]->copy(gr);
		};
	};
	
	// Used when time progresses by one time step without any change
	void advance_without_change() {
		seq[first_index]->copy(*get_last_value());
		first_time++;
		first_index=(first_index+1)%size;
	};
	void advance_with_null() {
		seq[first_index]->clear_edges();
		first_time++;
		first_index=(first_index+1)%size;
	};

	// Get the Graph * for the time t.
	Graph * get_graph_pointer(TimeType t) const {
		return seq[get_index(t)];
	};
	
	// Access the Graph the time t.
	Graph & access_graph(TimeType t) const {
		return *(seq[get_index(t)]);
	};
	
	// Remove a link
	bool remove_link(TimeType t, NodeID ori, NodeID des) {
		// If t is in the range, then simply remove the edge.
		if (range(t)) {
			return seq[get_index(t)]->remove_edge(ori,des);
		}
		// If t=last_time+1 and the edge exists, then advance time without change and remove the edge.
		else if (t==get_last_time()+1 && get_last_value()->find_edge(ori,des)) {
			advance_without_change();
			return seq[get_index(t)]->remove_edge(ori,des);
		}
		else return false; // Don't do anything when t<first_time or t>last_time+1.
	};
	bool remove_link_ID(TimeType t, LinkID id, NodeID ori, NodeID des) {
		// If t is in the range, then simply remove the edge.
		if (range(t))
			return seq[get_index(t)]->remove_edge_ID(ori,des,id);
		// If t=last_time+1 and the edge exists, then advance time without change and remove the edge.
		else if (t==get_last_time()+1) {
			advance_without_change();
			return seq[get_index(t)]->remove_edge_ID(ori,des,id);
		}
		else return false; // Don't do anything when t<first_time or t>last_time+1.
	};

	// Add a link
	bool add_link(TimeType t, NodeID ori, NodeID des) {
		// If t is in the range, then simply add the edge.
		if (range(t)) {
			return seq[get_index(t)]->add_edge(ori,des);
		}
		// If t=last_time+1, then advance time without change and try to add the edge.
		else if (t==get_last_time()+1) {
			advance_without_change();
			return seq[get_index(t)]->add_edge(ori,des);
		}
		else return false; // Don't do anything when t<first_time or t>last_time+1.
	};
	bool add_link_ID(TimeType t, NodeID ori, NodeID des, LinkID id) {
		// If t is in the range, then simply add the edge.
		if (range(t))
			return seq[get_index(t)]->add_edge_ID(ori,des,id);
		// If t=last_time+1, then advance time without change and tyr to add the edge.
		else if (t==get_last_time()+1) {
			advance_without_change();
			return seq[get_index(t)]->add_edge_ID(ori,des,id);
		}
		else return false; // Don't do anything when t<first_time or t>last_time+1.
	};

	// Print time and value pairs to stdout.
	void print_first() const {
		std::cout << "time:\t" << first_time << std::endl;
		seq[first_index]->print();
	};
	void print_last() const {
		std::cout << "time:\t" << first_time+size-1 << std::endl;
		seq[get_index(first_time+size-1)]->print();
	};
	void print_all() const {
  		long t=first_time;
  		while (t<first_time+size) {
			std::cout << "time:\t" << t << std::endl;
			seq[get_index(t)]->print();
			t++;
		};
	};
	void write(std::ostream &of, TimeType t) const {
		seq[get_index(t)]->write2(of);
	};

   private:
	// size of the structure
	long size;
	// time value of the first element
	TimeType first_time;
	// index of the first element in the seq.
	long first_index;
	// data structure that contains the elements of size.
	std::vector<Graph *> seq;
};

}; // End of namespace conet.

#endif
