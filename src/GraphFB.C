// 
//	CONETSIM - GraphFB.C
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

#include "GraphFB.h"
#include <iostream>
#include <istream>
#include <ostream>
#include <set>
#include <map>

#ifndef MAX_CHARS
#define MAX_CHARS 1000
#endif

using namespace std;

// Find if an edge(s) exist between ori and des
bool conet::GraphFB::find_edge(NodeID ori, NodeID des) {
	if (find_vertex(ori) && find_vertex(des)) 
		return (gra[ori].find(des)!=gra[ori].end());
	else
		return false;
};
	
// Assignment function.
bool conet::GraphFB::copy(const Graph &gr) {
	// Checking if they are the same type
	const GraphFB *p=dynamic_cast<const GraphFB *>(&gr);
	if (!p) return false;
	if (nvertices==gr.get_vertex_size()) {
		this->clear_edges();
	}
	else {
		nvertices=gr.get_vertex_size();
		this->clear_edges();
		gra.resize(nvertices);
	};
	for (NodeID i=0;i<nvertices;i++)
		gr.find_dep(i,gra[i]);
	Graph::copy(gr);
	return true;
};

// Clearing all edges only.
void conet::GraphFB::clear_edges() {
	for (long i=0; i<nvertices;i++) gra[i].clear();
	nedges=0;
};

// Make this graph full.
void conet::GraphFB::make_full() {
	this->clear_edges();
	for (NodeID i=0; i<nvertices;i++)
		for (NodeID j=0; j<nvertices;j++)
			if (i<j) this->add_edge(i,j);
};

// Remove an edge (not all edges) betwen ori and des. Returns false if it doesn't exist.
bool conet::GraphFB::remove_edge(NodeID ori, NodeID des) {
	if (find_edge(ori,des)) {
		gra[ori].erase(gra[ori].find(des));
		if (ori!=des) gra[des].erase(gra[des].find(ori));
		nedges--;
		return true;
	}
	else
		return false;
};

// Add an edge. 
// Returns false if it fails (it has a non-existing vertex, if the edge
// already exists (when multilink=false), etc), true if successful.
bool conet::GraphFB::add_edge(NodeID ori, NodeID des) {
	if (find_vertex(ori) && find_vertex(des)) {
		if (!loop && ori==des)
			return false;
		else if (!multi_link && gra[ori].find(des)!=gra[ori].end())
			return false;
		else {
			gra[ori].insert(des);
			if (ori!=des) gra[des].insert(ori);
			nedges++;
			return true;
		};
	}
	else
		return false;
};

// Find the vertices that affect a given vertex. (Return the number
// 	of vertices or -1 if the vertex doesn't belong to the graph.)
long conet::GraphFB::find_dep(NodeID v, NodeIDMSet &deps, bool neighbor) const {
	if (!find_vertex(v)) return -1;
	deps.clear();
	if (neighbor) {
		NodeID last=-1;
		for (NodeIDMSet::const_iterator i=gra[v].begin();i!=gra[v].end();i++) {
			if (*i!=v && *i!=last) {
				deps.insert(*i);
			};
			last=*i;
		};
	}
	else
		deps=gra[v];
	return deps.size();
};

long conet::GraphFB::find_dep2(NodeID v, NodeIDSSet &deps) const {
	if (!find_vertex(v)) return -1;
	deps.clear();
	NodeID last=-1;
	for (NodeIDMSet::const_iterator i=gra[v].begin();i!=gra[v].end();i++) {
		if (*i!=v && *i!=last) {
			deps.insert(*i);
		};
		last=*i;
	};
	return deps.size();
};

// Find degree information.
// Find the number of degrees for a given vertex.
long conet::GraphFB::find_degree(NodeID v, bool neighbor) const {
	if (!find_vertex(v)) return -1;
	if (neighbor) {
		long count=0;
		NodeID last=-1;
		for (NodeIDMSet::const_iterator i=gra[v].begin();i!=gra[v].end();i++) {
			if (*i!=v && *i!=last) count++;
			last=*i;
		};
		return count;
	}
	else
		return gra[v].size();
};

