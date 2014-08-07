// 
//	CONETSIM - Ex2.C
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

// Example 2 for conet library.
// Random-walking nodes in a periodic 2D lattice with node states (0<phi<1).
// There are short-range nearest-neighbor interaction, along with 
// long-range varying interactions, both of which are represented
// by state-less links.

// One Node staet (one type of nodes), with no Link state (but two types of links), and no NodeInput state.


#include <iostream>
#include <fstream>
#include <cstdlib>
#include "Ex2.h"
#include "Types.h"

static double epsilon=0.000001; // A small number for comparing double's.

using namespace std;
using namespace conet;
	
// checking errors for some parameter values.
void ParameterSet::error_check() const {
	if (nnodes<=0) {
		cerr << "# Bad parameter value: nnodes\n";
		exit(1);
	};
	if (double(nnodes)*(nnodes-1)*wiring_prob*0.9 > 2.1474836475e+9) {
		cerr << "# Bad parameter value: nnodes (too big).\n";
		exit(1);
	};
	if (xsize<=0) {
		cerr << "# Bad parameter value: xsize\n";
		exit(1);
	};
	if (ysize<=0) {
		cerr << "# Bad parameter value: ysize\n";
		exit(1);
	};
	if (double(xsize)*ysize > 2.1474836475e+9 ) {
		cerr << "# Bad parameter value: xsize and ysize (too big).\n";
		exit(1);
	};
	if (coupling_constant<-1.0-epsilon) {
		cerr << "# Bad parameter value: coupling_constant\n";
		exit(1);
	};
	if (node_state_threshold<-epsilon || node_state_threshold > 1.0+epsilon) {
		cerr << "# Bad parameter value: node_state_threshold.\n";
		exit(1);
	};
	if (node_state_topology<1 || node_state_topology > 2) {
		cerr << "# Bad parameter value: node_state_topology.\n";
		exit(1);
	};
	if (node_state_init_method<1 || node_state_init_method > 2) {
		cerr << "# Bad parameter value: node_state_init_method.\n";
		exit(1);
	};
	//if (nnodes>xsize*ysize) {
		//cerr << "# Bad parameter value: nnodes\n";
		//cerr << "# Too many nodes...\n";
		//exit(1);
	//};
	if (neighbor_type_sg!=1 && neighbor_type_sg!=3) {
		cerr << "# Bad parameter value: neighbor_type_sg\n";
		exit(1);
	};
	if (node_max_speed<0) {
		cerr << "# Bad parameter value: node_max_speed\n";
		exit(1);
	};
	if (total_time<0) {
		cerr << "# Bad parameter value: total_time\n";
		exit(1);
	};
	if (num_edges<0 && (wiring_prob<0 || wiring_prob>1)) {
		cerr << "# Bad parameter value: wiring_prob=" << wiring_prob << '\n';
		exit(1);
	};
	if (rseed1==0) {
		cerr << "# Bad parameter value: rseed1\n";
		exit(1);
	};
	if (rseed2==0) {
		cerr << "# Bad parameter value: rseed2\n";
		exit(1);
	};
	if (max_rn==0) {
		cerr << "# Bad parameter value: max_rn\n";
		exit(1);
	};
};

