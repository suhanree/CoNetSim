// 
//	CONETSIM - Graph.h
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


#ifndef GRAPH_H
#define GRAPH_H

#include "Types.h"

#include <set>
#include <map>
#include <vector>
#include <istream>
#include <ostream>
#include <iostream>

namespace conet {

// An abstract base class that represents a graph (vertex=node, edge=link).
// It only contains basic operations for graphs. 
//
//
// <Distinction 1>
// A graph can have nodes that are not created/removed (derived class name has F: Fixed),
// 	or have nodes that can be removed/created (derived class name has V: Varying).
//
// <Distinction 2>
// A graph can be either directional (derived class name has D: Directional),
//	or bidirectional (derived class name has B: Bidirectional).
//
// <Distinction 3>
// A graph doesn't have to know if the nodes have states or not, but it needs
//	to know about existence of link states.
// For a given graph, if there is no property for links, AND all link delays are 1,
//	we don't need to use link states. Then to save the resources, we don't
//	use link objects. (derived class name has none)
// On the other hand, if there exist link properties OR there exist links with delays 
//	greater than 1, we need to keep the link state. Then we need to keep
//	link objects (derived class name has S: State).
//
// Using above three distinctions, we will have 8 concrete subclasses for graphs.
//			<fixed nodes>	<directional links>	<link states and ID>
// 	GraphFB			Y		N			N
// 	GraphFBS		Y		N			Y
//	GraphFD 		Y		Y			N
//	GraphFDS		Y		Y			Y
// 	GraphVB 		N		N			N
// 	GraphVBS		N		N			Y
//	GraphVD 		N		Y			N
//	GraphVDS		N		Y			Y
//
// In addition to above distinctions, there are more distinctions (given as data members):
//	1, link type: 0, 1, 2,...
//	2, multiple links possible (more than one link can exist between the same two nodes): Y/N
//	3, self-loops possible (links that connects the same node): Y/N


class Graph {
   public :
	// Constructors.
   	Graph(LinkType lt=0, bool l=false, bool m=false) 
		: nedges(0), edge_type(lt), loop(l), multi_link(m) {};
	
	// Virtual destructor.
	virtual ~Graph() {};

	// Get the size of vertices.
	virtual long get_vertex_size() const=0;

	// Reset the graph by setting the size of vertices (to be used to start from scratch).
	// (delete all edges if exists; for GraphV's, nodes will be created from 0 to nv-1)
	virtual void reset(long nv=0)=0;
	
	// Get the size of edges.
	long get_edge_size() const {
		return nedges;
	};
	long get_link_size() const {
		return get_edge_size();
	};

	// Get the type of edges (links).
	long get_edge_type() const {
		return edge_type;
	};
	long get_link_type() const {
		return get_edge_type();
	};

	// Set the type of edges (links).
	void set_edge_type(LinkType lt) {
		edge_type=lt;
	};
	void set_link_type(LinkType lt) {
		set_edge_type(lt);
	};

	// Show if the self-loop can exist. (default: false)
	bool if_loop() const {
		return loop;
	};

	// Show if multiple links can exist. (default: false)
	bool if_multi_link() const {
		return multi_link;
	};

	// Check if the graph is empty.
	bool if_empty() {
		return !nedges;
	};

	// Check if the graph is full only when multi_link=loop=false..
	bool if_full();

	// Set self-loop
	void set_loop(bool l) {
		loop=l;
	};

	// Set multi_link
	void set_multi_link(bool ml) {
		multi_link=ml;
	};

	// Get if nodes are fixed.
	virtual bool if_nodes_fixed() const=0;

	// Get if links are directional.
	virtual bool if_directional() const=0;

	// Get if links have IDs.
	virtual bool if_link_ID() const=0;

	// Find the set of all current vertices.
	virtual void find_vertices(NodeIDSSet &vs) const=0;

	// Find if the given vertex exists.
	virtual bool find_vertex(NodeID v) const=0;

	// Find the set of all current edges (only works when there are edge states, classes that ends with S).
	virtual bool find_all_edges(LinkIDSet &edgeset) const=0;
	
	// Find if an edge(s) exists between two nodes (ori: origin, des: destination).
	// Returns true if existi(s), false if there is no edge.
	virtual bool find_edge(NodeID ori, NodeID des)=0;
	virtual bool find_edge_ID(NodeID ori, NodeID des, LinkIDSet &edgeset) const=0; // only works when there are edge states, classes that ends with S.

	// Assignment function (assumes they are the same type, and
	//	returns false if they are not.)
	virtual bool copy(const Graph &gr) {
		edge_type=gr.get_edge_type();
		nedges=gr.get_edge_size();
		loop=gr.if_loop();
		multi_link=gr.if_multi_link();
		return true;
	};

	// Clearing all edges.
	virtual void clear_edges()=0;

	// Make this graph full.
	virtual void make_full()=0;

