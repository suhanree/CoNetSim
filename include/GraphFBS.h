// 
//	CONETSIM - GraphFBS.h
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

#ifndef GRAPHFBS_H
#define GRAPHFBS_H

#include "Graph.h"
#include "Types.h"
#include <set>
#include <map>
#include <istream>
#include <ostream>

namespace conet {

// Class for graphs (fixed # of vertices, bidirectional edges, with edge states)
class GraphFBS : public GraphF {
   public :
	// Constructor.
   	GraphFBS() : GraphF(0), gra() {};
   	GraphFBS(NodeID nv, LinkType lt=0, bool l=false, bool m=false) : GraphF(nv,lt,l,m), gra(nv) {};

	// Virtual destructor.
	~GraphFBS() {};

	// Reset the graph by setting the size of vertices (to be used to start from scratch).
	// (delete all edges if exists; for GraphV's, nodes will be created from 0 to nv-1)
	void reset(long nv) {
		this->clear_edges();
		gra.resize(nv);
		nvertices=nv;
	};

	// Show if links are directional.
	virtual bool if_directional() const {
		return false;
	};

	// Show if links have states.
	virtual bool if_link_ID() const {
		return true;
	};

	// Find the set of all current edges (only works when there are edge states, classes that ends with S).
	bool find_all_edges(LinkIDSet &edgeset) const;
	
	// Find if an edge(s) exists between two nodes (ori: origin, des: destination).
	// Returns true if existi(s), false if there is no edge.
	bool find_edge(NodeID ori, NodeID des);
	bool find_edge_ID(NodeID ori, NodeID des, LinkIDSet &edgeset) const; // only works when there are edge states, classes that ends with S.

	// Assignment function.
	bool copy(const Graph &gr);

	// Clearing all edges only. ('delete' should be used)
	void clear_edges();

	// Make this graph full.
	void make_full();

	// Remove edge(s) between ori and des (from ori to des for directional edges)
	// (Returns false if it doesn't exist or unsuccessful).
	bool remove_edge(NodeID ori, NodeID des); // For all graphs (remove all edges between two nodes).
	bool remove_edge_ID(NodeID ori, NodeID des, LinkID id); //Remove the specified edge for graphs with ID (classes that ends with S).

	// Add a given edge. 
	// Returns false if it fails (if it has a non-existing vertex, if the edge
	// 	already exists (when multilink=false), etc), true if successful.
	// When adding an edge with edge states, the default value will be given.
	bool add_edge(NodeID ori, NodeID des); // For graphs without edge states
	bool add_edge_ID(NodeID ori, NodeID des, LinkID e1); // For graphs with edge states

	// Find the vertices that are connected to a given vertex. (Return the number
	// 	of vertices or -1 if the vertex doesn't belong to the graph.)
	// If neighbor=true, find the true neighbors (multilink and loop ignored).
	// All 3 funcions are overloaeded for NodeIDSSet and NodeIDLinkIDMMap.
	long find_dep(NodeID v, NodeIDMSet &deps, bool neighbor=false) const;
	long find_in_dep(NodeID v, NodeIDMSet &deps, bool neighbor=false) const {
		return this->find_dep(v,deps,neighbor);
	};
	long find_out_dep(NodeID v, NodeIDMSet &deps, bool neighbor=false) const {
		return this->find_dep(v,deps,neighbor);
	};

	long find_dep2(NodeID v, NodeIDSSet &deps) const;
	long find_in_dep2(NodeID v, NodeIDSSet &deps) const {
		return this->find_dep(v,deps);
	};
	long find_out_dep2(NodeID v, NodeIDSSet &deps) const {
		return this->find_dep(v,deps);
	};

	long find_dep_ID(NodeID v, NodeIDLinkIDMMap &deps) const;	//For classes that ends with S only.
	long find_in_dep_ID(NodeID v, NodeIDLinkIDMMap &deps) const {	//For classes that ends with S only.
		return this->find_dep(v,deps,neighbor);
	};
	long find_out_dep_ID(NodeID v, NodeIDLinkIDMMap &deps) const {//For classes that ends with S only.
		return this->find_dep(v,deps,neighbor);
	};

	// Find degree information.
	// Find the number of degrees for a given vertex. 
	// Returns -1 if the vertex doesn't exist.
	// If neighbor=true, find the number of neighbors (multilink and loop ignored).
	long find_degree(NodeID v, bool neighbor=false) const;
	long find_in_deg(NodeID v, bool neighbor=false) const {
		return find_degree(v,neighbor);
	};
	long find_out_deg(NodeID v, bool neighbor=false) const {
		return find_degree(v,neighbor);
	};

	// Find the number of edges between neighbors.
	// Returns the number of edges and number of neighbors through argument.
	long find_neighbor_connections(NodeID id, long &degree) const;
	long find_in_neighbor_connections(NodeID id, long &degree) const {
		return this->find_neighbor_connections(id,degree);
	};
	long find_out_neighbor_connections(NodeID id, long &degree) const {
		return this->find_neighbor_connections(id,degree);
	};

	// Find other statistical quantities.
	double find_degree_correlation(bool neighbor=false) const;

	// Print the graph showing all vertices and edges.
	// 	print  : show vertices and edges separately.
	// 	write1 : format: (Time) Ori Des (ID), (show all the edges each in a line)
	// 	write2 : format: (Time) Des Ori1 Ori2... (show all vertices with connected nodes)
	//	write3 : Pajek .net format
	//	write_edges : edges only for Pajek .net format
	void print() const;	// To stdout.
	void write1(std::ostream &of, long time=-1) const;	// To a file (when time<0, time will not be written)
	void write2(std::ostream &of, long time=-1) const;	// To a file (when time<0, time will not be written)
	void write3(std::ostream &of) const;	// To a file.
	void write_edges(std::ostream &of) const;	// To a file.
	void write_degrees(std::ostream &of) const;	// To a file.

	// Read the graph structure info from a file (# of vertices are assumed to be known).
	// read1: only reads (ori, des) from each line (for graphs without edge state).
	bool read1(std::istream &ifile) {
		return false;
	};
	// read2: only reads (ori, des, id) from each line (for graphs with edge states).
	bool read2(std::istream &ifile);

   protected: 
	std::vector<NodeIDLinkIDMMap> gra;

};

   	
}; // End of namespace conet.
#endif