// Read in all parameters.
void InputParamEx2::read_all_parameters() {
	// Read in all given parameter key-value pairs given by the input file.
	read();
	// print_key_val();

	// Assign values to parameters.
	if (!get("nnodes",param.nnodes,cerr)) {
		cerr << "# No parameter given: nnodes \n";
		cerr << endl;
		exit(1);
	};
	if (!get("xsize",param.xsize,cerr)) {
		cerr << "# No parameter given: xsize \n";
		exit(1);
	};
	if (!get("ysize",param.ysize,cerr)) {
		cerr << "# No parameter given: ysize \n";
		exit(1);
	};
	if (!get("geometry",param.geometry,cerr)) {
		cerr << "# No parameter given: geometry.";
		cerr << " Default value 1 (regular) will be used.\n";
	};
	if (param.geometry==2 && \
		!get("in_excluded_file",param.in_excluded_file,cerr)) {
		cerr << "# No parameter given: in_excluded_file \n";
		exit(1);
	};
	if (!get("coupling_constant",param.coupling_constant,cerr)) {
		cerr << "# No parameter given: coupling_constant \n";
		exit(1);
	};
	if (!get("node_state_threshold",param.node_state_threshold,cerr)) {
		cerr << "# No parameter given: node_state_threshold";
		cerr << ". Default value, " << param.node_state_threshold;
		cerr << ",  will be used.\n";
	};
	if (!get("node_state_topology",param.node_state_topology,cerr)) {
		cerr << "# No parameter given: node_state_topology";
		cerr << ". Default value 2 (circular) will be used.\n";
	};
	if (!param.if_nodes_in && !get("node_state_init_method",param.node_state_init_method,cerr)) {
		cerr << "# No parameter given: node_state_init_method";
		cerr << ". Default value 1 (random) will be used.\n";
	};
	if (!get("neighbor_type_sg",param.neighbor_type_sg,cerr)) {
		cerr << "# No parameter given: neighbor_type_sg";
		cerr << ". Default value 0 (no SG effect) will be used.\n";
	};
	if (!get("int_range",param.int_range,cerr)) {
		cerr << "# No parameter given: int_range";
		cerr << ". Default value 0 will be used.\n";
	};
	if (!get("node_max_speed",param.node_max_speed,cerr)) {
		cerr << "# No parameter given: node_max_speed";
		cerr << ". Default value 1 will be used.\n";
	};
	if (!get("neighbor_type_movement",param.neighbor_type_movement,cerr)) {
		cerr << "# No parameter given: neighbor_type_movement";
		cerr << ". Default value 1 (von Neumann+center) will be used.\n";
	};
	if (!get("update_method",param.update_method,cerr)) {
		cerr << "# No parameter given: update_method";
		cerr << ". Default value 2 (new) will be used.\n";
	};
	if (!get("total_time",param.total_time,cerr)) {
		cerr << "# No parameter given: total_time \n";
		exit(1);
	};
	double drseed1, drseed2, dmax=2147483647;
	if (!get("rseed1",drseed1,cerr)) {
		cerr << "# No parameter given: rseed1";
		cerr << ". Default value, " << param.rseed1 << ", will be used.\n";
	}
	else {
		if (drseed1 < 0) drseed1=-drseed1;
		param.rseed1=(unsigned long) (drseed1-dmax*((unsigned long) (drseed1/dmax))+0.5);
	};
	if (!get("rseed2",drseed2,cerr)) {
		cerr << "# No parameter given: rseed2";
		cerr << ". Default value, " << param.rseed2 << ", will be used.\n";
	}
	else {
		if (drseed2 < 0) drseed2=-drseed2;
		param.rseed2=(unsigned long) (drseed2-dmax*((unsigned long) (drseed2/dmax))+0.5);
	};
	if (!get("max_rn",param.max_rn,cerr)) {
		cerr << "# No parameter given: max_rn";
		cerr << ". Default value, " << param.max_rn << ", will be used.\n";
	};
	if (!get("convergence_method",param.convergence_method,cerr)) {
		cerr << "# No parameter given: convergence_method";
		cerr << ". Default value, 1, will be used.\n";
	};
	if (param.convergence_method && !get("convergence_check_period",param.convergence_check_period,cerr)) {
		param.convergence_check_period=param.xsize*param.ysize;
		cerr << "# No parameter given: convergence_check_period.\n";
		cerr << "  Default value, xsize*ysize=" << param.convergence_check_period;
		cerr << ", will be used.\n";
	};
	if (param.convergence_method>3 && !get("convergence_out_period",param.convergence_out_period,cerr)) {
		param.convergence_out_period=param.convergence_check_period;
		cerr << "# No parameter given: convergence_out_period.\n";
		cerr << "  Default value, convergence_out_period, " << param.convergence_check_period;
		cerr << ", will be used.\n";
	};
	if (!get("if_snapshot_out",param.if_snapshot_out,cerr)) {
		cerr << "# No parameter given: if_snapshot_out";
		cerr << ". Default value, false, will be used.\n";
	};
	if (param.if_snapshot_out && \
		!get("out_snapshot_file",param.out_snapshot_file,cerr)) {
		cerr << "# No parameter given: out_snapshot_file \n";
		exit(1);
	};
	if (param.if_snapshot_out && !get("out_snapshot_interval", \
		param.out_snapshot_interval,cerr)) {
		cerr << "# No parameter given: out_snapshot_interval";
		cerr << ". Default value 1 will be used.\n";
	};
	if (!get("if_ag_snapshot_out",param.if_ag_snapshot_out,cerr)) {
		cerr << "# No parameter given: if_ag_snapshot_out";
		cerr << ". Default value, false, will be used.\n";
	};
	if (param.if_ag_snapshot_out && !get("out_ag_snapshot_file",\
		param.out_ag_snapshot_file,cerr)) {
		cerr << "# No parameter given: out_ag_snapshot_file \n";
		exit(1);
	};
	if (param.if_ag_snapshot_out && !get("out_ag_snapshot_method",\
		param.out_ag_snapshot_method,cerr)) {
		cerr << "# No parameter given: out_ag_snapshot_method";
		cerr << ". Default value 0 will be used.\n";
	};
	if (param.if_ag_snapshot_out && !get("out_ag_snapshot_interval",\
		param.out_ag_snapshot_interval,cerr)) {
		cerr << "# No parameter given: out_ag_snapshot_interval";
		cerr << ". Default value 1 will be used.\n";
	};
	if (!get("if_sg_snapshot_out",param.if_sg_snapshot_out,cerr)) {
		cerr << "# No parameter given: if_sg_snapshot_out";
		cerr << ". Default value, false, will be used.\n";
	};
	if (param.if_sg_snapshot_out && !get("out_sg_snapshot_file",\
		param.out_sg_snapshot_file,cerr)) {
		cerr << "# No parameter given: out_sg_snapshot_file \n";
		exit(1);
	};
	if (param.if_sg_snapshot_out && !get("out_sg_snapshot_method",\
		param.out_sg_snapshot_method,cerr)) {
		cerr << "# No parameter given: out_sg_snapshot_method";
		cerr << ". Default value 0 will be used.\n";
	};
	if (param.if_sg_snapshot_out && !get("out_sg_snapshot_interval",\
		param.out_sg_snapshot_interval,cerr)) {
		cerr << "# No parameter given: out_sg_snapshot_interval";
		cerr << ". Default value 1 will be used.\n";
	};
	if (!get("if_node_out",param.if_node_out,cerr)) {
		cerr << "# No parameter given: if_node_out";
		cerr << ". Default value, false, will be used.\n";
	};
	if (param.if_node_out && !get("node_id",param.node_id,cerr)) {
		cerr << "# No parameter given: node_id \n";
		exit(1);
	};
	if (param.if_node_out && !get("out_node_file",param.out_node_file,cerr)) {
		cerr << "# No parameter given: out_node_file \n";
		exit(1);
	};
	if (param.if_node_out && !get("out_node_interval",\
		param.out_node_interval,cerr)) {
		cerr << "# No parameter given: out_node_interval";
		cerr << ". Default value 1 will be used.\n";
	};
	if (!get("if_summary_out",param.if_summary_out,cerr)) {
		cerr << "# No parameter given: if_summary_out";
		cerr << ". Default value, false, will be used.\n";
	};
	if (param.if_summary_out && !get("out_summary_file",\
		param.out_summary_file,cerr)) {
		cerr << "# No parameter given: out_summary_file \n";
		exit(1);
	};
	if (param.if_summary_out && !get("if_network_stat_out",param.if_network_stat_out,cerr)) {
		cerr << "# No parameter given: if_network_stat_out";
		cerr << ". Default value, false, will be used.\n";
	};
	if (param.if_summary_out && param.if_network_stat_out && !get("out_network_stat_time",\
		param.out_network_stat_time,cerr)) {
		cerr << "# No parameter given: out_network_stat_time";
		cerr << ". Default value, -1, will be used.\n";
	};
	if (param.if_summary_out && param.if_network_stat_out && !get("if_components_out",param.if_components_out,cerr)) {
		cerr << "# No parameter given: if_components_out";
		cerr << ". Default value, false, will be used.\n";
	};
	if (param.if_summary_out && param.if_network_stat_out && param.if_components_out && !get("out_components_file",\
		param.out_components_file,cerr)) {
		cerr << "# No parameter given: out_components_file \n";
		exit(1);
	};
	if (!get("if_agraph_in",param.if_agraph_in,cerr)) {
		param.if_agraph_in=false;
		cerr << "# No parameter given: in_agraph_in.";
		cerr << "  Default value, false, will be used.\n";
	};
	if (param.if_agraph_in) {
		if (get("in_agraph_file",param.in_agraph_file,cerr)) {
			if (!if_file(param.in_agraph_file)) {
				cerr << "# File, " << param.in_agraph_file << ", doesn't exist.";
				cerr << "  AG will be created randomly.\n";
				param.if_agraph_in=false;
			};
		}
		else {
			cerr << "# No parameter given: in_agraph_file.";
			cerr << "  AG will be created randomly.\n";
			param.if_agraph_in=false;
		};
	};
	if (!param.in_agraph_method && !get("in_agraph_method",param.in_agraph_method,cerr)) {
		cerr << "# No parameter given: in_agraph_method";
		cerr << "  Default value, 0, will be used.\n";
	};
	if (!param.if_agraph_in && !get("num_edges",param.num_edges,cerr)) {
		cerr << "# No parameter given: num_edges \n";
		exit(1);
	};
	if (!param.if_agraph_in && param.num_edges<0 && !get("wiring_prob",param.wiring_prob,cerr)) {
		cerr << "# No parameter given: wiring_prob \n";
		exit(1);
	};
	if (!get("if_nodes_in",param.if_nodes_in,cerr)) {
		param.if_nodes_in=false;
		cerr << "# No parameter given: in_nodes_in.";
		cerr << "  Default value, false, will be used.\n";
	};
	if (param.if_nodes_in) {
		if (get("in_nodes_file",param.in_nodes_file,cerr)) {
			if (!if_file(param.in_nodes_file)) {
				cerr << "# File, " << param.in_nodes_file << ", doesn't exist.";
				cerr << "  Nodes will be created randomly.\n";
				param.if_nodes_in=false;
			};
		}
		else {
			cerr << "# No parameter given: in_nodes_file.";
			cerr << "  Nodes will be created randomly.\n";
			param.if_nodes_in=false;
		};
	};
	if (!get("ag_threshold",param.ag_threshold,cerr)) {
		param.ag_threshold=0;
		cerr << "# No parameter given: ag_threshold.";
		cerr << "  Default value, 0, will be used.\n";
	};
	if (!get("rewiring_ratio",param.rewiring_ratio,cerr)) {
		param.rewiring_ratio=1;
		cerr << "# No parameter given: rewiring_ratio.";
		cerr << "  Default value, 1, will be used.\n";
	};
	if (!get("if_multilink",param.if_multilink,cerr)) {
		param.if_multilink=false;
		cerr << "# No parameter given: in_multilink.";
		cerr << "  Default value, false, will be used.\n";
	};
	
	// Checking values of basic input parameters
	param.error_check();
};

