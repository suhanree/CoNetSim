// 
//	CONETSIM - Graphs.h
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


#ifndef GRAPHS_H
#define GRAPHS_H

#include "Graph.h"
#include "GraphFB.h"
#include "GraphFD.h"
#include "GraphFDS.h"
#include "Types.h"
#include "TimeSeq.h"
#include "Errors.h"

#include <set>
#include <map>
#include <vector>
#include <istream>
#include <ostream>
#include <iostream>

namespace conet {

// Abstract class that represents the set of graphs.
// When the graphs are varying, time sequence of pointers of each Graph objects (TimeSeqGraphs) are used. 
// Since this class only has pointers, subclasses of this class should add real graph objects as their data member.
class Graphs {
   public:
   	// Constructors.
   	Graphs() {};

	// Destructors.
	virtual ~Graphs() {};

   protected:
	// Assign graph (constructed elsewhere) to a given link type, and set the time as the current time.
	// A given type has (max_delay+1) pointers, and they will be set with graphs given.
	// (to be used in constructors for subclasses of varying graphs only).
	virtual bool assign_graph_V(LinkType lt, const std::vector<Graph *> & grs, DelayType md=1) {
		return false;
	};

	// Assign graph (constructed elsewhere) to all fixed graphs.
	// (to be used in constructors for subclasses of fixed graphs only).
	virtual bool assign_graph_F(LinkType lt, Graph *pg) {
		return false;
	};

   public:
   	// Get the number of links.
	virtual LinkID get_nlinks() const=0;
	virtual LinkID get_nlinks_type(LinkType lt) const=0;
		
   	// Check if the given type exists.
	bool if_type(LinkType lt) const {
		if (this->if_fixed_type(lt) || this->if_varying_type(lt)) return true;
		else return false;
	};
	virtual bool if_fixed_type(LinkType lt) const {
		return false;
	};
	virtual bool if_varying_type(LinkType lt) const {
		return false;
	};

	// Get the number of types
	long get_ntypes() const {
		return this->get_n_fixed_types()+this->get_n_varying_types();
	};
	virtual long get_n_fixed_types() const {
		return 0;
	};
	virtual long get_n_varying_types() const {
		return 0;
	};

	// Get the time.
	virtual TimeType get_time() const {
		return -1;
	};

	// Get the graph of given type at the given time. (Note that the graph is returned as a pointer.)
	virtual Graph * get_graph_pointer(LinkType lt, TimeType t)=0;
	
	// Access the graph of given type at the given time. (Note that the graph is returned as a reference.)
	Graph & access_graph(LinkType lt, TimeType t) {
		return *(this->get_graph_pointer(lt,t));
	};

	// Remove a link. (only works when t is in range or t==last_time+1 for varying graphs)
	virtual bool remove_link_noID(LinkType lt, TimeType t, NodeID ori, NodeID des)=0;
	virtual bool remove_link(LinkID id, LinkType lt, TimeType t, NodeID ori, NodeID des)=0;

	// Add a link. (only works when t is in range or t==last_time+1 for varying graphs)
	virtual bool add_link_noID(LinkType lt, TimeType t, NodeID ori, NodeID des)=0;
	virtual bool add_link(LinkID id, LinkType lt, TimeType t, NodeID ori, NodeID des)=0;

	// Update the graph (only works when t is in range or t==last_time+1 for varying graphs)
	virtual bool update_graph(LinkType lt, TimeType t, const Graph &gr)=0;

	// Advance upto the given time (the given time t should be greater than time) (for varying graphs only).
	// 	if_copy=true: copy the previous value. 
	// 	if_copy=false: new value(s) will be null.
	virtual void advance(TimeType t, bool if_copy=true)=0; // For all graphs.
	virtual void advance_L(LinkType lt, TimeType t, bool if_copy=true)=0; // For LinkType=lt only.
	
	// Write the graphs.
	virtual void write(std::ostream &of)=0;	// For all time sequence of graphs it contains.
	virtual void write_L(std::ostream &of, LinkType lt)=0; 	// For a specific LinkType and all time.
	virtual void write_T(std::ostream &of, TimeType t)=0; 	// For all LinkType's and a specific time.
	virtual void write_LT(std::ostream &of, LinkType lt, TimeType t)=0; // For a specific LinkType and a specific time.
};

// Class that represents the set of graphs that are all fixed.
// One graph will be needed for each type. (Pointer will be used for graphs.)
// Subclasses can add real graph objects as the data member.
// Note that the constructor of the virtual base (Graphs) should be called from the most derived class.
class GraphsFixed: virtual public Graphs {
   public:
   	// Constructors.
   	GraphsFixed(): fgraphs() {};

