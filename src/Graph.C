// 
//	CONETSIM - Graph.C
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

#include "Graph.h"
#include <iostream>
#include <istream>
#include <ostream>
#include <algorithm>
#include <vector>
#include <set>
#include <map>

using namespace std;

// Check if the graph is full only when multi_link=loop=false..
bool conet::Graph::if_full() {
	if (multi_link || loop)
		return false;
	else {
		long nvertices=this->get_vertex_size();
		return (nedges==nvertices*(nvertices-1)/(this->if_directional()?1:2));
	};
};

// Obtain statistical quantities.
double conet::Graph::find_clustering_coeff_node(NodeID id) const {
	long degree;
	long num_connections=this->find_neighbor_connections(id,degree);	// number of connections
	if (degree>1)
		return double(num_connections)*(this->if_directional() ? 1.0 : 2.0)/(degree*(degree-1));
	else return 0.0;
};

double conet::Graph::find_clustering_in_coeff_node(NodeID id) const {
	long degree;
	long num_connections=this->find_in_neighbor_connections(id,degree);	// number of connections
	if (degree>1)
		return double(num_connections)*(this->if_directional() ? 1.0 : 2.0)/(degree*(degree-1));
	else return 0.0;
};

double conet::Graph::find_clustering_out_coeff_node(NodeID id) const {
	long degree;
	long num_connections=this->find_out_neighbor_connections(id,degree);	// number of connections
	if (degree>1)
		return double(num_connections)*(this->if_directional() ? 1.0 : 2.0)/(degree*(degree-1));
	else return 0.0;
};

double conet::Graph::find_clustering_coeff_cluster(const NodeIDSSet &cluster) const {
	// for a given cluster of the graph (directional).
	long size=cluster.size();
	double sum=0;	
	for (NodeIDSSet::const_iterator k=cluster.begin();k!=cluster.end();k++)
		sum+=this->find_clustering_coeff_node(*k);
	return sum/size;
};

double conet::Graph::find_clustering_in_coeff_cluster(const NodeIDSSet &cluster) const {
	// for a given cluster of the graph (directional).
	long size=cluster.size();
	double sum=0;	
	for (NodeIDSSet::const_iterator k=cluster.begin();k!=cluster.end();k++)
		sum+=this->find_clustering_in_coeff_node(*k);
	return sum/size;
};

double conet::Graph::find_clustering_out_coeff_cluster(const NodeIDSSet &cluster) const {
	// for a given cluster of the graph (directional).
	long size=cluster.size();
	double sum=0;	
	for (NodeIDSSet::const_iterator k=cluster.begin();k!=cluster.end();k++)
		sum+=this->find_clustering_out_coeff_node(*k);
	return sum/size;
};

double conet::Graph::find_mean_distance(double percent_sample) const {
	long npaths=0;	// Number of paths counted.
	long sum=0;
	NodeIDSSet vertexset;
	this->find_vertices(vertexset);
	long nvertices=vertexset.size();
	long num_try=long((percent_sample/100)*nvertices);
	if (num_try==0) num_try=1;	// num_try should not be zero.
	if (num_try>nvertices) num_try=nvertices;	// num_try should not be bigger than nvertices.

	vector<NodeID> nodes(nvertices);
	long index=0;
	for (NodeIDSSet::const_iterator i=vertexset.begin();i!=vertexset.end();i++)
	  	nodes[index++]=*i;
	random_shuffle(nodes.begin(),nodes.end());
	for (long i=0; i<num_try; i++) {
		NodeIDSSet deps1, deps2;	// Set of agents in a layer.
		NodeIDMSet temp_set;	// Temporary set
		NodeIDSSet found;	// store the ID's for which the path is already found.

		deps1.insert(nodes[i]); // First layer with the starting node only.
		found.insert(nodes[i]);

		long temp_dist=1;
		while(true) {
			// Find all dependent agents of agents in deps1
			// and insert them in deps2 if not already found.
			for (NodeIDSSet::const_iterator j=deps1.begin();\
				j!=deps1.end();j++)
				if (this->find_out_dep(*j,temp_set)!=0)
					for (NodeIDMSet::const_iterator k=temp_set.begin();\
						k!=temp_set.end();k++) {
						if (found.insert(*k).second) {
							deps2.insert(*k);
							npaths++;
							sum+=temp_dist;
						};
					};
			if (deps2.size()==0) break; // All paths reachable by id has been found
			deps1=deps2;
			deps2.clear();
			temp_dist++;
		};

	};

	if (npaths==0) return 0;
	return double(sum)/npaths;
};

