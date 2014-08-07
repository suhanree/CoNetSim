// 
//	CONETSIM - Utilities.C
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

#include "Types.h"
#include "Utilities.h"
#include "Link.h"
#include "Links.h"
#include "Graphs.h"

#include <iostream>
#include <ostream>
#include <cmath>

// To be used to compare two double values.
#define epsilon 1.0e-8

using namespace std;

// Print the vector of long.
void conet::print_vector(const vector<long> &v) {
	for (vector<long>::const_iterator i=v.begin();i!=v.end();i++)
		cout << *i << ' ';
	cout << endl;
};

void conet::print_vector(const vector<double> &v) {
	for (vector<double>::const_iterator i=v.begin();i!=v.end();i++)
		cout << *i << ' ';
	cout << endl;
};

// Print the list of long.
void conet::print_list(const list<long> &li) {
	for (std::list<long>::const_iterator i=li.begin();i!=li.end();i++)
		cout << *i << ' ';
	cout << endl;
};

// Print the set of long.
void conet::print_set(const set<long> &s) {
	for (set<long>::const_iterator i=s.begin();i!=s.end();i++)
		cout << *i << ' ';
	cout << endl;
};
void conet::print_set(const multiset<long> &s) {
	for (multiset<long>::const_iterator i=s.begin();i!=s.end();i++)
		cout << *i << ' ';
	cout << endl;
};
void conet::print_set(const PointSet &s) {
	for (PointSet::const_iterator i=s.begin();i!=s.end();i++)
		cout << *i << endl;
};

// Print the map of long and double.
void conet::print_map(const map<long,double> &m) {
	for (map<long,double>::const_iterator i=m.begin();i!=m.end();i++)
		cout << i->first << ' ' << i->second << endl;
};

void conet::print_map(const multimap<long,double> &m) {
	for (multimap<long,double>::const_iterator i=m.begin();i!=m.end();i++)
		cout << i->first << ' ' << i->second << endl;
};

// Print the map of long and long.
void conet::print_map(const map<long,long> &m) {
	for (map<long,long>::const_iterator i=m.begin();i!=m.end();i++) {
		cout << i->first << '\t' << i->second << endl;
	};
};
void conet::print_map(const multimap<long,long> &m) {
	for (multimap<long,long>::const_iterator i=m.begin();i!=m.end();i++) {
		cout << i->first << '\t' <<  i->second << endl;
	};
};

// Check if a file exists.
bool conet::if_file(std::string &filename) {
	struct stat file_info;
	return (!stat(filename.c_str(),&file_info));
};

// Find the cdf from given pdf (assuming sizes of both vectors are the same).
void conet::find_cdf(const std::vector<double> &pdf, vector<double> & cdf) {
	long size=pdf.size();	
	cdf[0]=0;
	for (long j=1;j<size;j++)
		cdf[j]=cdf[j-1]+pdf[j-1];
	// Normalize if necessary.
	if (fabs( 1.0 - (cdf[size-1]+pdf[size-1]) ) > epsilon)
		for (long j=1;j<size;j++) cdf[j]/=(cdf[size-1]+pdf[size-1]); 
};
void conet::find_cdf(const std::vector<long> &pdf, vector<double> & cdf) {
	long size=pdf.size();	
	cdf[0]=0;
	for (long i=1;i<size;i++)
		cdf[i]=cdf[i-1]+pdf[i-1];
	// Normalize if necessary.
	if (fabs(1.0-(cdf[size-1]+pdf[size-1]))> epsilon)
		for (long i=1;i<size;i++) cdf[i]/=(cdf[size-1]+pdf[size-1]); 
};