// Find the number of edges between neighbors.
// Returns the number of edges and number of neighbors through argument.
long conet::GraphFB::find_neighbor_connections(NodeID id, long &degree) const {
	NodeIDSSet neighbors;	// Finding neighbors.
	degree=this->find_dep2(id,neighbors);
	if (degree>1) {
		long num_edges=0;
		for (NodeIDSSet::const_iterator i=neighbors.begin();i!=neighbors.end();i++) {
			NodeID last=-1;
			for (NodeIDMSet::const_iterator j=gra[*i].begin();j!=gra[*i].end();j++) {
				// Loops and multilinks are ignored, and double counting avoided.
				if (*j>*i && *j!=last && neighbors.find(*j)!=neighbors.end())
					num_edges++;
				last=*j;
			};
		};
		return num_edges; // Every edge is counted once.
	}
	else return 0;
};

// Find the degree correlation coefficient. (loop and multilink not ignored).
double conet::GraphFB::find_degree_correlation(bool neighbor) const {
	vector<long> degree;	// k(v)
	this->find_degree_sequence(degree,neighbor);
	
	double sum1=0, sum2=0, sum3=0;
	long deg_in, deg_out;
	// Edge summation.
	for (NodeID v=0;v<nvertices;v++)
		for (NodeIDMSet::const_iterator i=gra[v].begin();i!=gra[v].end();i++) {
			deg_in=degree[*i];
			deg_out=degree[v];
			sum1+=deg_in*deg_out;
			sum2+=deg_in;
			sum3+=deg_in*deg_in;
		};
	sum1/=2*nedges;
	sum2/=2*nedges;
	sum3/=2*nedges;

	double var=sum3-sum2*sum2;
	if (var<1.0e-8) return 100;
	return (sum1-sum2*sum2)/var;
};

// Print the graph showing all vertices and edges.
void conet::GraphFB::print() const {
	cout << "=== vertices ===\n";
	cout << "There are " << nvertices << " vertices";
	if (nvertices) cout << ", from 0 to " << nvertices-1;
	cout << ".\n";
	cout << "=== edges ===\n";
	cout << "There are " << get_edge_size() << " bidirectional edges.\n";
	for (long j=0; j<nvertices;j++) {
		cout << j << ": ";
		for (NodeIDMSet::const_iterator i=gra[j].begin();\
			i!=gra[j].end();i++)
			cout << *i << ' '; 
		cout << endl;
	};
};

void conet::GraphFB::write1(ostream &of, long time) const {
	for (long j=0; j<nvertices;j++)
		for (NodeIDMSet::const_iterator i=gra[j].begin();\
			i!=gra[j].end();i++)
			if (j<=*i) {
				if (time>=0) of << time << '\t';
				of << j << '\t' << *i << '\n';
			};
};

void conet::GraphFB::write2(ostream &of, long time) const {
	for (long j=0; j<nvertices;j++) {
		if (time>=0) of << time << '\t';
		of << j;
		for (NodeIDMSet::const_iterator i=gra[j].begin();\
			i!=gra[j].end();i++)
			of << '\t' << *i; 
		of << '\n';
	};
};
 
void conet::GraphFB::write3(ostream &of) const {
	of << "*Vertices " << nvertices << '\n';
	for (long j=0; j<nvertices;j++)
		of << j+1 << " \"" << j+1 << "\"\n";
	of << "*Edges\n";
	for (long j=0; j<nvertices;j++)
		for (NodeIDMSet::const_iterator i=gra[j].begin();\
			i!=gra[j].end();i++)
			if (*i<=j) of << *i+1 << ' ' << j+1 << " 1\n";; 
};
 
void conet::GraphFB::write_edges(ostream &of) const {
	of << "*Edges\n";
	for (long j=0; j<nvertices;j++)
		for (NodeIDMSet::const_iterator i=gra[j].begin();\
			i!=gra[j].end();i++)
			if (*i<=j) of << *i+1 << ' ' << j+1 << " 1\n";; 
};
 
void conet::GraphFB::write_degrees(ostream &of) const {
	for (long j=0; j<nvertices;j++)
		of << j << '\t' << gra[j].size() << '\n';; 
};
 