	// Destructors.
	~GraphsFixed() {};

   protected:
	// Assign graphs (constructed elsewhere) to pointers this class use.
	// Only to be used in constructors of subclasses.
	bool assign_graph_F(LinkType lt, Graph * pg) {
		if (if_type(lt)) return false; // If the type already exists. return false.
		fgraphs[lt]=pg;
		return true;
	};

   public:
   	// Get the number of links.
	LinkID get_nlinks() const {
		LinkID sum=0;
		for (std::map<LinkType,Graph *>::const_iterator i=fgraphs.begin();i!=fgraphs.end();i++)
			sum+=(i->second)->get_link_size();
		return sum;
	};

	LinkID get_nlinks_type(LinkType lt) const {
		if (if_fixed_type(lt)) return ((fgraphs.find(lt))->second)->get_link_size();
		else throw Bad_Link_Type();
	};

   	// Check if the given type exists.
	bool if_fixed_type(LinkType lt) const {
		if (fgraphs.find(lt)!=fgraphs.end()) return true;
		else return false;
	};

	// Get the number of types
	long get_n_fixed_types() const {
		return fgraphs.size();
	};

	// Get the graph of given type at the given time. (Note that the graph is returned as a pointer.)
	Graph * get_graph_pointer(LinkType lt, TimeType t) {
		if (if_fixed_type(lt)) return fgraphs[lt];
		else throw Bad_Link_Type();
	};

	// Remove a link. (Can be done only at t=0)
	bool remove_link_noID(LinkType lt, TimeType t, NodeID ori, NodeID des) {
		if (t==0) {
			if (!if_fixed_type(lt)) throw Bad_Link_Type();
			else return fgraphs[lt]->remove_edge(ori,des);
		}
		else return false;
	};
	bool remove_link(LinkID id, LinkType lt, TimeType t, NodeID ori, NodeID des) {
		if (t==0) {
			if (!if_fixed_type(lt)) throw Bad_Link_Type();
			else return fgraphs[lt]->remove_edge_ID(ori,des,id);
		}
		else return false;
	};

	// Add a link. (Can be done only at t=0)
	bool add_link_noID(LinkType lt, TimeType t, NodeID ori, NodeID des) {
		if (t==0) {
			if (!if_fixed_type(lt)) throw Bad_Link_Type();
			else return fgraphs[lt]->add_edge(ori,des);
		}
		else return false;
	};
	bool add_link(LinkID id, LinkType lt, TimeType t, NodeID ori, NodeID des) {
		if (t==0) {
			if (!if_fixed_type(lt)) throw Bad_Link_Type();
			else return fgraphs[lt]->add_edge_ID(ori,des,id);
		}
		else return false;
	};

	// Update the graph. (Can be done only at t=0)
	bool update_graph(LinkType lt, TimeType t, const Graph &gr) {
		if (t==0) {
			if (!if_fixed_type(lt)) throw Bad_Link_Type();
			else return fgraphs[lt]->copy(gr);
		}
		else return false;
	};

	// Advance without change (not needed).
	void advance(TimeType t, bool if_copy=true) {};
	void advance_L(LinkType lt, TimeType t, bool if_copy=true) {};
	
	// Write the graphs.
	void write(std::ostream &of) {
		for (std::map<LinkType,Graph *>::const_iterator i=fgraphs.begin();i!=fgraphs.end();i++)
			this->write_L(of,i->first);
	};
	void write_L(std::ostream &of, LinkType lt) {
		if (!if_fixed_type(lt)) throw Bad_Link_Type();
		of << "LinkType " << lt << '\n';
		fgraphs[lt]->write1(of);
	};
	void write_T(std::ostream &of, TimeType t) {
		this->write(of);
	};
	void write_LT(std::ostream &of, LinkType lt, TimeType t) {
		this->write_L(of,lt);
	};

   protected:
   	// Set of graphs, each graph does not change.
   	std::map<LinkType,Graph *> fgraphs;
};

// Class that represents the set of graphs that are all varying.
// Time sequences of pointers for each Graph (TimeSeqGraphs) are used. 
// Since this class only has pointers, subclasses of this class should add real graph objects as their data member,
//	and let the pointers of TimeSeqGraphs object point to those graphs.
// Note that the constructor of the virtual base (Graphs) should be called from the most derived class.
class GraphsVarying: virtual public Graphs {
   public:
   	// Constructors.
   	GraphsVarying(TimeType &t): vgraphs(), time(t) {};

