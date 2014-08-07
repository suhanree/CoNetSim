// 
//	CONETSIM - Utilities.h
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

#ifndef UTILITIES_H
#define UTILITIES_H

#include "Types.h"
#include "Graph.h"
#include "Link.h"
#include "Graphs.h"
#include "Links.h"
#include "Errors.h"
#include "Random4.h"
#include "Point.h"

#include <iostream>
#include <istream>
#include <ostream>
#include <vector>
#include <list>
#include <set>
#include <string>
#include <map>
#include <sys/stat.h>

namespace conet {

// Print the vector of long.
void print_vector(const std::vector<long> &v);
void print_vector(const std::vector<double> &v);

// Print the list of long.
void print_list(const std::list<long> &li);

// Print the set of long.
void print_set(const std::set<long> &s);
void print_set(const std::multiset<long> &s);
void print_set(const PointSet &s);
     
// Print the map of long and double.
void print_map(const std::map<long,double> &m);
void print_map(const std::multimap<long,double> &m);

// Print the map of long and long.
void print_map(const std::map<long,long> &m);
void print_map(const std::multimap<long,long> &m);

// Check if a file exists.
bool if_file(std::string &filename);

// Find the cdf from the pdf (to be used in Random module).
void find_cdf(const std::vector<double> &pdf, std::vector<double> &cdf); 
void find_cdf(const std::vector<long> &pdf, std::vector<double> &cdf); 

class Links; // Forward declaration

// Initializing a graph structure as an ER random graph. 
//	when num_edges is given, the probability a given edge is added is
//		num_edges/(# of all possible edges)
// when nvertices<4e+4, num_edges will be exactly the same as the given value, but
//	otherwise wiring_prob will be used and num_edges will be the approximate value.
// When link states exist, this function will generate link states as a default value,
//	and also generate link ID's from 0 to nedges-1.
bool initialize_random_graph1(LinkType lt, Graphs &graphs, Links &links, long num_edges, unsigned long rseed);
bool initialize_random_graph2(LinkType lt, Graphs &graphs, Links &links, double wiring_prob, unsigned long rseed);
bool initialize_full_graph(LinkType lt, Graphs &graphs, Links &links);

// Each vertex originates an edge with randomly chosen destination 
//	(not exactly the ER random graph, but a similar random graph
//	since the out-degree will be the same for all vertices.)
bool initialize_random_graph3(LinkType lt, Graphs &graphs, Links &links, long fixed_out_degree, unsigned long rseed);

// Combine two graphs (if ag is directed, then change it to undirected)
bool combine_graphs(GraphFB &cg, const GraphFB &sg, const GraphF &ag);

// Write the components (assuming components are already found as a vector).
void write_components(const std::vector<NodeIDSSet> &components, std::ostream &of);

// get the max out of 2 or 3 integers.
long find_max(long i1, long i2, long i3);
long find_max(long i1, long i2);

}; // End of namespace conet.

#endif