// Read the graph info from a file.
bool conet::GraphFB::read1(std::istream &ifile) {
	char tmpstr[MAX_CHARS+1];

	// Read the number of vertices, link_type, loop, multi_link
	// Vertices will be numbered from 0 to nvertices-1.
	while (ifile.peek()=='#')
		ifile.getline(tmpstr, MAX_CHARS, '\n');
//	ifile >> nvertices >> edge_type >> loop >> multi_link;
//	ifile.getline(tmpstr, MAX_CHARS, '\n');

	// Erase and make graph with no edges.
	this->clear_edges();
//	gra.resize(nvertices);

	// Add edges.
	NodeID ori, des;
	do {
		if(ifile.peek()!='#' && ifile >> ori >> des)
			this->add_edge(ori,des);
	} while (ifile.getline(tmpstr, MAX_CHARS, '\n'));
	return true;
};

// Assignment function.
bool conet::GraphFB_Full::copy(const Graph &gr) {
	// Checking if they are the same type
	const GraphFB_Full *p=dynamic_cast<const GraphFB_Full *>(&gr);
	if (!p) return false;
	edge_type=gr.get_edge_type();
	nvertices==gr.get_vertex_size();
	nedges=gr.get_edge_size();
	return true;
};

// Find the vertices that affect a given vertex. (Return the number
// 	of vertices or -1 if the vertex doesn't belong to the graph.)
long conet::GraphFB_Full::find_dep(NodeID v, NodeIDMSet &deps, bool neighbor) const {
	if (!find_vertex(v)) return -1;
	deps.clear();
	for (NodeID i=0;i<nvertices;i++) if (i!=v) deps.insert(i);
	return deps.size();
};
long conet::GraphFB_Full::find_dep2(NodeID v, NodeIDSSet &deps) const {
	if (!find_vertex(v)) return -1;
	deps.clear();
	for (NodeID i=0;i<nvertices;i++) if (i!=v) deps.insert(i);
	return deps.size();
};

// Print the graph showing all vertices and edges.
void conet::GraphFB_Full::print() const {
	cout << "=== vertices ===\n";
	cout << "There are " << nvertices << " vertices";
	if (nvertices) cout << ", from 0 to " << nvertices-1;
	cout << ".\n";
	cout << "=== edges ===\n";
	cout << "There are " << get_edge_size() << " bidirectional edges.\n";
	for (long j=0; j<nvertices;j++) {
		cout << j << ": ";
		for (NodeID i=0;i<nvertices;i++) if (i!=j) cout << i << ' '; 
		cout << endl;
	};
};

void conet::GraphFB_Full::write1(ostream &of, long time) const {
	for (long j=0; j<nvertices;j++)
		for (NodeID i=0;i<nvertices;i++)
			if (j<=i) {
				if (time>=0) of << time << '\t';
				of << j << '\t' << i << '\n';
			};
};

void conet::GraphFB_Full::write2(ostream &of, long time) const {
	for (long j=0; j<nvertices;j++) {
		if (time>=0) of << time << '\t';
		of << j;
		for (NodeID i=0;i<nvertices;i++)
			of << '\t' << i; 
		of << '\n';
	};
};
 
void conet::GraphFB_Full::write3(ostream &of) const {
	of << "*Vertices " << nvertices << '\n';
	for (long j=0; j<nvertices;j++)
		of << j+1 << " \"" << j+1 << "\"\n";
	of << "*Edges\n";
	for (long j=0; j<nvertices;j++)
		for (NodeID i=0;i<nvertices;i++)
			if (i<=j) of << i+1 << ' ' << j+1 << " 1\n";; 
};
 
void conet::GraphFB_Full::write_edges(ostream &of) const {
	of << "*Edges\n";
	for (long j=0; j<nvertices;j++)
		for (NodeID i=0;i<nvertices;i++)
			if (i<=j) of << i+1 << ' ' << j+1 << " 1\n";; 
};
 
void conet::GraphFB_Full::write_degrees(ostream &of) const {
	for (long j=0; j<nvertices;j++)
		of << j << '\t' << nvertices-1 << '\n';; 
};
 