	// Destructors.
	~GraphsVarying() {};

   protected:
	// Assign graphs (constructed elsewhere) to pointers this class use.
	bool assign_graph_V(LinkType lt, const std::vector<Graph *> &grs, DelayType md=1) {
		if (if_type(lt)) return false; // If the type already exists. return false.
		if (grs.size()!=md+1) return false; //Sizes should match.
		else {
			vgraphs[lt]=TimeSeqGraphs(grs,time); // time is assumed to be 0.
			return true;
		};
	};

   public:
   	// Get the number of links.
	LinkID get_nlinks() const {
		LinkID sum=0;
		for (std::map<LinkType,TimeSeqGraphs>::const_iterator i=vgraphs.begin();i!=vgraphs.end();i++)
			sum+=((i->second).get_graph_pointer(time))->get_link_size();
		return sum;
	};

	LinkID get_nlinks_type(LinkType lt) const {
		if (if_varying_type(lt)) return ((vgraphs.find(lt))->second).get_graph_pointer(time)->get_link_size();
		else throw Bad_Link_Type();
	};

   	// Check if the given type exists.
	bool if_varying_type(LinkType lt) const {
		if (vgraphs.find(lt)!=vgraphs.end()) return true;
		else return false;
	};

	// Get the number of types
	long get_n_varying_types() const {
		return vgraphs.size();
	};

	// Get the time.
	TimeType get_time() const {
		return time;
	};

	// Get the graph of given type at the given time. (Note that the graph is returned as a pointer.)
	Graph * get_graph_pointer(LinkType lt, TimeType t) {
		if (!if_varying_type(lt)) throw Bad_Link_Type();
		return vgraphs[lt].get_graph_pointer(t);
	};

	// Remove a link. (only works when t is in range or t==last_time+1)
	bool remove_link_noID(LinkType lt, TimeType t, NodeID ori, NodeID des) {
		if (!if_varying_type(lt)) throw Bad_Link_Type();
		return vgraphs[lt].remove_link(t,ori,des);
	};
	bool remove_link(LinkID id, LinkType lt, TimeType t, NodeID ori, NodeID des) {
		if (!if_varying_type(lt)) throw Bad_Link_Type();
		return vgraphs[lt].remove_link_ID(t,id,ori,des);
	};

	// Add a link. (only works when t is in range or t==last_time+1)
	bool add_link_noID(LinkType lt, TimeType t, NodeID ori, NodeID des) {
		if (!if_varying_type(lt)) throw Bad_Link_Type();
		return vgraphs[lt].add_link(t,ori,des);
	};
	bool add_link(LinkID id, LinkType lt, TimeType t, NodeID ori, NodeID des) {
		if (!if_varying_type(lt)) throw Bad_Link_Type();
		return vgraphs[lt].add_link_ID(t,ori,des,id);
	};

	// Update the graph (only works when t is in range or t==last_time+1)
	bool update_graph(LinkType lt, TimeType t, const Graph &gr) {
		if (!if_varying_type(lt)) throw Bad_Link_Type();
		vgraphs[lt].add_value(t,gr);
		return true;
	};

	// Advance without change upto the given time if some graphs are not already updated (for varying graphs only).
	void advance(TimeType t, bool if_copy=true) {
		for (std::map<LinkType,TimeSeqGraphs>::iterator i=vgraphs.begin();i!=vgraphs.end();i++)
			advance_L(i->first,t,if_copy);
	};
	void advance_L(LinkType lt, TimeType t, bool if_copy=true) {
		if (!if_varying_type(lt)) throw Bad_Link_Type();
		if (time>t) return; // If t is less than time, do nothing.
		if (vgraphs[lt].get_last_time()>=t) {
			for (TimeType tt=time;tt<=t;tt++) {
				if (if_copy) vgraphs[lt].add_value(tt,vgraphs[lt].access_graph(tt-1)); 
				else vgraphs[lt].access_graph(tt).clear_edges(); 
			};
		}
		else
			while (vgraphs[lt].get_last_time()<t) {
				if (if_copy) vgraphs[lt].advance_without_change();
				else vgraphs[lt].advance_with_null();
			};
	};
	
