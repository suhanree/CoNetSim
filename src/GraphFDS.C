// 
//	CONETSIM - GraphFDS.C
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

#include "GraphFDS.h"
#include <iostream>
#include <istream>
#include <ostream>
#include <vector>
#include <set>
#include <map>
#include <cstdlib>
#include <cmath>

using namespace std;

const long MAX_CHARS=1000;

// Find the set of all current edges.
bool conet::GraphFDS::find_all_edges(LinkIDSet &edgeset) const {
	for (NodeID id=0;id<nvertices;id++) {
		for (NodeIDLinkIDMMap::const_iterator i=gra[id].begin();i!=gra[id].end();i++)
			edgeset.insert(i->second);
	};
	return true;
};

// Find edge(s) between two vertices.
// Returns true if exist(s), false if none.
bool conet::GraphFDS::find_edge(NodeID ori, NodeID des) {
	if (find_vertex(ori) && find_vertex(des)) {
		if (gra[des].find(ori)!=gra[des].end())
			return true;
		else
			return false;
	}
	else
		return false;
};
	
bool conet::GraphFDS::find_edge_ID(NodeID ori, NodeID des, LinkIDSet &edgeset) const {
	edgeset.clear();
	if (find_vertex(ori) && find_vertex(des)) {
		pair<NodeIDLinkIDMMap::const_iterator,NodeIDLinkIDMMap::const_iterator> temp=gra[des].equal_range(ori);
		long count=0;
		for (NodeIDLinkIDMMap::const_iterator i=temp.first;i!=temp.second;i++) {
			edgeset.insert(i->second);
			count++;
		};
		if (count) return true;
		else return false;
	}
	else
		return false;
};
	
// Assignment function.
bool conet::GraphFDS::copy(const Graph &gr) {
	// Checking if they are the same type
	const GraphFDS *p=dynamic_cast<const GraphFDS *>(&gr);
	if (!p) return false;
	this->clear_edges();
	if (nvertices!=gr.get_vertex_size()) {
		nvertices=gr.get_vertex_size();
		gra.resize(nvertices);
		gra_out.resize(nvertices);
	};
	for (NodeID id=0;id<nvertices;id++) {
		gr.find_in_dep_ID(id,gra[id]);
		gr.find_out_dep_ID(id,gra_out[id]);
	};
	Graph::copy(gr);
	return true;
};

// Clearing all edges only.
void conet::GraphFDS::clear_edges() {
	for (NodeID id=0; id<nvertices;id++) {
		gra[id].clear();
		gra_out[id].clear();
	};
	nedges=0;
};

// Make this graph full.
void conet::GraphFDS::make_full() {
	this->clear_edges();
	NodeID id=0;
	for (NodeID i=0; i<nvertices;i++)
		for (NodeID j=0; j<nvertices;j++) {
			this->add_edge_ID(i,j,id);
			id++;
		};
};

// Remove an edges (not all edges) between two vertices. Returns false if it doesn't exist.
bool conet::GraphFDS::remove_edge(NodeID ori, NodeID des) {
	if (find_edge(ori,des)) {
		gra[des].erase(gra[des].find(ori));
		gra_out[ori].erase(gra_out[ori].find(des));
		nedges--;
		return true;
	}
	else
		return false;
};

// Remove edge e between ori and des (return false if e is not between ori and des)
bool conet::GraphFDS::remove_edge_ID(NodeID ori, NodeID des, LinkID id) {
	pair<NodeIDLinkIDMMap::iterator,NodeIDLinkIDMMap::iterator> range;
	if (find_vertex(ori) && find_vertex(des)) {
		range=gra[des].equal_range(ori);
		bool found=false;
		NodeIDLinkIDMMap::iterator found_pos;
		for (NodeIDLinkIDMMap::iterator i=range.first;i!=range.second;i++) {
			if (i->second==id) {
				found_pos=i;
				found=true;
				break;
			};
		};
		range=gra_out[ori].equal_range(des);
		for (NodeIDLinkIDMMap::iterator i=range.first;i!=range.second;i++) {
			if (i->second==id && found) {
				gra[des].erase(found_pos);
				gra_out[ori].erase(i);
				nedges--;
				return true;
			};
		};
		return false;
	}
	else
		return false;
};