// Print the information about all nodes graphically.
// This function should be used for (xsize, ysize <20).
void print_node_screen(RW2Nodes &nodes) {

	long nnodes=nodes.get_nnodes();
	if (nnodes==0) return;
	long xsize=Point2DTorus::get_xsize();
	long ysize=Point2DTorus::get_ysize();
	
	string boundary("|");
	for (long i=0;i<xsize;i++) boundary+=string("-|");

	string *snapshot=new string[ysize];
	for (long i=0;i<ysize;i++) {
		snapshot[i]="|";
		for (long j=0;j<xsize;j++) snapshot[i]+=" |";
	};

	for (NodeID i=0;i<nnodes;i++) {
		long ix=nodes.access_state(i,0,nodes.get_time()).get_pos().x;
		long iy=nodes.access_state(i,0,nodes.get_time()).get_pos().y;
		char c[2];
		c[0]=char('0')+i%10;
		c[1]='\0';
		snapshot[iy].replace(ix*2+1,1,c,1);
	};
	
	cout << boundary << endl;
	for (long iy=ysize-1;iy>=0;iy--) {
		cout << snapshot[iy] << endl;
		cout << boundary << endl;
	};
};

// Checking states of nodes from a file 
// (Returns false if some states are the same)
bool check_nodes_pos(RW2NodesBase &nodes) {
	set<Point2DTorus,Point2DTorusLessThan> pset;
	for (NodeID i=0;i<nodes.get_nnodes();i++)
		if (!pset.insert(nodes.access_state(i,0,nodes.get_time()).get_pos()).second)
			return false;
	return true;
};