	// Write the graphs.
	void write(std::ostream &of) {
		for (std::map<LinkType,TimeSeqGraphs>::const_iterator i=vgraphs.begin();i!=vgraphs.end();i++)
			for (TimeType tt=(i->second).get_first_time();tt<=(i->second).get_last_time();tt++)
				write_LT(of,i->first,tt);
	};
	void write_L(std::ostream &of, LinkType lt) {
		if (!if_varying_type(lt)) throw Bad_Link_Type();
		for (TimeType tt=vgraphs[lt].get_first_time();tt<=vgraphs[lt].get_last_time();tt++)
			write_LT(of,lt,tt);
	};
	void write_T(std::ostream &of, TimeType t) {
		for (std::map<LinkType,TimeSeqGraphs>::const_iterator i=vgraphs.begin();i!=vgraphs.end();i++)
			write_LT(of,i->first,t);
	};
	void write_LT(std::ostream &of, LinkType lt, TimeType t) {
		if (!if_varying_type(lt)) throw Bad_Link_Type();
		of << "LinkType " << lt << " at Time " << t << '\n';
		(vgraphs[lt].get_graph_pointer(t))->write1(of,t);
	};

   protected:
   	// Set of graphs, each graph has a time sequence of objects.
   	std::map<LinkType,TimeSeqGraphs> vgraphs;

	// Reference for the current time.
	const TimeType &time;
};

// Graphs object that contains both fixed and varying graphs.
class GraphsFixedVarying: public GraphsFixed, public GraphsVarying {
   public:
   	// Constructor.
   	GraphsFixedVarying(TimeType &t): GraphsVarying(t) {};

	// Destructor.
	~GraphsFixedVarying() {};

   	// Get the number of links.
	LinkID get_nlinks() const {
		return GraphsFixed::get_nlinks()+GraphsVarying::get_nlinks();
	};

	LinkID get_nlinks_type(LinkType lt) const {
		if (if_fixed_type(lt)) return GraphsFixed::get_nlinks_type(lt);
		if (if_varying_type(lt)) return GraphsVarying::get_nlinks_type(lt);
		throw Bad_Link_Type();
	};

	// Get the graph of given type at the given time.
	Graph * get_graph_pointer(LinkType lt, TimeType t) {
		if (if_fixed_type(lt)) return GraphsFixed::get_graph_pointer(lt,t);
		if (if_varying_type(lt)) return GraphsVarying::get_graph_pointer(lt,t);
		else throw Bad_Link_Type();
	};

	// Remove a link. (Can be done only at t=0)
	bool remove_link_noID(LinkType lt, TimeType t, NodeID ori, NodeID des) {
		if (if_fixed_type(lt)) return GraphsFixed::remove_link_noID(lt,t,ori,des);
		if (if_varying_type(lt)) return GraphsVarying::remove_link_noID(lt,t,ori,des);
		else throw Bad_Link_Type();
	};
	bool remove_link(LinkID id, LinkType lt, TimeType t, NodeID ori, NodeID des) {
		if (if_fixed_type(lt)) return GraphsFixed::remove_link(id,lt,t,ori,des);
		if (if_varying_type(lt)) return GraphsVarying::remove_link(id,lt,t,ori,des);
		else throw Bad_Link_Type();
	};

	// Add a link. (Can be done only at t=0)
	bool add_link_noID(LinkType lt, TimeType t, NodeID ori, NodeID des) {
		if (if_fixed_type(lt)) return GraphsFixed::add_link_noID(lt,t,ori,des);
		if (if_varying_type(lt)) return GraphsVarying::add_link_noID(lt,t,ori,des);
		else throw Bad_Link_Type();
	};
	bool add_link(LinkID id, LinkType lt, TimeType t, NodeID ori, NodeID des) {
		if (if_fixed_type(lt)) return GraphsFixed::add_link(id,lt,t,ori,des);
		if (if_varying_type(lt)) return GraphsVarying::add_link(id,lt,t,ori,des);
		else throw Bad_Link_Type();
	};

	// Update the graph.
	bool update_graph(LinkType lt, TimeType t, const Graph &gr) {
		if (if_fixed_type(lt)) return GraphsFixed::update_graph(lt,t,gr);
		if (if_varying_type(lt)) return GraphsVarying::update_graph(lt,t,gr);
		else throw Bad_Link_Type();
	};

	// Advance without change (for varying graphs only) (for all links if needed).
	void advance(TimeType t, bool if_copy=true) {
		GraphsFixed::advance(t,if_copy); // will not do anything actually
		GraphsVarying::advance(t,if_copy);
	};
	void advance_L(LinkType lt, TimeType t, bool if_copy=true) {
		GraphsFixed::advance_L(lt,t,if_copy); // will not do anything actually
		GraphsVarying::advance_L(lt,t,if_copy);
	};
	