double conet::Graph::find_mean_distance_component(const NodeIDSSet &component, double percent_sample) const {
	// Finding the mean distance of a given cluster using the given percentage of nodes.
	// This cluster is assumed to be a weakly connected cluster.
	long size=component.size();
	if (size<=1) return 0;

	long count=0;	// number of nodes used for averaging.
	long npaths=0;	// Number of paths counted.
	long sum=0;
	long num_try=long((percent_sample/100)*size);
	if (num_try==0) num_try=1;	// num_try should not be zero.
	if (num_try>size) num_try=size;	// num_try should not be bigger than size.

	// Shuffle the id's of the vertices.
	vector<NodeID> nodes(size);
	long index=0;
	for (NodeIDSSet::const_iterator i=component.begin();i!=component.end();i++)
	  	nodes[index++]=*i;
	random_shuffle(nodes.begin(),nodes.end());
	for (long i=0; i<num_try; i++) {
		NodeIDSSet deps1, deps2;	// Set of agents in a layer.
		NodeIDMSet temp_set;	// Temporary set
		NodeIDSSet found;	// store the ID's for which the path is already found.

		deps1.insert(nodes[i]); // First layer with the starting node only.
		found.insert(nodes[i]);

		long temp_dist=1;
		while (true) {
			// Find all dependent agents of agents in deps1
			// and insert them in deps2 if not already found.
			for (NodeIDSSet::const_iterator j=deps1.begin();\
				j!=deps1.end();j++)
				if (this->find_out_dep(*j,temp_set)!=0)
					for (NodeIDMSet::const_iterator k=temp_set.begin();\
						k!=temp_set.end();k++) {
						if (found.insert(*k).second) {
							deps2.insert(*k);
							npaths++;
							sum+=temp_dist;
						};
					};
			if (deps2.size()==0) break; // All paths reachable by id has been found
			deps1=deps2;
			deps2.clear();
			temp_dist++;
		};

	};

	if (npaths==0) return 0;
	return double(sum)/npaths;
};

// Find the components of the graph (weakly connected components for digraphs).
// (Returns the number of components.)
long conet::Graph::find_components(vector<NodeIDSSet> &components) const {
	NodeIDSSet vertexset;
	this->find_vertices(vertexset);
	long nvertices=vertexset.size();
	NodeIDSSet searched;	// Set of agents that are already searched.
	NodeIDSSet deps1, deps2;	// Set of agents in a layer.
	NodeIDSSet temp_set1;	// Temporary set.
	NodeIDMSet temp_set2;	// Temporary set

	for (NodeIDSSet::const_iterator i=vertexset.begin();i!=vertexset.end();i++)
		if (searched.find(*i)==searched.end()) {
			temp_set1.insert(*i); // Set of agents in this component.
			deps1=temp_set1;
			bool all;	// true if all agents of the component are found.
			do {
				all=true;
				// Find all dependent agents of agents in deps1
				// and insert them in deps2.
				for (NodeIDSSet::const_iterator i=deps1.begin();\
					i!=deps1.end();i++)
					if (searched.insert(*i).second && this->find_dep(*i,temp_set2)!=0) {
						for (NodeIDMSet::const_iterator j=temp_set2.begin();\
							j!=temp_set2.end();j++) {
							deps2.insert(*j);
							// Find if all dependent agents are 
							// already in the component.
							if (temp_set1.insert(*j).second) {
								all=false;
							};
						};
					};
				deps1=deps2;
				deps2.clear();
			} while (!all);
			components.push_back(temp_set1);
			temp_set1.clear();
		};
	return components.size();
};

// Find the degree sequence (not sorted). (Returns the maximun degree value)
long conet::GraphF::find_degree_sequence(std::vector<long> &degree, bool neighbor) const {
	long max=0;
	degree.resize(nvertices);
	for (NodeID v=0;v<nvertices;v++) {
		degree[v]=this->find_degree(v,neighbor);
		if (degree[v]>max) max=degree[v];
	};
	return max;
};

long conet::GraphF::find_in_deg_sequence(std::vector<long> &in_deg, bool neighbor) const {
	long max=0;
	in_deg.resize(nvertices);
	for (NodeID v=0;v<nvertices;v++) {
		in_deg[v]=this->find_in_deg(v,neighbor);
		if (in_deg[v]>max) max=in_deg[v];
	};
	return max;
};

long conet::GraphF::find_out_deg_sequence(std::vector<long> &out_deg, bool neighbor) const {
	long max=0;
	out_deg.resize(nvertices);
	for (NodeID v=0;v<nvertices;v++) {
		out_deg[v]=this->find_out_deg(v,neighbor);
		if (out_deg[v]>max) max=out_deg[v];
	};
	return max;
};

// Find the degree statistics.
long conet::GraphF::find_degree_stat(vector<long> &k_stat) const {
	//k_stat.clear(); // Not necessary when it is used repeatedly.
	long size=k_stat.size();
	for (long k=0;k<size;k++)
		k_stat[k]=0;
	long degree;
	for (NodeID i=0;i<nvertices;i++) {
		degree=this->find_degree(i);
		if (degree<size)
			k_stat[degree]++;
	};
	return k_stat[0];
};

long conet::GraphF::find_in_deg_stat(vector<long> &k_stat) const {
	//k_stat.clear(); // Not necessary when it is used repeatedly.
	long size=k_stat.size();
	for (long k=0;k<size;k++)
		k_stat[k]=0;
	long degree;
	for (NodeID i=0;i<nvertices;i++) {
		degree=this->find_in_deg(i);
		if (degree<size)
			k_stat[degree]++;
	};
	return k_stat[0];
};