// Initializing a graph as a random graph at t=0 using the number of edges.
bool conet::initialize_random_graph1(LinkType lt, Graphs &graphs, Links &links, long num_edges, unsigned long rseed) {
	TimeType current_time=0;
	if (graphs.get_time()!=current_time || links.get_time()!=current_time) return false;

	Graph &gr=graphs.access_graph(lt,current_time); // should get it as a reference to change the value.
	long nvertices=gr.get_vertex_size();
	bool if_multilink=gr.if_multi_link();

	if (nvertices<2)
		return true;
	else if (nvertices>4e+04) // If nvertices > 40000, overflow and use the below function instead.
		return initialize_random_graph2(lt,graphs,links,double(num_edges)/(nvertices-1),rseed);
	else {
		gr.clear_edges(); // Delete all edges in Graph if exists.
		links.clear(lt); // Delete all edges.
		RNG rng(rseed);
		long num_all_edges=nvertices*(nvertices-1)/(gr.if_directional()?1:2);
		NodeIDSSet vertexset; // Empty set.
		gr.find_vertices(vertexset); // Find all vertices.
	   if (if_multilink) {
		multiset<long> found_multi;
		do {
			found_multi.insert(rng.ndraw(0,num_all_edges-1));
		} while (found_multi.size()<num_edges);
		long count=0;
		long ID=0;
		for (NodeIDSSet::const_iterator i=vertexset.begin(); \
			i!=vertexset.end();i++) 
			for (NodeIDSSet::const_iterator j=vertexset.begin();\
				j!=vertexset.end();j++) {
				if ((gr.if_directional() ? *i!=*j : *i>*j)) {
					if (found_multi.find(count)!=found_multi.end()) {
						long count2=found_multi.count(count); // Find the number of links.
						for (long k=0;k<count2;k++) {
							if (gr.if_link_ID()) {
								if (gr.add_edge_ID(*i,*j,ID)) {
									if (links.add_link(ID,lt,current_time))
										ID++;
									else
										gr.remove_edge_ID(*i,*j,ID);
								};
							}
							else 
								if (gr.add_edge(*i,*j)) 
									links.add_link_noID(lt,current_time);
						};
					};
					count++;
				};
			};
	   }
	   else {
		set<long> found;
		do {
			found.insert(rng.ndraw(0,num_all_edges-1));
		} while (found.size()<num_edges);
		long count=0;
		long ID=0;
		for (NodeIDSSet::const_iterator i=vertexset.begin(); \
			i!=vertexset.end();i++) 
			for (NodeIDSSet::const_iterator j=vertexset.begin();\
				j!=vertexset.end();j++) {
				if ((gr.if_directional() ? *i!=*j : *i>*j)) {
					if (found.find(count)!=found.end()) {
						if (gr.if_link_ID()) {
							if (gr.add_edge_ID(*i,*j,ID)) {
								if (links.add_link(ID,lt,current_time))
									ID++;
								else
									gr.remove_edge_ID(*i,*j,ID);
							};
						}
						else 
							if (gr.add_edge(*i,*j)) 
								links.add_link_noID(lt,current_time);
					};
					count++;
				};
			};
	   };
		return true;
	};
};

// Initializing a graph as a random graph at t=0 using the wiring probability.
bool conet::initialize_random_graph2(LinkType lt, Graphs &graphs, Links &links, double wiring_prob, unsigned long rseed) {
	TimeType current_time=0;
	if (graphs.get_time()!=current_time || links.get_time()!=current_time) return false;
	Graph &gr=graphs.access_graph(lt,current_time);

	long nvertices=gr.get_vertex_size();
	if (nvertices<2) return true;
	if (wiring_prob>1.0 || wiring_prob<0.0) return false;

	gr.clear_edges(); // Delete all edges if exists.
	links.clear(lt); // Delete all edges.
	
	RNG rng(rseed);
	NodeIDSSet vertexset; // Empty set.
	gr.find_vertices(vertexset); // Find all vertices.

	long ID=0;
	for (NodeIDSSet::const_iterator i=vertexset.begin(); \
		i!=vertexset.end();i++) 
		for (NodeIDSSet::const_iterator j=vertexset.begin();\
			j!=vertexset.end();j++)
			if ((gr.if_directional() ? *i!=*j : *i>*j) && rng.fdraw()<wiring_prob) {
				if (gr.if_link_ID()) {
					if (gr.add_edge_ID(*i,*j,ID)) {
						if (links.add_link(ID,lt,current_time))
							ID++;
						else
							gr.remove_edge_ID(*i,*j,ID);
					};
				}
				else 
					if (gr.add_edge(*i,*j))
						links.add_link_noID(lt,current_time);
			};
};