	// Write the graphs.
	void write(std::ostream &of) {
		GraphsFixed::write(of);
		GraphsVarying::write(of);
	};
	void write_L(std::ostream &of, LinkType lt) {
		if (if_fixed_type(lt)) GraphsFixed::write_L(of,lt);
		if (if_varying_type(lt)) GraphsVarying::write_L(of,lt);
	};
	void write_T(std::ostream &of, TimeType t) {
		GraphsFixed::write_T(of,t);
		GraphsVarying::write_T(of,t);
	};
	void write_LT(std::ostream &of, LinkType lt, TimeType t) {
		if (if_fixed_type(lt)) GraphsFixed::write_LT(of,lt,t);
		if (if_varying_type(lt)) GraphsVarying::write_LT(of,lt,t);
	};
};

// Example:
// Class with two link types (i.e. two Graph's: GraphFB & GraphFD) with max_delay=1 for both graphs.
// Both graphs are varying; while one is bidirectional (LinkType=0), and the other is directional (LinkType=1).
class Graphs_Ex0: public GraphsVarying {
   public:
   	// Constructor.
	// nv: number of nodes, if_loop & if_multilink: for the directional graph (LinkType=1).
   	Graphs_Ex0(NodeID nv, TimeType &t, bool if_multilink=false, bool if_loop=false): GraphsVarying(t), g1(2), g2(2) { 
		std::vector<Graph *> pg1(2), pg2(2);
		for (long i=0;i<2;i++) {
			g1[i].reset(nv);
			g2[i].reset(nv);
			g2[i].set_multi_link(if_multilink);
			g2[i].set_loop(if_loop);
			g1[i].set_edge_type(0);
			g2[i].set_edge_type(1);
			pg1[i]=&g1[i];
			pg2[i]=&g2[i];
		};
		GraphsVarying::assign_graph_V(0,pg1,1); // LinkType=0
		GraphsVarying::assign_graph_V(1,pg2,1); // LinkType=1
	};

	// Destructor.
	~Graphs_Ex0() {};

	// Only dealing with ID-less links.
	bool add_link(LinkID id, LinkType lt, TimeType t, NodeID ori, NodeID des) {
		return false;
	};
	bool remove_link(LinkID id, LinkType lt, TimeType t, NodeID ori, NodeID des) {
		return false;
	};

   private:
   	std::vector<GraphFB> g1;
	std::vector<GraphFD> g2;
};

// Example:
// Class with two link types (i.e. two Graph's: GraphFB & GraphFB) with max_delay=1 for both graphs.
class Graphs_Ex1: public GraphsVarying {
   public:
   	// Constructor.
	// nv: number of nodes, if_loop & if_multilink: for the directional graph (LinkType=1).
   	Graphs_Ex1(NodeID nv, TimeType &t, bool if_multilink=false, bool if_loop=false): GraphsVarying(t), g1(2), g2(2) { 
		std::vector<Graph *> pg1(2), pg2(2);
		for (long i=0;i<2;i++) {
			g1[i].reset(nv);
			g2[i].reset(nv);
			g2[i].set_multi_link(if_multilink);
			g2[i].set_loop(if_loop);
			g1[i].set_edge_type(0);
			g2[i].set_edge_type(1);
			pg1[i]=&g1[i];
			pg2[i]=&g2[i];
		};
		GraphsVarying::assign_graph_V(0,pg1,1); // LinkType=0
		GraphsVarying::assign_graph_V(1,pg2,1); // LinkType=1
	};

	// Destructor.
	~Graphs_Ex1() {};

	// Only dealing with ID-less links.
	bool add_link(LinkID id, LinkType lt, TimeType t, NodeID ori, NodeID des) {
		return false;
	};
	bool remove_link(LinkID id, LinkType lt, TimeType t, NodeID ori, NodeID des) {
		return false;
	};

   private:
   	std::vector<GraphFB> g1;
	std::vector<GraphFB> g2;
};

// Example:
// Class with one link type (GraphFDS) with states and inhomogeneous delays (loop=true, multi_link=true) with changing link structure.
class Graphs_Ex2: public GraphsVarying {
   public:
   	// Constructor.
   	Graphs_Ex2(NodeID nv, DelayType md, TimeType &t): GraphsVarying(t), grs(md+1) { // nv: number of nodes.
		std::vector<Graph *> pgrs(md+1);
		for (long i=0;i<md+1;i++) {
			grs[i].reset(nv);
			grs[i].set_edge_type(0);
			grs[i].set_loop(true);
			grs[i].set_multi_link(true);
			pgrs[i]=&grs[i];
		};
		GraphsVarying::assign_graph_V(0,pgrs,md); // LinkType=0
	};