// Add an edge. 
// Returns false if it fails (if it has a non-existing vertex, if the edge
// already exists when multilink=false, etc), true if successful.
bool conet::GraphFDS::add_edge_ID(NodeID ori, NodeID des, LinkID id) {
	if (find_vertex(ori) && find_vertex(des)) {
		if (!loop && ori==des)
			return false;
		else {
			if (!multi_link) {
			LinkIDSet edgeset;
			find_edge_ID(ori,des,edgeset);
			if (edgeset.find(id)!=edgeset.end())
				return false;
			};
			gra[des].insert(NodeIDLinkIDMMap::value_type(ori,id));
			gra_out[ori].insert(NodeIDLinkIDMMap::value_type(des,id));
			nedges++;
			return true;
		};
	}
	else
		return false;
};

// Find the vertices that affect a given vertex. (Return the number
// 	of vertices or -1 if the vertex doesn't belong to the graph.)
long conet::GraphFDS::find_dep(NodeID v, NodeIDMSet &deps, bool neighbor) const {
	// We can define it as below.
	if (find_vertex(v)) {
		this->find_in_dep(v,deps,neighbor);	// dpes will be cleared here.
		NodeID last=-1;
		for(NodeIDLinkIDMMap::const_iterator i=gra_out[v].begin(); \
			i!=gra_out[v].end();i++) {
			if (neighbor) {
				if (i->first!=v && i->first!=last && deps.find(i->first)==deps.end())
					deps.insert(i->first); 
			}
			else
				deps.insert(i->first); 
			last=i->first;
		};
		return deps.size();
	}
	else 
		return -1;
};

long conet::GraphFDS::find_dep2(NodeID v, NodeIDSSet &deps) const {
	// We can define it as below.
	if (find_vertex(v)) {
		this->find_in_dep2(v,deps);	// dpes will be cleared here.
		for(NodeIDLinkIDMMap::const_iterator i=gra_out[v].begin(); \
			i!=gra_out[v].end();i++) {
			deps.insert(i->first); 
		};
		return deps.size();
	}
	else
		return -1;
};

long conet::GraphFDS::find_dep_ID(NodeID v, NodeIDLinkIDMMap &deps) const {
	if (find_vertex(v)) {
		this->find_in_dep_ID(v,deps);	// dpes will be cleared here.
		for(NodeIDLinkIDMMap::const_iterator i=gra_out[v].begin(); \
			i!=gra_out[v].end();i++) {
			deps.insert(*i);
		};
		return deps.size();
	}
	else
		return -1;
};

long conet::GraphFDS::find_in_dep(NodeID v, NodeIDMSet &deps, bool neighbor) const {
	if (find_vertex(v)) {
		deps.clear();
		NodeID last=-1;
		for(NodeIDLinkIDMMap::const_iterator i=gra[v].begin(); \
			i!=gra[v].end();i++) {
			if (neighbor) {
				if (i->first!=v && i->first!=last)
					deps.insert(i->first); 
			}
			else 
				deps.insert(i->first); 
			last=i->first;
		};
		return deps.size();
	}
	else
		return -1;
};

long conet::GraphFDS::find_in_dep2(NodeID v, NodeIDSSet &deps) const {
	if (find_vertex(v)) {
		deps.clear();
		NodeID last=-1;
		for(NodeIDLinkIDMMap::const_iterator i=gra[v].begin(); \
			i!=gra[v].end();i++) {
			if (i->first!=v && i->first!=last)
				deps.insert(i->first); 
			last=i->first;
		};
		return deps.size();
	}
	else
		return -1;
};

long conet::GraphFDS::find_in_dep_ID(NodeID v, NodeIDLinkIDMMap &deps) const {
	if (find_vertex(v)) {
		deps=gra[v];
		return deps.size();
	}
	else
		return -1;
};

long conet::GraphFDS::find_out_dep(NodeID v, NodeIDMSet &deps, bool neighbor) const {
	if (find_vertex(v)) {
		deps.clear();
		NodeID last=-1;
		for(NodeIDLinkIDMMap::const_iterator i=gra_out[v].begin(); \
			i!=gra_out[v].end();i++) {
			if (neighbor) {
				if (i->first!=v && i->first!=last)
					deps.insert(i->first); 
			}
			else
				deps.insert(i->first);
			last=i->first;
		};
		return deps.size();
	}
	else
		return -1;
};

long conet::GraphFDS::find_out_dep2(NodeID v, NodeIDSSet &deps) const {
	if (find_vertex(v)) {
		deps.clear();
		NodeID last=-1;
		for(NodeIDLinkIDMMap::const_iterator i=gra_out[v].begin(); \
			i!=gra_out[v].end();i++) {
			if (i->first!=v && i->first!=last)
				deps.insert(i->first); 
			last=i->first;
		};
		return deps.size();
	}
	else
		return -1;
};