	// Remove edge(s) between ori and des (from ori to des for directional edges)
	// (Returns false if it doesn't exist or unsuccessful).
	virtual bool remove_edge(NodeID ori, NodeID des)=0; // For all graphs (remove all edges between two nodes).
	virtual bool remove_edge_ID(NodeID ori, NodeID des, LinkID id)=0; //Remove the specified edge for graphs with ID (classes that ends with S).

	// Add a given edge. 
	// Returns false if it fails (if it has a non-existing vertex, if the edge
	// 	already exists (when multilink=false), etc), true if successful.
	// When adding an edge with edge states, the default value will be given.
	virtual bool add_edge(NodeID ori, NodeID des)=0; // For graphs without edge states
	virtual bool add_edge_ID(NodeID ori, NodeID des, LinkID e1)=0; // For graphs with edge states

	// Find the vertices that are connected to a given vertex. (Return the number
	// 	of vertices or -1 if the vertex doesn't belong to the graph.)
	// If neighbor=true, find the true neighbors (multilink and loop ignored).
	// All 3 funcions are defined agagin for NodeIDSSet and NodeIDLinkIDMMap.
	virtual long find_dep(NodeID v, NodeIDMSet &deps, bool neighbor=false) const=0;
	virtual long find_in_dep(NodeID v, NodeIDMSet &deps, bool neighbor=false) const=0;
	virtual long find_out_dep(NodeID v, NodeIDMSet &deps, bool neighbor=false) const=0;

	virtual long find_dep2(NodeID v, NodeIDSSet &deps) const=0;
	virtual long find_in_dep2(NodeID v, NodeIDSSet &deps) const=0;
	virtual long find_out_dep2(NodeID v, NodeIDSSet &deps) const=0;

	virtual long find_dep_ID(NodeID v, NodeIDLinkIDMMap &deps) const=0;	//For classes that ends with S only.
	virtual long find_in_dep_ID(NodeID v, NodeIDLinkIDMMap &deps) const=0;	//For classes that ends with S only.
	virtual long find_out_dep_ID(NodeID v, NodeIDLinkIDMMap &deps) const=0;	//For classes that ends with S only.

	// Find degree information.
	// Find the number of degrees for a given vertex. 
	// Returns -1 if the vertex doesn't exist.
	// If neighbor=true, find the number of neighbors (multilink and loop ignored).
	virtual long find_degree(NodeID v, bool neighbor=false) const=0;
	virtual long find_in_deg(NodeID v, bool neighbor=false) const=0;
	virtual long find_out_deg(NodeID v, bool neighbor=false) const=0;
	
	// Find the degree sequence (not sorted). (Returns the maximun degree value)
	virtual long find_degree_sequence(std::vector<long> &degree, bool neighbor=false) const=0;
	virtual long find_in_deg_sequence(std::vector<long> &degree, bool neighbor=false) const=0;
	virtual long find_out_deg_sequence(std::vector<long> &degree, bool neighbor=false) const=0;

	// Find the degree statistics. (returns the number of vertices with k=0)
	// The size of the vector should be given beforehand.
	virtual long find_degree_stat(std::vector<long> &k_stat) const=0;
	virtual long find_in_deg_stat(std::vector<long> &k_stat) const=0;
	virtual long find_out_deg_stat(std::vector<long> &k_stat) const=0;

	// Find the number of edges between neighbors.
	// Returns the number of edges and number of neighbors through argument.
	virtual long find_neighbor_connections(NodeID id, long &degree) const=0;
	virtual long find_in_neighbor_connections(NodeID id, long &degree) const=0;
	virtual long find_out_neighbor_connections(NodeID id, long &degree) const=0;

	// Find other statistical quantities.
	// Find the clustering coefficients (in and out will be used for directed graphs).
	// Averaging over all nodes (for directed graphs, average over ins and outs).
	virtual double find_clustering_coeff() const=0;	
	virtual double find_clustering_in_coeff() const=0;
	virtual double find_clustering_out_coeff() const=0;
	// For a given node
	virtual double find_clustering_coeff_node(NodeID id) const;
	virtual double find_clustering_in_coeff_node(NodeID id) const;
	virtual double find_clustering_out_coeff_node(NodeID id) const;
	// Averaging over all nodes in a cluster.
	virtual double find_clustering_coeff_cluster(const NodeIDSSet & cluster) const;	
	virtual double find_clustering_in_coeff_cluster(const NodeIDSSet & cluster) const;
	virtual double find_clustering_out_coeff_cluster(const NodeIDSSet & cluster) const;

	// Find mean path length (distance) for all reachable pairs of nodes.
	// (If there exist non-reachable pairs, use the second function.)
	virtual double find_mean_distance(double percent_sample=100.0) const; // Assuming the whole network is one component.
	virtual double find_mean_distance_component(const NodeIDSSet & component, \
		double percent_sample=100.0) const; // Used when a cluster is known.

	// Find the degree correlation coefficient.
	virtual double find_degree_correlation(bool neighbor=false) const=0;

	// Find the components of the graph.
	// (For directed graphs, find the number of weakly connected graphs.)
	// (Returns the number of components.)
	virtual long find_components(std::vector<NodeIDSSet> &components) const;
	