// Initializing a graph as a random graph at t=0 using the fixed out_degree for each node.
// Each vertex originates an edge with randomly choosing destination (suitable when AG is directional)
bool conet::initialize_random_graph3(LinkType lt, Graphs &graphs, Links &links, long fixed_out_degree, unsigned long rseed) {
	TimeType current_time=0;
	if (graphs.get_time()!=current_time || links.get_time()!=current_time) return false;
	Graph &gr=graphs.access_graph(lt,current_time);

	long nvertices=gr.get_vertex_size();
	if (nvertices<2) return true;

	gr.clear_edges(); // Delete all edges if exists.
	links.clear(lt); // Delete all edges.

	RNG rng(rseed);
	NodeID ori,des;
	NodeIDSSet vertexset; // Empty set.

	gr.find_vertices(vertexset); // Find all vertices.

	// To store ID's in a vector 
	// (then this works also for GraphV and its subclasses)
	vector<NodeID> vertexarray(nvertices); 
	long ind=0;
	for (NodeIDSSet::const_iterator j=vertexset.begin();\
		j!=vertexset.end();j++) {
		vertexarray[ind]=*j;
		ind++;
	};
	long ID=0;
	for (NodeIDSSet::const_iterator i=vertexset.begin(); \
		i!=vertexset.end();i++) {
		NodeID k;
		long count=0;
		do {
			k=rng.ndraw(0,nvertices-1);
				if (gr.if_link_ID()) {
					if (gr.add_edge_ID(*i,vertexarray[k],ID)) {
						if (links.add_link(ID,lt,current_time)) {
							ID++;
							count++;
						}
						else
							gr.remove_edge_ID(*i,vertexarray[k],ID);
					};
				}
				else {
					if (gr.add_edge(*i,vertexarray[k])) {
						links.add_link_noID(lt,current_time);
						count++;
					};
				};
		} while (count<fixed_out_degree);
	};
	return true;
};

// Initialize a full graph.
bool conet::initialize_full_graph(LinkType lt, Graphs &graphs, Links &links) {
	Graph &gr=graphs.access_graph(lt,0);
	gr.make_full();
	links.clear(lt);
	// Add all possible links from the full graph.
	NodeIDSSet vertexset; // Empty set.
	gr.find_vertices(vertexset); // Find all vertices.
	long ID=0;
	for (NodeIDSSet::const_iterator i=vertexset.begin(); \
		i!=vertexset.end();i++) 
		for (NodeIDSSet::const_iterator j=vertexset.begin();\
			j!=vertexset.end();j++)
			if ((gr.if_directional() ? *i!=*j : *i>*j)) {
				if (gr.if_link_ID()) {
					if (links.add_link(ID,lt,0))
						ID++;
				}
				else 
					links.add_link_noID(lt,0);
			};
};

// Combine two graphs (if ag is directed, change it to undirected)
bool conet::combine_graphs(GraphFB &cg, const GraphFB &sg, const GraphF &ag) {
	// Assume all 3 graphs have the same number of vertices (fixed number).
	if (cg.get_vertex_size()!=sg.get_vertex_size() || \
		sg.get_vertex_size()!=ag.get_vertex_size()) return false;
	// Assume cg and sg are bidirectional, ag can be both.
	if (cg.if_directional() || sg.if_directional()) return false;	
	// Assume all 3 graphs have no self-loop.
	if (cg.if_loop() || sg.if_loop() || ag.if_loop()) return false;
	// Assume cg has no multilin, but sg and cg can have multiple links.
	if (cg.if_multi_link()) return false;
	
	cg.clear_edges();

	NodeIDMSet deps;

	// Copying sg to cg.
	for (NodeID id=0;id<sg.get_vertex_size();id++) {
		sg.find_in_dep(id,deps,true);
		for (NodeIDMSet::const_iterator i=deps.begin();i!=deps.end();i++) {
			cg.add_edge(id,*i);
		};
	};
	// Copying ag to cg.
	for (NodeID id=0;id<ag.get_vertex_size();id++) {
		ag.find_in_dep(id,deps,true);
		for (NodeIDMSet::const_iterator i=deps.begin();i!=deps.end();i++) {
			cg.add_edge(id,*i);
		};
	};
	return true;
};

// Write the components
void conet::write_components(const vector<NodeIDSSet> &components, ostream &of) {
	long ncomponents=components.size();
	for (long i=0;i<ncomponents;i++)
		for (NodeIDSSet::const_iterator j=components[i].begin();j!=components[i].end();j++)
			of << i << '\t' << *j << '\n';
};

// get the maximum value out of 3 integers.
long conet::find_max(long i1, long i2, long i3) {
	long temp=(i1>=i2 ? i1 : i2);
	return (temp>=i3 ? temp : i3);
};
long conet::find_max(long i1, long i2) {
	return (i1>=i2 ? i1 : i2);
};