// A function to check the convergence of the node state of nodes.
bool check_convergence(RW2NodesBase &nodes, TimeType t, const Graphs &graphs, short method) {
	NodeID size=nodes.get_nnodes(); // NodeID=0,..., size-1
	const int meshsize=50;
	double per;
	int per_int;
	NodeType nt=nodes.get_type_S(); // Type of nodes.
	switch (method) {
	   // Check with the node state values of all nodes.
	   case 1 :
	   	if (t==0) return false;	// At the start, it cannot compare with the previous step.
		for (NodeID i=0;i<size;i++) {
			double diff=nodes.access_state(i,nt,t).get_phi()-nodes.access_state(i,nt,t-1).get_phi();
			if (diff>epsilon || diff<-epsilon)
				return false;
		};
		return true;
		break;
	   // Find if the state reached period-1 or period-0 (used to find 2->1).
	   case 4 :
		per=abs(meshsize*find_av_diff(dynamic_cast<RW2Nodes &>(nodes),0,t,2,meshsize));
		per_int=int(per+0.5);
		if (abs(per-per_int)<0.1 && per_int<=1)
			return true;
		else
			return false;
		break;
	   // Find if the state reached period-2 ~ period-0 (used to find 3->2).
	   case 5 :
		per=abs(meshsize*find_av_diff(dynamic_cast<RW2Nodes &>(nodes),0,t,2,meshsize));
		per_int=int(per+0.5);
		if (abs(per-per_int)<0.1 && per_int<=2)
			return true;
		else
			return false;
		break;
	   // Find if the state reached period-3 ~ period-0 (used to find 4->3).
	   case 6 :
		per=abs(meshsize*find_av_diff(dynamic_cast<RW2Nodes &>(nodes),0,t,2,meshsize));
		per_int=int(per+0.5);
		if (abs(per-per_int)<0.1 && per_int<=3)
			return true;
		else
			return false;
		break;
	   // Find if the state reached period-4 ~ period-0 (used to find 5->4).
	   case 7 :
		per=abs(meshsize*find_av_diff(dynamic_cast<RW2Nodes &>(nodes),0,t,2,meshsize));
		per_int=int(per+0.5);
		if (abs(per-per_int)<0.1 && per_int<=4)
			return true;
		else
			return false;
		break;
	   // Find if the state reached period-0 (special case to find ?->0 transitions).
	   case 10 :
		per=abs(meshsize*find_av_diff(dynamic_cast<RW2Nodes &>(nodes),0,t,2,meshsize));
		per_int=int(per+0.5);
		if (abs(per-per_int)<0.1 && per_int==0)
			return true;
		else
			return false;
		break;
	   // Check with the node state values of all nodes.
	   default : // method=1
		for (NodeID i=0;i<size;i++) {
			double diff=nodes.access_state(i,nt,t).get_phi()-nodes.access_state(i,nt,t-1).get_phi();
			if (diff>epsilon || diff<-epsilon)
				return false;
		};
		return true;
	};   
	return false;
};

// A function to find the absolutedifference between two node states when they are linear(1) or circular(2).
double find_phi_diff(double a, double b, short topology) {
	double diff=a-b;
	if (topology==2) { // when circular.
		if (diff>=0.5) diff=1.0-diff;
		else if (diff<0.0)
			if (diff>-0.5) diff=-diff;
			else diff=1.0+diff;
		return diff;
	};
	// when linear.
	if (diff<0) diff=-diff;
	return diff;
};

