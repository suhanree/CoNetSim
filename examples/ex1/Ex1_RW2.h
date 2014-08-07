// 
//	CONETSIM - Ex1_RW2.h
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

// Example 1 for conet library.
// Random-walking nodes (fixed number) in a periodic 2D lattice with opinions (0<op<1).
// There are short-range nearest-neighbor interaction, along with 
// long-range fixed interactions, both of which are represented
// by state-less links.

// One Node staet, with no Link state (but two types of links), and no NodeInput state.


#ifndef EX1_RW2_H
#define EX1_RW2_H

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include "Node.h"
#include "Nodes.h"
#include "Link.h"
#include "Links.h"
#include "Graph.h"
#include "GraphFB.h"
#include "GraphFD.h"
#include "GraphFDS.h"
#include "Graphs.h"
#include "Network.h"
#include "Point.h"
#include "Errors.h"
#include "Utilities.h"
#include "Types.h"
#include "Input.h"
#include "Output.h"
#include "RW2Network.h"

using namespace conet;

//===========<(1) Define ParameterSet struct>==========================================
// structure for parameters.
struct ParameterSet {
	NodeID nnodes;
	long xsize;
	long ysize;
	short geometry;
	string in_excluded_file;
	double op_threshold;
	short opinion_topology;
	short op_init_method;
	double coupling_constant;
	long int_range;
	int neighbor_type_sg;
	long node_max_speed;
	int neighbor_type_movement;
	long update_method;
	long total_time;

	unsigned long rseed1;
	unsigned long rseed2;
	long max_rn;
	int convergence_method;
	long convergence_check_period;
	long convergence_out_period;

	bool if_snapshot_out;
	string out_snapshot_file;
	long out_snapshot_interval;

	bool if_ag_snapshot_out;
	string out_ag_snapshot_file;
	int out_ag_snapshot_method;
	long out_ag_snapshot_interval;

	bool if_sg_snapshot_out;
	string out_sg_snapshot_file;
	int out_sg_snapshot_method;
	long out_sg_snapshot_interval;

	bool if_node_out;
	NodeID node_id;
	string out_node_file;
	long out_node_interval;

	bool if_summary_out;
	string out_summary_file;

	bool if_network_stat_out;
	long out_network_stat_time;
	bool if_components_out;
	string out_components_file;

	bool if_agraph_in;
	string in_agraph_file;
	short in_agraph_method;
	long num_edges;
	double wiring_prob;

	bool if_nodes_in;
	string in_nodes_file;

	// constructor for assgining default values.
	ParameterSet() {
		geometry=1;
		op_threshold=0.5;
		opinion_topology=2;
		op_init_method=1;
		int_range=0;
		neighbor_type_sg=0;
		node_max_speed=1;
		neighbor_type_movement=1;
		update_method=1;
	
		rseed1=1;
		rseed2=1;
		max_rn=1000;
		convergence_method=1;
		convergence_check_period=10;
		convergence_out_period=10;
	
		if_snapshot_out=false;
		out_snapshot_interval=1;
	
		if_ag_snapshot_out=false;
		out_ag_snapshot_method=0;
		out_ag_snapshot_interval=1;
	
		if_sg_snapshot_out=false;
		out_sg_snapshot_method=0;
		out_sg_snapshot_interval=1;
	
		if_node_out=false;
		out_node_interval=1;
	
		if_summary_out=false;
	
		if_network_stat_out=false;
		out_network_stat_time=-1;
		if_components_out=false;
	
		if_agraph_in=false;
		in_agraph_method=0;
		num_edges=0;
		wiring_prob=0;
	
		if_nodes_in=false;
	};

	void error_check() const;
};
	
// Derived class for the InputParam for reading all inpus parameters.
class InputParamRW2: public InputParam {
   public:
   	// constructor
   	InputParamRW2(const std::string &infile, ParameterSet &p): InputParam(infile,p) {};
	// destructor
	~InputParamRW2() {};

	// Read all parameters from the given file and store in the ParameterSet structure.
	// This member function has to be overwritten in a subclass to use this function.
	// "struct ParameterSet" has to be defined first.
	// Here InputParam::get() and InputParam::read() will be used to assign values to the parameters.
	void read_all_parameters();

};

//===========<(2) add definitions and some utility functions for this system>==========================================
// For exception handling.
// Bad neighborhood type.
struct NeighborTypeError {};

// Some utility functions.

// Print the information about all nodes graphically.
// This function should be used for (xsize, ysize < 30~40).
void print_node_screen(RW2Nodes &nodes);

// Checking positions of nodes from a file to find out if some positions are the same.
bool check_nodes_pos(RW2NodesBase &nodes);

// A function to check the convergence of the system at the given time.
//	when method=1, using the opinions of all nodes (comparing opinions at two subsequent time steps).
// 	method=4~7 (special case to find the transition from period-i to period-(i-1), i=method-2) 
// 	method=10 (special case to find the transition from period-2 to period-1 to period-0 eventually) 
bool check_convergence(RW2NodesBase &nodes, TimeType t, const Graphs &graphs, short method=1);

// A function to find the absolute difference between two opinions when opinions are linear(1) or circular(2).
double find_op_diff(double a, double b, short topology);
// A function to find the difference between two opinions when opinions are linear(1) or circular(2).
double find_op_diff2(double a, double b, short topology);

// Finding groups of opinions based on opinions only
long find_opiniongroups(RW2NodesBase &nodes, NodeType nt, TimeType t, std::vector<std::multiset<double> > & opiniongroups,\
	short topology, double thres);

// Find the average diff of opinions between consecutive locations (location-wise)
//	xory=1: x, xory=2: y direction
double find_av_diff(RW2NodesBase &nodes, NodeType nt, TimeType t, short topology, long meshsize, short xory=1);

// Find the maximum min-max width of p at each x or y.
double find_op_width(RW2NodesBase &nodes, NodeType nt, TimeType t, short topology, short xory=1);

// Find the average diff of x-location between consecutive opinions
double find_av_diff_p(RW2NodesBase &nodes, NodeType nt, TimeType t, short topology, long meshsize, double op_thres);

// Initialize nodes.
bool initialize_nodes_states_file(RW2NodesBase &nodes, NodeType nt, TimeType t, std::string filename);
bool initialize_nodes_states_random(RW2NodesBase &nodes, NodeType nt, TimeType t, short op_init_method, short geometry, PointSet &excluded);

#endif