long conet::GraphF::find_out_deg_stat(vector<long> &k_stat) const {
	//k_stat.clear(); // Not necessary when it is used repeatedly.
	long size=k_stat.size();
	for (NodeID k=0;k<size;k++)
		k_stat[k]=0;
	long degree;
	for (NodeID i=0;i<nvertices;i++) {
		degree=this->find_out_deg(i);
		if (degree<size)
			k_stat[degree]++;
	};
	return k_stat[0];
};

double conet::GraphF::find_clustering_coeff() const {
	double sum=0;
	for (NodeID id=0;id<nvertices;id++)
		sum+=this->find_clustering_coeff_node(id);
	return sum/nvertices;
};

double conet::GraphF::find_clustering_in_coeff() const {
	double sum=0;
	for (NodeID id=0;id<nvertices;id++)
		sum+=this->find_clustering_in_coeff_node(id);
	return sum/nvertices;
};

double conet::GraphF::find_clustering_out_coeff() const {
	double sum=0;
	for (NodeID id=0;id<nvertices;id++)
		sum+=this->find_clustering_out_coeff_node(id);
	return sum/nvertices;
};

// Find the degree sequence (not sorted). (Returns the maximun degree value)
long conet::GraphV::find_degree_sequence(std::vector<long> &degree, bool neighbor) const {
	long nvertices=vertexset.size();
	long max=0;
	degree.resize(nvertices);
	for (NodeIDSSet::const_iterator i=vertexset.begin();i!=vertexset.end();i++) {
		degree[*i]=this->find_degree(*i,neighbor);
		if (degree[*i]>max) max=degree[*i];
	};
	return max;
};

long conet::GraphV::find_in_deg_sequence(std::vector<long> &in_deg, bool neighbor) const {
	long nvertices=vertexset.size();
	long max=0;
	in_deg.resize(nvertices);
	for (NodeIDSSet::const_iterator i=vertexset.begin();i!=vertexset.end();i++) {
		in_deg[*i]=this->find_in_deg(*i,neighbor);
		if (in_deg[*i]>max) max=in_deg[*i];
	};
	return max;
};

long conet::GraphV::find_out_deg_sequence(std::vector<long> &out_deg, bool neighbor) const {
	long nvertices=vertexset.size();
	long max=0;
	out_deg.resize(nvertices);
	for (NodeIDSSet::const_iterator i=vertexset.begin();i!=vertexset.end();i++) {
		out_deg[*i]=this->find_out_deg(*i,neighbor);
		if (out_deg[*i]>max) max=out_deg[*i];
	};
	return max;
};

// Find the degree statistics.
long conet::GraphV::find_degree_stat(vector<long> &k_stat) const {
	//k_stat.clear(); // Not necessary when it is used repeatedly.
	long size=k_stat.size();
	for (long k=0;k<size;k++)
		k_stat[k]=0;
	long degree;
	for (NodeIDSSet::const_iterator i=vertexset.begin();i!=vertexset.end();i++) {
		degree=this->find_degree(*i);
		if (degree<size)
			k_stat[degree]++;
	};
	return k_stat[0];
};

long conet::GraphV::find_in_deg_stat(vector<long> &k_stat) const {
	//k_stat.clear(); // Not necessary when it is used repeatedly.
	long size=k_stat.size();
	for (long k=0;k<size;k++)
		k_stat[k]=0;
	long degree;
	for (NodeIDSSet::const_iterator i=vertexset.begin();i!=vertexset.end();i++) {
		degree=this->find_in_deg(*i);
		if (degree<size)
			k_stat[degree]++;
	};
	return k_stat[0];
};

long conet::GraphV::find_out_deg_stat(vector<long> &k_stat) const {
	//k_stat.clear(); // Not necessary when it is used repeatedly.
	long size=k_stat.size();
	for (NodeID k=0;k<size;k++)
		k_stat[k]=0;
	long degree;
	for (NodeIDSSet::const_iterator i=vertexset.begin();i!=vertexset.end();i++) {
		degree=this->find_out_deg(*i);
		if (degree<size)
			k_stat[degree]++;
	};
	return k_stat[0];
};

double conet::GraphV::find_clustering_coeff() const {
	double sum=0;
	for (NodeIDSSet::const_iterator i=vertexset.begin();i!=vertexset.end();i++)
		sum+=this->find_clustering_coeff_node(*i);
	return sum/vertexset.size();
};

double conet::GraphV::find_clustering_in_coeff() const {
	double sum=0;
	for (NodeIDSSet::const_iterator i=vertexset.begin();i!=vertexset.end();i++)
		sum+=this->find_clustering_in_coeff_node(*i);
	return sum/vertexset.size();
};

double conet::GraphV::find_clustering_out_coeff() const {
	double sum=0;
	for (NodeIDSSet::const_iterator i=vertexset.begin();i!=vertexset.end();i++)
		sum+=this->find_clustering_out_coeff_node(*i);
	return sum/vertexset.size();
};