// A function to find the difference between two node states when they are linear(1) or circular(2).
double find_phi_diff2(double a, double b, short topology) {
	double diff=a-b;
	if (topology==2) { // when circular.
		if (diff>=0.5) diff=diff-1;
		else if (diff<-0.5) diff=1.0+diff;
		return diff;
	};
	return diff;
};

// Finding groups of node states at t based on node states only 
long find_nodegroups(RW2NodesBase &nodes, NodeType nt, TimeType t, std::vector<multiset<double> > & nodegroups, \
	short topology, double thres) {
	if (!nodes.if_type(nt)) throw Bad_Node_Type();
	NodeID nnodes=nodes.get_nnodes();
	double temp=nodes.access_state(0,nt,t).get_phi();
	long ngroups=1;
	const long max_groups=1000;
	vector<double> group_values(max_groups);
	group_values[0]=temp;
	nodegroups[0].insert(temp);
	multiset<double> temp_set;
	for (NodeID i=1;i<nnodes;i++) {
		temp=nodes.access_state(i,nt,t).get_phi();
		bool ingroup=false;
		for (long j=0;j<ngroups;j++) {
			double diff=find_phi_diff(group_values[j],temp,topology);
			if (diff<thres && diff > -thres) {
				nodegroups[j].insert(temp);
				ingroup=true;
				break;
			};
		};
		if (!ingroup) {
			ngroups++;
			group_values[ngroups-1]=temp;
			temp_set.clear();
			temp_set.insert(temp);
			nodegroups.push_back(temp_set);
// cout << "create group " << ngroups << '\t' << group_values[ngroups-1] << endl;
		};
	};
				
	return ngroups;
};

// Find the average diff of node states between consecutive locations (location-wise)
//	xory=1: x, xory=2: y direction
double find_av_diff(RW2NodesBase &nodes, NodeType nt, TimeType t, short topology, long meshsize, short xory) {
	if (!nodes.if_type(nt)) throw Bad_Node_Type();
	long size,xsize,ysize;
	xsize=Point2DTorus::get_xsize();
	ysize=Point2DTorus::get_ysize();
	size=(xory==1 ? xsize : ysize);
	if (size<meshsize) meshsize=size;

	if (size%meshsize) {
		cerr << " # Bad meshsize.\n";
		exit(1);
	};

	int binsize=size/meshsize;

	vector<multiset<double> > node_state_set(meshsize);
	multiset<double> vacantset;
	for (long i=0;i<meshsize;i++)
		node_state_set[i]=vacantset;
	long nnodes=nodes.get_nnodes();
	for (NodeID id=0;id<nnodes;id++) {
		long tem_x, tem_y;
		tem_x=nodes.access_state(id,nt,t).get_pos().get_x();
		tem_y=nodes.access_state(id,nt,t).get_pos().get_y();
		double tem_p=nodes.access_state(id,nt,t).get_phi();
		if (xory==1) {
			//if (tem_y!=0) continue;
			node_state_set[tem_x/binsize].insert(tem_p);
		}
		else {
			//if (tem_x!=0) continue;
			node_state_set[tem_y/binsize].insert(tem_p);
		};
	};

	vector<double> density(meshsize);
	int not_used_meshsize=0;
	for (int i=0;i<meshsize;i++) {
		long size2=node_state_set[i].size();
		if (size2==0) {
			density[i]=-1;
			not_used_meshsize++;
		}
		else if (size2==1) {
			density[i]=*(node_state_set[i].begin());
		}
		else {
			double sum=0;
			double std=*(node_state_set[i].begin());
			double max=0, min=0;
			// This calculation is meaningful only when (node state width inside a mesh) < 0.5.
			for (multiset<double>::const_iterator j=node_state_set[i].begin();\
				j!=node_state_set[i].end();j++) {
				double diff=find_phi_diff2(*j,*(node_state_set[i].begin()),topology);
				if (diff>max) max=diff;
				if (diff<min) min=diff;
				sum+=diff;
			};
			if (max-min< 0.5) {
				double average=sum/size2+*(node_state_set[i].begin());
				if (average>1.0) average-=1.0;
				else if (average<0.0) average+=1.0;
				density[i]=average;
			}
			else {
				density[i]=-1;
				not_used_meshsize++;
			};
		};
	};
	if (not_used_meshsize>meshsize/2) return 100;
	short interval=0;
	multiset<double> diff_set;
	double prev;
	if (density[0]>-0.5) {
		interval=1;
		prev=density[0];
	};
	for (int i=1;i<meshsize;i++) {
		if (density[i]>-0.5) {
			if (interval) diff_set.insert(find_phi_diff2(density[i],prev,topology)/interval);
			prev=density[i];
			interval=1;
		}
		else 
			if (interval) interval++;
	};
	double ave=0;
	for (multiset<double>::const_iterator j=diff_set.begin();j!=diff_set.end();j++) 
		ave+=*j;
	if (diff_set.size()>0)
		return (ave/diff_set.size());
	else
		return 0;
};