	// Destructor.
	~Graphs_Ex2() {};

	// Only dealing with ID-ed links.
	bool add_link_noID(LinkType lt, TimeType t, NodeID ori, NodeID des) {
		return false;
	};
	bool remove_link_noID(LinkType lt, TimeType t, NodeID ori, NodeID des) {
		return false;
	};

   private:
   	std::vector<GraphFDS> grs;
};

// Example:
// Class with one link type (GraphFDS) with states and inhomogeneous delays (loop=true, multi_link=true),
// 	Assuming the link structure does not change with times (Then the time sequence not needed).
class Graphs_Ex3: public GraphsFixed {
   public:
   	// Constructor.
   	Graphs_Ex3(NodeID nv): GraphsFixed(), gr(nv,0,true,true) { // nv: number of nodes. LinkType=0
		GraphsFixed::assign_graph_F(0,&gr);
	};

	// Destructor.
	~Graphs_Ex3() {};

	// Only dealing with ID-ed links.
	bool add_link_noID(LinkType lt, TimeType t, NodeID ori, NodeID des) {
		return false;
	};
	bool remove_link_noID(LinkType lt, TimeType t, NodeID ori, NodeID des) {
		return false;
	};

   private:
   	GraphFDS gr;
};

// Example:
// Class with two link types (i.e. two Graph's: GraphFB & GraphFB) with max_delay=1 for both graphs (loop=false, multi_link=false).
// One graph is fixed (LinkType=1), and the other is varying (LinkType=0) (both bidirectional)
class Graphs_Ex4: public GraphsFixedVarying {
   public:
   	// Constructor.
   	Graphs_Ex4(NodeID nv, TimeType &t): GraphsFixedVarying(t), g1s(2), g2(nv) { // nv: number of nodes. g2 (LinkType=1): fixed, g1s (LinkType=0): varying)
		std::vector<Graph *> pg1s(2);
		for (long i=0;i<2;i++) {
			g1s[i].reset(nv);
			g1s[i].set_edge_type(0);
			pg1s[i]=&g1s[i];
		};
		GraphsVarying::assign_graph_V(0,pg1s,1);
		g2.set_edge_type(1);
		GraphsFixed::assign_graph_F(1,&g2);
	};

	// Destructor.
	~Graphs_Ex4() {};

	// Only dealing with ID-less links.
	bool add_link(LinkID id, LinkType lt, TimeType t, NodeID ori, NodeID des) {
		return false;
	};
	bool remove_link(LinkID id, LinkType lt, TimeType t, NodeID ori, NodeID des) {
		return false;
	};

   private:
	std::vector<GraphFB> g1s;	// To be used for the time sequence of the varying graph.
   	GraphFB g2; // To be used for the fixed graph.
};

// Example:
// Class with two link types (i.e. two Graph's: GraphFB & GraphFD) with max_delay=1 for both graphs (loop=false, multi_link=false).
// One graph is varying (bidirectional, LinkType=0), and the other is fixed (directional, LinkType=1).
class Graphs_Ex5: public GraphsFixedVarying {
   public:
   	// Constructor.
   	Graphs_Ex5(NodeID nv, TimeType &t): GraphsFixedVarying(t), g1s(2), g2(nv) { // nv: number of nodes.
		std::vector<Graph *> pg1s(2);
		for (long i=0;i<2;i++) {
			g1s[i].reset(nv);
			g1s[i].set_edge_type(0);
			pg1s[i]=&g1s[i];
		};
		GraphsVarying::assign_graph_V(0,pg1s,1);
		g2.set_edge_type(1);
		GraphsFixed::assign_graph_F(1,&g2);
	};

	// Destructor.
	~Graphs_Ex5() {};

	// Only dealing with ID-less links.
	bool add_link(LinkID id, LinkType lt, TimeType t, NodeID ori, NodeID des) {
		return false;
	};
	bool remove_link(LinkID id, LinkType lt, TimeType t, NodeID ori, NodeID des) {
		return false;
	};

   private:
	std::vector<GraphFB> g1s;	// To be used for the time sequence of the varying graph (bidirectional).
   	GraphFD g2; // To be used for the fixed graph (directional)
};


}; // End of namespace conet.
#endif