	// Print the graph showing all vertices and edges.
	// 	print  : show vertices and edges separately.
	// 	write1 : format: (Time) Ori Des (ID), (show all the edges each in a line)
	// 	write2 : format: (Time) Des Ori1 Ori2... (show all vertices with connected nodes)
	//	write3 : Pajek .net format
	//	write_edges : edges only for Pajek .net format
	virtual void print() const=0;	// To stdout.
	virtual void write1(std::ostream &of, long time=-1) const=0;	// To a file (when time<0, time will not be written)
	virtual void write2(std::ostream &of, long time=-1) const=0;	// To a file (when time<0, time will not be written)
	virtual void write3(std::ostream &of) const=0;	// To a file.
	virtual void write_edges(std::ostream &of) const=0;	// To a file.
	virtual void write_degrees(std::ostream &of) const=0;	// To a file.

	// Read the graph structure info from a file (# of vertices are assumed to be known).
	// read1: only reads (ori, des) from each line (for graphs without edge state).
	virtual bool read1(std::istream &ifile)=0;
	// read2: only reads (ori, des, id) from each line (for graphs with edge states).
	virtual bool read2(std::istream &ifile)=0;

   protected:
   	long nedges; // Number of edges.
	LinkType edge_type; // Type of edges (links).

   	bool loop;
	bool multi_link;
};


// Abstract class that represents graphs with fixed vertices.
class GraphF: public Graph {
   public :
	// Constructors.
	GraphF() : Graph() {};
	GraphF(long nv, LinkType lt=0, bool l=false, bool m=false) 
		: Graph(lt,l,m), nvertices(nv) {};

	// Destructor.
	~GraphF() {};

	// Get the size of vertices.
	long get_vertex_size() const {
		return nvertices;
	};

	// Show if nodes are fixed.
	bool if_nodes_fixed() const {
		return true;
	};

	// Find the set of all current vertices.
	void find_vertices(NodeIDSSet &vertexset) const {
		vertexset.clear();
		for (NodeID id=0;id<nvertices;id++)
			vertexset.insert(id);
	};

	// Find if the given vertex exists.
	bool find_vertex(NodeID v) const {
		if (v<0 || v>nvertices-1) return false;
		else return true;
	};

	// Find the degree sequence (not sorted). (Returns the maximun degree value)
	long find_degree_sequence(std::vector<long> &degree, bool neighbor=false) const;
	long find_in_deg_sequence(std::vector<long> &degree, bool neighbor=false) const;
	long find_out_deg_sequence(std::vector<long> &degree, bool neighbor=false) const;

	// Find the degree statistics. (returns the number of vertices with k=0)
	long find_degree_stat(std::vector<long> &k_stat) const;
	long find_in_deg_stat(std::vector<long> &k_stat) const;
	long find_out_deg_stat(std::vector<long> &k_stat) const;

	// Find the clustering coefficients (in and out will be used for directed graphs).
	// Averaging over all nodes (for directed graphs, average over ins and outs).
	double find_clustering_coeff() const;	
	double find_clustering_in_coeff() const;	
	double find_clustering_out_coeff() const;	

   protected:
   	long nvertices; // Number of vertices.
};

// Abstract class that represents graphs with varying vertices.
class GraphV : public Graph {
   public :
	// Constructors.
   	GraphV(NodeIDSSet &vs, LinkType lt=0, bool l=false, bool m=false)	// there should be a set of vertices already
		: Graph(lt,l,m), vertexset(vs) {};

	// Destructor.
	~GraphV() {};

	// Get sizes of vertices.
	long get_vertex_size() const {
		return vertexset.size();
	};

	// Show if nodes are fixed.
	bool if_nodes_fixed() const {
		return false;
	};

	// Find the set of all current vertices.
	void find_vertices(NodeIDSSet &vs) const {
		vs=vertexset;
	};
	// Find if the given vertex exists.
	bool find_vertex(NodeID v) const {
		return (vertexset.find(v)!=vertexset.end());
	};

	// Find the degree sequence (not sorted). (Returns the maximun degree value)
	long find_degree_sequence(std::vector<long> &degree, bool neighbor=false) const;
	long find_in_deg_sequence(std::vector<long> &degree, bool neighbor=false) const;
	long find_out_deg_sequence(std::vector<long> &degree, bool neighbor=false) const;

	// Find the degree statistics. (returns the number of vertices with k=0)
	long find_degree_stat(std::vector<long> &k_stat) const;
	long find_in_deg_stat(std::vector<long> &k_stat) const;
	long find_out_deg_stat(std::vector<long> &k_stat) const;

	// Find the clustering coefficients (in and out will be used for directed graphs).
	// Averaging over all nodes (for directed graphs, average over ins and outs).
	double find_clustering_coeff() const;	
	double find_clustering_in_coeff() const;	
	double find_clustering_out_coeff() const;	

   protected:
   	NodeIDSSet &vertexset; // Set of all vertices (reference).

};

}; // End of namespace conet.
#endif