// Find the maximum min-max width of p at each x or y.
double find_phi_width(RW2NodesBase &nodes, NodeType nt, TimeType t, short topology, short xory) {
	if (!nodes.if_type(nt)) throw Bad_Node_Type();
	long size,xsize,ysize;
	xsize=Point2DTorus::get_xsize();
	ysize=Point2DTorus::get_ysize();
	size=(xory==1 ? xsize : ysize);

	vector<multiset<double> > node_state_set(size);
	multiset<double> vacantset;
	for (long i=0;i<size;i++)
		node_state_set[i]=vacantset;
	NodeID nnodes=nodes.get_nnodes();
	for (int n=0;n<nnodes;n++) {
		long tem_x, tem_y;
		tem_x=nodes.access_state(n,nt,t).get_pos().get_x();
		tem_y=nodes.access_state(n,nt,t).get_pos().get_y();
		double tem_p=nodes.access_state(n,nt,t).get_phi();
		if (xory==1) {
			node_state_set[tem_x].insert(tem_p);
		}
		else {
			node_state_set[tem_y].insert(tem_p);
		};
	};

	double width;
	double maxwidth=0;
	for (int i=0;i<size;i++) {
		long size2=node_state_set[i].size();
		if (size2==0) {
			width=0;
		}
		else if (size2==1) {
			width=0;
		}
		else {
			double plus=0, minus=0;
			for (multiset<double>::const_iterator j=node_state_set[i].begin();\
				j!=node_state_set[i].end();j++) {
				double diff=find_phi_diff2(*j,*(node_state_set[i].begin()),topology);
				if (diff>0 && diff>plus) plus=diff;
				if (diff<0 && diff<minus) minus=diff;
			};
			width=plus-minus;
		};
		if (width>maxwidth) maxwidth=width;
	};

	return maxwidth;
};

// Find the average diff of x-location between consecutive node states
double find_av_diff_p(RW2NodesBase &nodes, NodeType nt, TimeType t, short topology, long meshsize, double thres) {
	if (!nodes.if_type(nt)) throw Bad_Node_Type();
	long size=meshsize;
	long xsize,ysize;
	xsize=Point2DTorus::get_xsize();
	ysize=Point2DTorus::get_ysize();
	vector<multiset<double> > xset(size);
	vector<multiset<double> > xset2(size*10);
	multiset<double> vacantset;
	for (long i=0;i<size;i++) xset[i]=vacantset;
	for (long i=0;i<size*10;i++) xset2[i]=vacantset;
	NodeID nnodes=nodes.get_nnodes();
	for (NodeID n=0;n<nnodes;n++) {
		long tem_x, tem_y;
		tem_x=nodes.access_state(n,nt,t).get_pos().get_x();
		double tem_x_double=(double(tem_x)+0.5)/xsize;
		tem_y=nodes.access_state(n,nt,t).get_pos().get_y();
		double tem_p=nodes.access_state(n,nt,t).get_phi();
		long tem_p_int=int(size*tem_p);
		long tem_p_int2=int(size*10*tem_p);
		xset[tem_p_int].insert(tem_x_double);
		xset2[tem_p_int2].insert(tem_x_double);
	};

	double width, maxwidth=0;
	for (int i=0;i<size*10;i++) {
		long size2=xset2[i].size();
		if (size2==0) {
			width=0;
		}
		else if (size2==1) {
			width=0;
		}
		else {
			double plus=0, minus=0;
			for (multiset<double>::const_iterator j=xset2[i].begin();\
				j!=xset2[i].end();j++) {
				double difference=find_phi_diff2(*j,*(xset2[i].begin()),topology);
				if (difference>plus) plus=difference;
				if (difference<minus) minus=difference;
			};
			width=plus-minus;
			if (width>maxwidth) maxwidth=width;
		};
	};
	if (maxwidth>thres) return -2;


	vector<double> density(size);
	for (int i=0;i<size;i++) {
		long size2=xset[i].size();
		if (size2==0) {
			density[i]=-1;
		}
		else if (size2==1) {
			density[i]=*(xset[i].begin());
		}
		else {
			double sum=0;
			for (multiset<double>::const_iterator j=xset[i].begin();\
				j!=xset[i].end();j++) {
				double difference=find_phi_diff2(*j,*(xset[i].begin()),topology);
				sum+=difference;
			};
			double average=sum/size2+*(xset[i].begin());
			if (average>1.0) average-=1.0;
			else if (average<0.0) average+=1.0;
			density[i]=average;
		};
	};

	short interval=0;
	multiset<double> diff_set;
	double prev;
	if (density[0]>-0.5) {
		interval=1;
		prev=density[0];
	};
	for (int i=1;i<size;i++) {
		if (density[i]>-0.5) {
			if (interval) diff_set.insert(find_phi_diff2(density[i],prev,topology)/interval);
			prev=density[i];
			interval=1;
		}
		else 
			if (interval) interval++;
	};
	double ave=0;
	for (multiset<double>::const_iterator j=diff_set.begin();j!=diff_set.end();j++) {
		ave+=*j;
	};

	if (diff_set.size()>0)
		return (ave/diff_set.size());
	else
		return 0;
};

