// 
//	CONETSIM - Ex1Network.h
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

#ifndef EX1NETWORK_H
#define EX1NETWORK_H

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
#include "NodeInput.h"
#include "NodeInputs.h"
#include "Network.h"
#include "Graphs.h"
#include "Point.h"
#include "TimeSeq.h"
#include "Errors.h"
#include "Utilities.h"
#include "Types.h"
#include "Input.h"
#include "Output.h"

using namespace conet;

//===========<(1) Defining the states>==================================================
// A class that defines a Node state.
class RW2State {
   public :
   	// Constructors.
   	RW2State(): pos(0,0), op(0.5) {};
	RW2State(long ix, long iy): pos(ix,iy), op(0.5) {};
	RW2State(const Point2DTorus &pos1): pos(pos1), op(0.5) {};
	RW2State(long ix, long iy, double op1): pos(ix,iy) { 
		update_op(op1);
	};
	RW2State(const Point2DTorus & pos1, double op1): pos(pos1) {
		update_op(op1);
	};

	// Destructor.
	~RW2State() {};

	// Change x and y values.
	void update_x(long lx) {
		pos.x=lx;
		pos.get_right();
//		(pos.x>=0? pos.x%=pos.tor.xsize : pos.x=pos.tor.xsize+pos.x%pos.tor.xsize);
	};
	void update_y(long ly) {
		pos.y=ly;
		pos.get_right();
//		(pos.y>=0? pos.y%=pos.tor.ysize : pos.y=pos.tor.ysize+pos.y%pos.tor.ysize);
	};
	void update_pos(long ix, long iy) {
		pos.x=ix; pos.y=iy;
		pos.get_right();
//		pos.torus();
	};
	void update_pos(const Point2DTorus & pos1) {
		pos.x=pos1.x; pos.y=pos1.y;
		pos.get_right();
//		pos.torus();
	};

	// Change op
	void update_op(double op1) {
		double epsilon=0.000001;
		if (op1<=0.0-epsilon)
			op=0.0;
		else if (op1>=1.0+epsilon) 
			op=1.0;
		else
			op=op1;
	};

	// Update the whole state.
	void update(long ix, long iy, double op1) {
		update_pos(ix,iy);
		update_op(op1);
	};
	void update(const RW2State & st) {
		pos.x=st.get_pos().x; pos.y=st.get_pos().y; op=st.get_op(); 
	};
	
	// Read the point from a file.
	bool read(std::ifstream &ifile) {
		bool tf = (ifile >> pos.x >> pos.y >> op);
		pos.get_right();
//		pos.torus();
		if (op<=0 || op>=1)
			return false;
		return tf;
	};

	// Get the node postition.
	Point2DTorus get_pos() const {
		return pos;
	};

	// Get the node values.
	double get_op() const {
		return op;
	};

	// Get the topology of the opinion space.
	static short get_op_topology() {
		return opinion_topology;
	};

	// Set the topology of the opinion space.
	static void set_op_topology(short topo) {
		opinion_topology=topo;
	};

	// Print the values.
	void print() const {
		std::cout << '(' << pos.x << ',' << pos.y << ')' << '\t' << op << '\n';
	};
	void write(std::ofstream &of) const {
		of << pos.x << '\t' << pos.y << '\t' << op << '\n';
	};

   private :
   	Point2DTorus	pos; // Position of the node.
	double op;	// Opinion of the node. (0<op<1)
	
	// Topology of the opinion space. (1 when linear, 2 when circular (periodic)).
	static short opinion_topology;
};

// A class that defines the "less than" function for the RW2State class.
class RW2StateLessThan {
public :
	inline bool operator() (const RW2State & st1, const RW2State & st2) {
		return (st1.get_pos().y==st2.get_pos().y ? \
			st1.get_pos().x < st2.get_pos().x : st1.get_pos().y < st2.get_pos().y);
	};
};