long conet::GraphFDS::find_out_dep_ID(NodeID v, NodeIDLinkIDMMap &deps) const {
	if (find_vertex(v)) {
		deps=gra_out[v];
		return deps.size();
	}
	else
		return -1;
};

// Find degree information.
// Find the number of degrees for a given vertex.
long conet::GraphFDS::find_degree(NodeID v, bool neighbor) const {
	// We can define it as below.
	if (find_vertex(v)) {
		if (neighbor) {
			NodeIDSSet deps;
			for (NodeIDLinkIDMMap::const_iterator i=gra_out[v].begin();\
				i!=gra_out[v].end();i++)
				if (i->first!=v) deps.insert(i->first);
			for (NodeIDLinkIDMMap::const_iterator j=gra[v].begin();\
				j!=gra[v].end();j++)
				if (j->first!=v) deps.insert(j->first);
			return deps.size();;
		}
		else 
			return this->find_in_deg(v)+this->find_out_deg(v);
	}
	else
		return -1;
};
long conet::GraphFDS::find_in_deg(NodeID v, bool neighbor) const {
	if (find_vertex(v)) {
		if (neighbor) {
			long count=0;
			NodeID last=-1;
			for (NodeIDLinkIDMMap::const_iterator i=gra[v].begin();i!=gra[v].end();i++) {
				if (i->first!=v && i->first!=last) count++;
				last=i->first;
			};
			return count;
		}
		else 
			return gra[v].size();
	}
	else
		return -1;
};
long conet::GraphFDS::find_out_deg(NodeID v, bool neighbor) const {
	if (find_vertex(v)) {
		if (neighbor) {
			long count=0;
			NodeID last=-1;
			for (NodeIDLinkIDMMap::const_iterator i=gra_out[v].begin();i!=gra_out[v].end();i++) {
				if (i->first!=v && i->first!=last) count++;
				last=i->first;
			};
			return count;
		}
		else
			return gra_out[v].size();
	}
	else
		return -1;
};

// Find the number of edges between neighbors.
// Returns the number of edges and number of neighbors through argument.
long conet::GraphFDS::find_neighbor_connections(NodeID id, long &degree) const {
	NodeIDMSet deps;
	degree=this->find_dep(id,deps,true);
	if (degree>1) {
		long num_links=0;
		for (NodeIDMSet::const_iterator i=deps.begin();i!=deps.end();i++) {
			NodeID last=-1;
			for (NodeIDLinkIDMMap::const_iterator j=gra_out[*i].begin();j!=gra_out[*i].end();j++) {
				// Loops and multilinks are ignored
				if (j->first!=*i && j->first!=last && deps.find(j->first)!=deps.end())
					num_links++;
				last=j->first;
			};
		};
		return num_links;
	}
	else 
		return 0;
};

long conet::GraphFDS::find_in_neighbor_connections(NodeID id, long &degree) const {
	NodeIDMSet deps;
	degree=this->find_in_dep(id,deps,true);
	if (degree>1) {
		long num_links=0;
		for (NodeIDMSet::const_iterator i=deps.begin();i!=deps.end();i++) {
			NodeID last=-1;
			for (NodeIDLinkIDMMap::const_iterator j=gra_out[*i].begin();j!=gra_out[*i].end();j++) {
				// Loops and multilinks are ignored
				if (j->first!=*i && j->first!=last && deps.find(j->first)!=deps.end())
					num_links++;
				last=j->first;
			};
		};
		return num_links;
	}
	else 
		return 0;
};

long conet::GraphFDS::find_out_neighbor_connections(NodeID id, long &degree) const {
	if (find_vertex(id)) {
		NodeIDMSet deps;
		degree=this->find_out_dep(id,deps,true);
		if (degree>1) {
			long num_links=0;
			for (NodeIDMSet::const_iterator i=deps.begin();i!=deps.end();i++) {
				NodeID last=-1;
				for (NodeIDLinkIDMMap::const_iterator j=gra_out[*i].begin();j!=gra_out[*i].end();j++) {
					// Loops and multilinks are ignored
					if (j->first!=*i && j->first!=last && deps.find(j->first)!=deps.end())
						num_links++;
					last=j->first;
				};
			};
			return num_links;
		}
		else return 0;
	}
	else 
		return -1;
};