// Initialize nodes.
bool initialize_nodes_states_file(RW2NodesBase &nodes, NodeType nt, TimeType t, std::string filename) {
	if (!nodes.if_type(nt)) throw Bad_Node_Type();
	if (!if_file(filename)) throw Bad_Input_File(filename);
	NodeID id;
	RW2State st;
	long nnodes=nodes.get_nnodes();
	long max_chars=1000;
	char tmpstr[max_chars+1];
	std::ifstream ifile(filename.c_str(),ios::in);
	do {
		// This only gives each node its RW2State.
		// The random seed will be still needed,
		// so the set_seed function should be used.
		if(ifile.peek()!='#' && ifile >> id) {
			if(!(ifile >> st)) throw Bad_Input_Value();
			if (nodes.if_ID(id)) nodes.set_state(id,nt,t,st);
		};
	} while (ifile.getline(tmpstr, max_chars, '\n'));
	return true;
};

bool initialize_nodes_states_random(RW2NodesBase &nodes, NodeType nt, TimeType t, short node_state_init_method, short geometry, PointSet &excluded) {
	if (!nodes.if_type(nt)) throw Bad_Node_Type();
	long nnodes=nodes.get_nnodes();
	if (nnodes==0) return true;

	long xsize=Point2DTorus::get_xsize();
	long ysize=Point2DTorus::get_ysize();
	
	Point2DTorus p;
	excluded.find(p);
	for (NodeID id=0; id<nnodes;id++) {
		do {
			p=Point2DTorus(nodes.access_rng(id,nt).ndraw(0,xsize-1),nodes.access_rng(id,nt).ndraw(0,ysize-1));
		} while (excluded.size() && excluded.find(p)!=excluded.end());
		RW2State st(p.get_x(), p.get_y(), \
			(node_state_init_method==2 ? double(id)/nnodes+0.5/double(nnodes) : nodes.access_rng(id,nt).fdraw()));
		nodes.set_state(id,nt,t,st);
	};
	return true;
};