// instream operator overloading for RW2State.
inline std::istream & operator>>(std::istream &s, RW2State &st) {
	long x, y;
	double op;
	s >> x >> y >> op;
	st.update_pos(x,y);
	st.update_op(op);
	return s;
};

// outstream operator overloading for RW2State.
inline std::ostream & operator<<(std::ostream &s, const RW2State &st) {
	return s << st.get_pos().x << '\t' << st.get_pos().y << '\t' \
		<< st.get_op();
};

//===========<(2) Choose Node, Link, Nodes, Links, and Graphs>===============
// Node for RW2.
typedef NodeVR<RW2State> RW2Node;

// Nodes for RW2.
typedef NodesNF1S<RW2State> RW2NodesBase;
typedef NodesNF_VR<RW2State> RW2Nodes;

// Link for RW2.
// 	not necessary because links here are state-less with max_delay=1.
typedef LinksPlain RW2Links;

// Graphs for Ex1.
// 	not defined here. It will be Graphs_Ex4, or Graphs_Ex5

//========<(3) Defining the Ex1Network class>=========================================
// Network for Ex1
class Ex1Network: public Network2<RW2State> {
   public:
   	// Constructor.
	Ex1Network(RW2Nodes &nn, RW2Links &ll, Graphs &gg, ParameterSet &p);

	// Destructor.
	~Ex1Network();
	
	// Initialize nodes.
	void initialize_nodes();

	// Initialize links and graphs.
	void initialize_links();

	// Create nodes.
	void create_nodes(NodeType nt, TimeType t);

	// Update node states (rule-based).
	void update_node_states(NodeType nt, TimeType t);

	// Update nodes (rule-based).
	void update_nodes(TimeType t) {
		update_node_states(0,t);
	};

	// Create links (rule-based).
	void create_links(LinkType lt, TimeType t);

	// Remove links (rule-based).
	void remove_links(LinkType lt, TimeType t);

	// Update links (rule-based).
	void update_links(TimeType t) {
		remove_links(0,t);
		create_links(0,t);
	};

	// write outputs (if necessary).
	void write_nodes_T(Output &outputs, TimeType t, short method=0);
	void write_output(TimeType t);
	void write_summary(TimeType t, bool if_converged);

   public:
	// For outputs (a string named "Output::noout" will be used when there will be no output).
	Output nodes_out;
	Output nodes_out_p;
	Output ag_out;
	Output sg_out;
	Output one_node_out;
	Output summary_out;

   private:
	// For writing the network stat.
   	void write_network_stat(TimeType t);

	// print neighbors (for SG)
	void print_neighbor_states() const { 
		print_set(neighbor_states);
	};

	// Print the occupancy information. (for SG)
	void print_occupancy() const;

	// Update the occupancy for new postitions. (for SG)
	void update_occupancy(TimeType t);

	// Find the neighboring agents based on a given position (including myself). (for SG)
	void find_neighbors(const Point2DTorus &p);

   	// A function that finds the index from the Point. (for SG)
	long find_index(const Point2DTorus &p) {
		return p.x+p.y*Point2DTorus::get_xsize();
	};

	// Find the next state of a node. (for node update)
	RW2State next_state(NodeID id, TimeType t);
	
	// A function to find the movement of an opinion by other opinions (depends on update_method) (for node update).
	double find_op_movement(double myop, long size);

   private : 
   	// Set of excluded points. (for NodeState)
	PointSet excluded;

   	// Set of neighboring points (for SG)
	PointSet neighbor_states;
	// Set of neighbors. (for SG)
	NodeIDSSet neighbors;
	// A vector that contains the occupancy information of the state space. (for SG)
	// This should be updated whenever a node changes its state.
	vector<NodeIDSSet> occupancy;

	// Set of relative transition points (represented by relative positions) (for node update)
	PointSet rel_trans;
	// Set of points (not relative) that can be used for transition. (for node update)
	PointSet transitions; 
	// Set of opinions (used as a temporary storage). (for node update)
	std::valarray<double> ops;
	
};

#endif