// Find the degree correlation coefficient (loop and multilink not ignored).
double conet::GraphFDS::find_degree_correlation(bool neighbor) const {
	double sum1=0, sum2=0, sum3=0, sum4=0, sum5=0;
	long deg_in, deg_out;
	// Edge summation.
	for (NodeID v=0;v<nvertices;v++)
		for (NodeIDLinkIDMMap::const_iterator i=gra_out[v].begin();i!=gra_out[v].end();i++) {
			deg_in=this->find_in_deg(i->first,neighbor);
			deg_out=this->find_out_deg(v,neighbor);
			sum1+=deg_out*deg_in;
			sum2+=deg_out;
			sum3+=deg_out*deg_out;
			sum4+=deg_in;
			sum5+=deg_in*deg_in;
		};
	sum1/=nedges;
	sum2/=nedges;
	sum3/=nedges;
	sum4/=nedges;
	sum5/=nedges;

	double var_out=sum3-sum2*sum2, var_in=sum5-sum4*sum4;
	if (var_out<1.0e-8 || var_in<1.0e-8) return 100;
	return (sum1-sum2*sum4)/(sqrt(var_out)*sqrt(var_in));
};

// Print the graph showing all vertices and edges.
void conet::GraphFDS::print() const {
	cout << "=== vertices ===\n";
	cout << "There are " << nvertices << " vertices";
	if (nvertices) cout << ", from 0 to " << nvertices-1;
	cout << ".\n";
	cout << "=== edges ===\n";
	cout << "There are " << get_edge_size() << " directional inward edges (des: ori1 (ID1), ori2 (ID2), ori3 (ID3),...).\n";
	for (long j=0; j<nvertices; j++) {
		cout << j << ": ";
		for (NodeIDLinkIDMMap::const_iterator i=gra[j].begin();\
			i!=gra[j].end();i++) {
			cout << i->first << " (" << i->second << "), ";
		};
		cout << endl;
	};
};

void conet::GraphFDS::write1(ostream &of, long time) const {
	for (long j=0; j<nvertices; j++) {
		for (NodeIDLinkIDMMap::const_iterator i=gra[j].begin();\
			i!=gra[j].end();i++) {
			if (time>=0) of << time << '\t';
			of << i->second << '\t' << i->first << '\t' << j << '\n';
		};
	};
};

void conet::GraphFDS::write2(ostream &of, long time) const {
	for (long j=0; j<nvertices; j++) {
		if (time>=0) of << time << '\t';
		of << j;
		for (NodeIDLinkIDMMap::const_iterator i=gra[j].begin();\
			i!=gra[j].end();i++) {
			of << '\t' << i->first << '\t' << i->second;
		};
		of << '\n';
	};
};

// This is for Pajek output.
void conet::GraphFDS::write3(ostream &of) const { 
	of << "*Vertices " << nvertices << '\n';
	for (long j=0; j<nvertices; j++)
		of << j+1 << " \"" << j+1 << "\"\n";
	of << "*Arcs\n";
	for (long j=0; j<nvertices;j++)
		for (NodeIDLinkIDMMap::const_iterator i=gra[j].begin();\
			i!=gra[j].end();i++) {
			of << i->first +1 << ' ' << j+1 << ' ' << i->second+1 << '\n';
		};
};

void conet::GraphFDS::write_edges(ostream &of) const {
	of << "*Arcs\n";
	for (long j=0; j<nvertices;j++)
		for (NodeIDLinkIDMMap::const_iterator i=gra[j].begin();\
			i!=gra[j].end();i++) {
			of << i->first +1 << ' ' << j+1 << ' ' << i->second << '\n';
		};
};

void conet::GraphFDS::write_degrees(ostream &of) const {
	for (long j=0; j<nvertices;j++) {
		long in_deg=gra[j].size();
		long out_deg=gra_out[j].size();
		of << j << '\t' << in_deg+out_deg << '\t' << in_deg << '\t' << out_deg << '\n';
	};
};

// Read the graph info from a file (with ID's)
// Assume that the # of agents are already fixed.
bool conet::GraphFDS::read2(std::istream &ifile) {
	char tmpstr[MAX_CHARS+1];
	// Read the number of vertices, link_type, loop, multi_link
	// Vertices will be numbered from 0 to nvertices-1.
	while (ifile.peek()=='#')
		ifile.getline(tmpstr, MAX_CHARS, '\n');
	ifile >> nvertices >> edge_type >> loop >> multi_link;
	ifile.getline(tmpstr, MAX_CHARS, '\n');

	// Erase and make graph with no edges.
	this->clear_edges();
	gra.resize(nvertices);

	// Add edges.
	NodeID ori, des;
	LinkID id;
	do {
		if(ifile.peek()!='#' && ifile >> ori >> des >> id)
			this->add_edge_ID(ori,des,id);
	} while (ifile.getline(tmpstr, MAX_CHARS, '\n'));
	return true;
};