// Main function.
int main(int argc, char **argv) {

	// Command line input test.
	if (argc!=2) {
		cerr << " Usage: Ex2 <input filename>\n";
		exit(1);
	};
	
	//====================================================================
	//=============(Setting Up : Reading and setting other parameters)====
	cout << "1, Reading parameters and setting up..." << endl;
	// Input filename in a string.
	string infile(argv[1]);

	// parameters.
	ParameterSet param;

	// Getting the input parameters for Ex2.
	InputParamEx2 in_param(infile,param);
	in_param.read_all_parameters();
	in_param.close();

	// Assigning the size of the grid for the Node State.
	Point2DTorus::set_size(param.xsize,param.ysize); 
	cout << "\tGrid size: " << Point2DTorus::get_xsize() << " and " << Point2DTorus::get_ysize() << endl;

	// Set the topology of the node state space.
	RW2State::set_node_state_topology(param.node_state_topology);
	cout << "\tThe topology of the node state space is ";
	if (param.node_state_topology==1) 
		cout << "linear."<< endl;
	else 
		cout << "periodic."<< endl;

	// sg effect (short-range interaction).
	bool sg_effect=false;
	if (param.neighbor_type_sg>0) sg_effect=true;
	if (sg_effect) {
		cout << "\tSG effect on node state values is on with the ";
		cout << "threshold, " << param.node_state_threshold << endl;
		cout << "\tAG " << (param.if_multilink? "can " : "cannot ") << "have multiple links";
		cout << ", and the threshold for AG rewiring is " << param.ag_threshold << endl;
		cout << "\tInteraction range for nodes: " << param.int_range << ", neighbor type: ";
		cout << param.neighbor_type_sg << endl;
	};
	cout << "\tNum of steps a node can move in a timestep: "\
		<< param.node_max_speed << ", movement type: ";
	cout << param.neighbor_type_movement << endl;
	cout << "\tUpdate_method: " << param.update_method << endl;
	// Assigning the maximum number of random numbers for RW2Node.
	RNG_Limit::set_max_ran(param.max_rn);
	cout << "\tMax num of random numbers each node can use: " << param.max_rn << endl;
	cout << "\tRandom seed for nodes, rseed1: " << param.rseed1 << endl;
	if (!param.if_agraph_in) 
		cout << "\tRandom seed for the whole system, rseed2: " \
		<< param.rseed2 << endl;
	cout << "   Done." << endl; 
	//==============================================================
	//=============(Setting Up : Initialization)====================
	cout << "2, Initializing Node states and Graphs..." << endl;
   try {
   	TimeType time=0; 

	// Nodes creation.
	RW2Nodes nodes(time);
	// Dynamical values for the simulations (two graphs, one Node state)
	// Graphs
	// 	LinkType=0: varying (bidirectional, called SG)
	// 	LinkType=1: fixed (directional or bidirectional, called AG)
	// AG (LinkType=1) can have multiple links between two nodes (if param.if_multilink=true)
	// SG (LinkType=0) will not have multiple links regardless
	bool if_loop=false;	// Both graphs will not have a self-loop.
	Graphs_Ex0 graphs1(param.nnodes,time,param.if_multilink,if_loop); // will be used when AG is directional
	Graphs_Ex1 graphs2(param.nnodes,time,param.if_multilink,if_loop); // will be used when AG is bidirectional
	Graphs *pgraphs;
	if (param.update_method<=3) pgraphs=&graphs1; 	// directional AG
	else pgraphs=&graphs2; 				// bidirectional AG
	Graphs &graphs=*pgraphs; // 'graphs' will contain two graphs (SG (0) and AG (1)).

	// Define Links object (since there is no Link State, it only counts number of links)
	RW2Links links(time,2); // # of types:2, (0: SG, 1: AG)

	// Create the Network object.
	Ex2Network net(nodes,links,graphs,param);

	// Nodes creation at time=0.
	net.create_nodes(0,time);
	cout << "\tCreated " << param.nnodes << " nodes, AG(graph) and SG(graph)." << endl;

	// Nodes initialization.
	net.initialize_nodes();
	
	// SG (LinkType=0) and AG (LinkType=1) initialization (SG depends on positions of nodes at time=0).
	net.initialize_links();

	cout << "   Done (The network has been created)." << endl;

	//===============================================================
	//=============(Simultion: Time Evolution)=======================
	
	cout << "3, Simulation starting..." << endl;
	// Will print the time information with the interval given below.
	const long print_interval=(param.total_time/100>0 ? param.total_time/100 : 1);

	// Will be used for convergence check.
	long conv_time=-1;

	// Time evolution
	while (true) {
		// Printing the time info after every print_interval.
		if(time%print_interval==0) cout << "Time: " << time << endl;
		
		// Write the necessary outputs.
		net.write_output(time);

		// Check the convergence of node states.
		// If it is found to be the same for "convergence_check_period" times,
		// we will assume it is converged.
		if(time>0 && param.convergence_method && param.convergence_check_period) {
			if(check_convergence(nodes,time,graphs,param.convergence_method))
				conv_time++;
			else
				conv_time=0;
		};

		// Stop the simulation if total_time has been reached, or the state has been converged for convergence_check_period.
		if (time>=param.total_time || (param.convergence_check_period && conv_time==param.convergence_check_period)) break;

		time++; // time advances.

		// Updating nodes, AG and SG. (the order is important!).
		net.update_nodes(time);
		net.update_links(time);

	};

	long final_time=time;
	bool if_converged=false;
	if (conv_time==param.convergence_check_period) {
		if_converged=true;
		cout << "   Converged. ";
	}
	else cout << "   Done. ";

	cout << "The time has evolved from 0 to " << final_time << " (timestep)." << endl;

	// Writing the summary
	if(param.if_summary_out) net.write_summary(final_time,if_converged);

	cout << "4, End of program.\n";

   }
   catch (MAX_RN_REACHED m) {
	cerr << "# Error: maximum number of random numbers for each  node reached.\n" ;
	return 1;
   }
   catch (TOO_MANY_RN_NEEDED m) {
	cerr << "# Error: maximum number of random numbers for a RNG reached.\n" ;
	cerr << "         The number should be less than 20% of the period of RNG.\n" ;
	return 1;
   }
   catch (Bad_Input_File m) {
	cerr << "# Error: Bad input file, " << m.filename << ".\n" ;
	return 1;
   }
   catch (Bad_Input_Value m) {
	cerr << "# Error: Bad input value.\n";
	return 1;
   }
   catch (Bad_Edge m) {
	cerr << "# Error: Bad edge.\n";
	return 1;
   }
   catch (Bad_Input_ID m) {
	cerr << "# Error: Bad_Input_ID Type.\n";
	return 1;
   }
   catch (Bad_Input_Time m) {
	cerr << "# Error: Bad Input Time.\n";
	return 1;
   }
   catch (Bad_Input_Type m) {
	cerr << "# Error: Bad Input Type.\n";
	return 1;
   }
   catch (TimeSeq_Range_Error m) {
	cerr << "# Error: TimeSeq Range Error.\n";
	return 1;
   }
   catch (TimeSeq_Size_Set m) {
	cerr << "# Error: TimeSeq Size Set.\n";
	return 1;
   }
   catch (TimeSeq_Size_Not_Set m) {
	cerr << "# Error: TimeSeq Size Not Set.\n";
	return 1;
   }
   catch (Bad_Node_ID m) {
	cerr << "# Error: Bad Node ID.\n";
	return 1;
   }
   catch (Bad_Link_ID m) {
	cerr << "# Error: Bad Link ID.\n";
	return 1;
   }
   catch (Bad_Node_Type m) {
	cerr << "# Error: Bad Node Type.\n";
	return 1;
   }
   catch (Bad_Link_Type m) {
	cerr << "# Error: Bad Link Type.\n";
	return 1;
   }
   catch (Wrong_Node_Type m) {
	cerr << "# Error: Wrong Node Type.\n";
	return 1;
   }
   catch (Wrong_Link_Type m) {
	cerr << "# Error: Wrong Link Type.\n";
	return 1;
   }
   catch (NodeID_None_Available m) {
	cerr << "# Error: NodeID None Available.\n";
	return 1;
   }
   catch (LinkID_None_Available m) {
	cerr << "# Error: LinkID None Available.\n";
	return 1;
   }
   catch (Bad_Torus_Size m) {
	cerr << "# Error: Bad Torus Size.\n";
	return 1;
   }
   catch (NeighborTypeError m) {
	cerr << "# Error: Neighborhood Type.\n";
	return 1;
   };
exit(0);
	return 0;
};

