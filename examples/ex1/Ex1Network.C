// 
//	CONETSIM - Ex1Network.C
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

#include <iostream>
#include <fstream>
#include <cstdlib>
#include "Ex1Network.h"
#include "Ex1.h"
#include "Utilities.h"
#include "Types.h"

#ifndef MAX_CHARS
#define MAX_CHARS 1000
#endif

// Initializing the opinion topology (static variable).
short RW2State::opinion_topology=1;

// Network for Ex1
// constructor
Ex1Network::Ex1Network(RW2Nodes &nn, RW2Links &ll, Graphs &gg, ParameterSet &p): conet::Network2<RW2State>(nn,ll,gg,p), 
	nodes_out(param.if_snapshot_out ? param.out_snapshot_file : Output::noout), 
	nodes_out_p(param.if_snapshot_out && param.convergence_method>3 ? param.out_snapshot_file+'p' : Output::noout),  //Additional information on period
	ag_out(param.if_ag_snapshot_out ? param.out_ag_snapshot_file : Output::noout), 
	sg_out(param.if_sg_snapshot_out ? param.out_sg_snapshot_file : Output::noout),
	one_node_out(param.if_node_out ? param.out_node_file : Output::noout),
	summary_out(param.if_summary_out ? param.out_summary_file : Output::noout), ops(param.nnodes) {
	// finding excluded points (for NodeState)
	if (param.geometry==2) {
		std::ifstream ifile;
		ifile.open(param.in_excluded_file.c_str(),ios::in);
		if (!ifile) {
			cerr << "# Error: Input file, " << param.in_excluded_file;
			cerr << ", doesn't exist.\n";
			exit(1);
		};
		long x, y;
		while (ifile >> x >> y && x>=0 && x<param.xsize && y>=0 && y<param.ysize) excluded.insert(Point2DTorus(x,y));
		ifile.close();
		// cout << "\t(Total of " << excluded.size() << " points will be excluded.)" << endl;
	};
	// For SG.
	conet::find_rel_neighbors(param.int_range,param.neighbor_type_sg,neighbor_states);
	long state_size=Point2DTorus::get_xsize()*Point2DTorus::get_ysize();
	occupancy.resize(state_size);

	// For node update
	// Find the relative points of a node can move in a time step.
	conet::find_rel_neighbors(param.node_max_speed,param.neighbor_type_movement,rel_trans);
};

// destructor
Ex1Network::~Ex1Network() {
	// Closing all output files if necessary.
	if (param.if_snapshot_out) {
		nodes_out.close();
		nodes_out_p.close();
	};
	if (param.if_ag_snapshot_out) ag_out.close();
	if (param.if_sg_snapshot_out) sg_out.close();
	if (param.if_node_out) one_node_out.close();
	summary_out.close();
};
	
// Initialize nodes.
void Ex1Network::initialize_nodes() {
	nodes.initialize_random_seeds(0,param.rseed1,param.max_rn);
	cout << "\tRandom seeds for all nodes are initialized." << endl;
	if (param.if_nodes_in) {
		if (!initialize_nodes_states_file(nodes,0,0,param.in_nodes_file)) { // reading from a file.
			cerr << "# Check the input file for nodes, ";
			cerr << param.in_nodes_file << '\n';
			exit(1);
		};
			
		cout << "\tInitialized " << param.nnodes;
		cout << " nodes from the file, " << param.in_nodes_file << endl;
	}
	else {
		if (initialize_nodes_states_random(nodes,0,0,param.op_init_method,param.geometry,excluded)) {
			cout << "\tInitialized " << param.nnodes << " nodes ";
			cout << "randomly with ";
			if (param.op_init_method==1) cout << "random opinions." << endl;
			else cout << "regular opinions." << endl;
		};
	};
};

// Initialize links and graphs.
void Ex1Network::initialize_links() {
	if (graphs.get_time()!=0) return;
	// AG initialization.
	if (param.if_agraph_in) {
		Input in2(param.in_agraph_file);
		in2.read_graph(graphs.access_graph(1,0));
		for (long i=0;i<graphs.access_graph(1,0).get_link_size();i++) links.add_link_noID(1,0);
		cout << "\tInitialized AG(graph) from the file, " << param.in_agraph_file << ", with " << graphs.get_nlinks_type(1) << " links." << endl;
	}
	else
		switch (param.in_agraph_method) {
		   case 0:
			if (param.num_edges>0) {
				initialize_random_graph1(1,graphs,links,param.num_edges,param.rseed2);
				cout << "\tInitialized AG(" << (param.update_method<3? "directional" : "bidirectional") << " graph) randomly with ";
				cout << param.num_edges << " edges." << endl;
			}
			else if (param.num_edges==0) { // Do nothing
				cout << "\tAG(" << (param.update_method<3? "directional" : "bidirectional") << " graph) doesn't exist because there is no";
				cout << " edge." << endl;
			}
			else {
				initialize_random_graph2(1,graphs,links,param.wiring_prob,param.rseed2);
				cout << "\tInitialized AG(" << (param.update_method<3? "directional" : "bidirectional") << " graph) randomly with ";
				cout << "probability " << param.wiring_prob << '.' << endl;
			};
			break;
		   case 1:
			initialize_random_graph2(1,graphs,links,param.wiring_prob,param.rseed2);
			cout << "\tInitialized AG(" << (param.update_method<3? "directional" : "bidirectional") << " graph) (having one edge per vertex)";
			cout << " randomly." << endl;
			break;
		   default:
		   	break;
		};
	// SG initialization
	create_links(0,0);
};

// Create nodes.
void Ex1Network::create_nodes(NodeType nt, TimeType t) {
	if (nt==0 && t==0) {
		for (NodeID id=0;id<param.nnodes;id++) {// ID: 0 ~ nnodes-1
			create_node(id,0,0);
		};
		// cout << "\tCreated node objects for " << param.nnodes << " nodes."<< endl;
	};
};

// Update nodes (rule-based).
void Ex1Network::update_node_states(NodeType nt, TimeType t) {
	if (nt!=0) return;
	// Assumes that nodes are not created nor removed during the simulation.
	for (NodeID id=0;id<param.nnodes;id++) {
		// find the next position, find the new opinion, and assign the new state.
		update_node_state(id,nt,t,next_state(id,t-1)); // advance function not needed, because
			// it will advance to the next time step automatically when t=get_last_time()+1
	};	
};

// Remove links (rule-based).
void Ex1Network::remove_links(LinkType lt, TimeType t) {
	// For LinkType=0, all links has to be removed every timestep (starting from no links at every step).
	graphs.advance_L(0,t,false); //false: advance with null, true: advance with copying the previous value.
	if (lt==0) links.clear(lt);
};

// Create links (rule-based).
void Ex1Network::create_links(LinkType lt, TimeType t) { // For LinkType=0 only.
	if (lt!=0) return;
	update_occupancy(t); // occupancy should be updated based on new avec.
	long nnodes=nodes.get_nnodes();
	for (NodeID id=0;id<nnodes;id++) {
		// Find the neighboring nodes (within int_range)
		find_neighbors(access_node_state(id,0,t).get_pos());
		// Add edges for all neighbors found.
		for (NodeIDSSet::const_iterator i=neighbors.begin(); \
			i!=neighbors.end();i++) {
			create_link_noID(0,*i,id,t);
		};
	};
};

// Output nodes (to a set of output files).
void Ex1Network::write_nodes_T(Output &output, TimeType t, short method) {
	if (method==0) {
		for (NodeID id=0;id<nodes.get_nnodes();id++) {
			output.ofile << t << '\t' << id << '\t';
			nodes.write_ID_T(output.ofile,id,t);
			output.write('\n');
		};
	};
};

// Write outputs of this Ex1 problem.
void Ex1Network::write_output(TimeType t) {
	// Writing outputs to files if necessary (writing if t=0).
	if(param.if_snapshot_out && ((t==0 || (param.out_snapshot_interval \
		&& t%param.out_snapshot_interval==0)) || (!param.out_snapshot_interval && t==param.total_time))) {
			write_nodes_T(nodes_out,t,0);
	};
	if(param.if_ag_snapshot_out && ((t==0 || (param.out_ag_snapshot_interval \
		&& t%param.out_ag_snapshot_interval==0)) || (!param.out_ag_snapshot_interval && t==param.total_time))) {
		switch (param.out_ag_snapshot_method) {
			case 1:
				ag_out.write_graph1(graphs.access_graph(1,t),t);
				break;
			case 2:
				ag_out.write_graph2(graphs.access_graph(1,t),t);
				break;
			case 3:
				ag_out.write("T\t");
				ag_out.write(t); // writing the current time
				ag_out.write('\n');
				ag_out.write_graph3(graphs.access_graph(1,t));
				break;
			default:
				ag_out.write_graph1(graphs.access_graph(1,t),t);
				break;
		};
	};
	if(param.if_sg_snapshot_out && ((t==0 || (param.out_sg_snapshot_interval \
		&& t%param.out_sg_snapshot_interval==0)) || (!param.out_sg_snapshot_interval && t==param.total_time))) {
		switch (param.out_sg_snapshot_method) {
			case 1:
				sg_out.write_graph1(graphs.access_graph(0,t),t);
				break;
			case 2:
				sg_out.write_graph2(graphs.access_graph(0,t),t);
				break;
			case 3:
				sg_out.write("T\t");
				sg_out.write(t); // writing the current time
				sg_out.write('\n');
				sg_out.write_graph3(graphs.access_graph(0,t));
				break;
			default:
				sg_out.write_graph1(graphs.access_graph(0,t),t);
				break;
		};
	};
	if(param.if_node_out && ((t==0 || (param.out_node_interval \
		&& t%param.out_node_interval==0)) || (!param.out_node_interval && t==param.total_time))) {
		one_node_out.ofile << t << '\t' << param.node_id << '\t';
		nodes.write_ID_T(one_node_out.ofile,param.node_id,t);
		one_node_out.write('\n');
	};

	// Writing the period info to a separate file.
	if(param.convergence_method>3 && param.if_snapshot_out && param.convergence_out_period 
		&& t%param.convergence_out_period==0) {
		int meshsize=50;
		double per=abs(meshsize*find_av_diff(nodes,0,t,param.opinion_topology,meshsize));
		nodes_out_p.write(t);
		nodes_out_p.write('\t');
		nodes_out_p.write(per);
		nodes_out_p.write('\t');
		nodes_out_p.write(int(per+0.5));
		nodes_out_p.write('\n');
	};

	// Writing the network stat
	if (param.if_network_stat_out && param.out_network_stat_time>-1 && param.out_network_stat_time==t) write_network_stat(t);
};

// Write summary of this Ex1 problem.
void Ex1Network::write_summary(TimeType t, bool if_converged) {
	long ngroups=1;
	double ave_diff=0;
	vector<multiset<double> > opiniongroups(ngroups);
	if (if_converged) {
		ngroups=find_opiniongroups(nodes,0,t,opiniongroups,param.opinion_topology,param.op_threshold);
	};
	summary_out.write("nnodes\t");
	summary_out.write(nodes.get_nnodes());
	summary_out.write('\n');
	summary_out.write("edges\t");
	summary_out.write(graphs.get_nlinks_type(1));
	summary_out.write('\n');
	summary_out.write("time\t");
	summary_out.write(t-(if_converged ? param.convergence_check_period : 0));
	summary_out.write('\n');
	if (param.convergence_method<4 && param.convergence_method>0) {
		summary_out.write("converged\t");
		summary_out.write(if_converged ? ngroups : 0);
		summary_out.write('\n');
		if (if_converged || param.opinion_topology==1) {
			for (int i=0;i<ngroups;i++) {
				summary_out.write("opinions\t");
				double sum=0;
				for (set<double>::const_iterator j=opiniongroups[i].begin();\
					j!=opiniongroups[i].end();j++) {
					sum+=*j;
				};
				summary_out.write(sum/opiniongroups[i].size());
				summary_out.write('\t');
				summary_out.write(static_cast<long>(opiniongroups[i].size()));
				summary_out.write('\n');
			};
		};
	}
	else if (param.convergence_method>3) {
		summary_out.write("converged\t0\n");
		summary_out.write("simulations to observe specific transitions\n");
	}
	else if (param.convergence_method==0) {
		ngroups=find_opiniongroups(nodes,0,t,opiniongroups, param.opinion_topology,param.op_threshold);
		summary_out.write("ngroups\t");
		summary_out.write(ngroups);
		summary_out.write('\n');
	
	};
	if (!if_converged || param.convergence_method>3)  {
		summary_out.write("av_diff_x\t");
		double width_x=find_op_width(nodes,0,t,param.opinion_topology,1);
		double width_y=find_op_width(nodes,0,t,param.opinion_topology,2);
		long meshsize=50;
		// diff_x and diff_y will be 100, only when opinion widths of more than
		//	half meshes are greater than 0.5 (making the results not useful)
		double diff_x=find_av_diff(nodes,0,t,param.opinion_topology,meshsize);
		double diff_y=find_av_diff(nodes,0,t,param.opinion_topology,meshsize,2);
		if (diff_x>99 || param.xsize==1) {// not converged x-direction
			summary_out.write("0\t-1\n");
		}
		else {
			summary_out.write(diff_x);
			summary_out.write('\t');
			summary_out.write(int(meshsize*(diff_x>0?diff_x:-diff_x)+0.5));
			summary_out.write('\n');
		};
		summary_out.write("av_diff_y\t");
		if (diff_y>99 || param.ysize==1) {// not converged y-direction
			summary_out.write("0\t-1\n");
		}
		else {
			summary_out.write(diff_y);
			summary_out.write('\t');
			summary_out.write(int(meshsize*(diff_y>0?diff_y:-diff_y)+0.5));
			summary_out.write('\n');
		};
		summary_out.write("av_diff_p\t");
		if (width_x>param.op_threshold && width_y>param.op_threshold) { // periodic fractionally
			double diff_p=find_av_diff_p(nodes,0,t,param.opinion_topology,meshsize,param.op_threshold);
			if (diff_p<-1) {
				summary_out.write("0\t-1\n");
			}
			else {
				summary_out.write(diff_p);
				summary_out.write('\t');
				summary_out.write(int((meshsize)*(diff_p>0?diff_p:-diff_p)+0.5));
				summary_out.write('\n');
			};
		}
		else {
			summary_out.write("0\t-1\n");
		};
	};
	if (param.if_network_stat_out && (param.out_network_stat_time==-1 || t<param.out_network_stat_time)) write_network_stat(t);
};

// Write the network stat if necessary.
void Ex1Network::write_network_stat(TimeType t) {
	vector<NodeIDSSet> components;
	GraphFB cg(param.nnodes); // Combined graph of AG and SG (bidirectional)
	conet::combine_graphs(cg,dynamic_cast<GraphFB &>(graphs.access_graph(0,t)),dynamic_cast<GraphFB &>(graphs.access_graph(1,t)));
	long ncomponents=cg.find_components(components);
	summary_out.write("network_stat_time\t");
	summary_out.write(t);
	summary_out.write('\n');
	summary_out.write("ncomponents\t");
	summary_out.write(ncomponents);
	summary_out.write('\n');
	summary_out.write("clustering_coeff\t");
	summary_out.write(cg.find_clustering_coeff());
	summary_out.write('\n');
	summary_out.write("mean_distance\t");
	summary_out.write(cg.find_mean_distance());
	summary_out.write('\n');
	if (param.if_components_out) {
		Output components_out(param.if_components_out ? param.out_components_file : Output::noout);
		for (long i=0;i<ncomponents;i++) {
			long componentsize=components[i].size();
			components_out.write(i);
			components_out.write('\t');
			components_out.write(componentsize);
			components_out.write('\n');
			double sum=0.0;
			for (NodeIDSSet::const_iterator j=components[i].begin();\
				j!=components[i].end();j++) {
				sum+=nodes.access_state(*j,0,t).get_op();
				components_out.write(*j);
				components_out.write('\t');
				components_out.write(nodes.access_state(*j,0,t).get_op());
				components_out.write('\n');
			};
			//sum/=componentsize;
			//network_stat_out.write(sum);
			//network_stat_out.write('\n');
		};
		components_out.close();
	};
};

// Update the occupancy.
void Ex1Network::update_occupancy(TimeType t) {
	// Delete the occupancy information first.
	long size=occupancy.size();
	for (long i=0;i<size;i++)
		occupancy[i].clear();
	// Then, re-find the occupancy information from states of nodes.
	long nnodes=nodes.get_nnodes();
	for (NodeID id=0;id<nnodes;id++)
		occupancy[find_index(nodes.access_state(id,0,t).get_pos())].insert(id);
};

// Find the neighboring nodes based on a given state.
void Ex1Network::find_neighbors(const Point2DTorus &p) {
	neighbors.clear();
	long index;
	for (PointSet::const_iterator i=neighbor_states.begin(); \
		i!=neighbor_states.end();i++) {
		Point2DTorus p1=p+(*i);
		if (p1.if_inside()) {
			index=find_index(p+(*i));
			for (NodeIDSSet::const_iterator j=occupancy[index].begin();\
				j!=occupancy[index].end();j++) 
				neighbors.insert(*j);
		};
	};
};

// Print the occupancy information.
void Ex1Network::print_occupancy() const {
	long size=occupancy.size();
	long xsize=Point2DTorus::get_xsize();
	for (long i=0;i<size;i++) {
		cout << '(' << i%xsize << ',' << i/xsize << "): ";
		print_set(occupancy[i]);
	};
}; 

// Find the next state of a node at t+1 (for node update)
RW2State Ex1Network::next_state(NodeID id, TimeType t) {
	// Old values
	long old_x=access_node_state(id,0,t).get_pos().x;
	long old_y=access_node_state(id,0,t).get_pos().y;
	double old_op=access_node_state(id,0,t).get_op();

	// New values to be found here.
	long new_x,new_y;
	double new_op;

	// (1) Find the opinion of a node at t+1.
	NodeIDMSet deps_ag_in, deps_ag_out, deps_sg;
	long dep_out_size=access_graph(1,t).find_out_deg(id);

	// Gathering the opinions of AG in-neighbors (LinkType=1).
	long count=0;
	if (access_graph(1,t).find_in_dep(id,deps_ag_in)>0) {
		for (NodeIDMSet::const_iterator j=deps_ag_in.begin();j!=deps_ag_in.end();j++) {
			double opinion=access_node_state(*j,0,t).get_op();
			if (find_op_diff(opinion,old_op,param.opinion_topology) < param.op_threshold)
				ops[count++]=opinion;
		};
	};
	// Gathering the opinions of SG neighbors (LinkType=0). (when overlapped, do not add)
	if (access_graph(0,t).find_dep(id,deps_sg)>0) {
		for (NodeIDMSet::const_iterator j=deps_sg.begin();j!=deps_sg.end();j++) {
			double opinion=access_node_state(*j,0,t).get_op();
			if ((find_op_diff(opinion,old_op,param.opinion_topology) < param.op_threshold) \
				&& deps_ag_in.find(*j)==deps_ag_in.end())
				ops[count++]=opinion;
		};
	};

	// Get the new opinion.
	if (count>0) {
		new_op=old_op+param.coupling_constant*find_op_movement(old_op,count);
		while (new_op>1.0) new_op--;
		while (new_op<0.0) new_op++;
	};

	// (2) Find the position of a node at t+1.
	// Find possible transition points for a node. 
	transitions.clear();
	for (PointSet::const_iterator i=rel_trans.begin();\
		i!=rel_trans.end();i++) {
		Point2DTorus p=access_node_state(id,0,t).get_pos()+*i;
		if (p.if_inside())
			if (excluded.size()==0 || excluded.find(p)==excluded.end())
				transitions.insert(p);
	};
	long size=transitions.size(); // Number of possible transtion points.
	switch (size) { 
		case 0:
			new_x=old_x;
			new_y=old_y;
			break;
		case 1:
			new_x=(*(transitions.begin())).x;
			new_y=(*(transitions.begin())).y;
			break;
		default:
			PointSet::const_iterator j=transitions.begin();
			// Assuming the equal probability for all possible transitions.
			int draw=access_node_rng(id,0).ndraw(0,size-1);
			for (int i=0;i<draw;i++) j++;
			new_x=j->x;
			new_y=j->y;
	};
	
	return RW2State(new_x,new_y,new_op);
};

// A function to find the movement of an opinion by other opinions. (for node update)
double Ex1Network::find_op_movement(double myop, long size) {
	double temp1, temp2=0.0;
	for (long i=0;i<size;i++) {
		temp1=ops[i]-myop;
		if (param.opinion_topology==2) {
			if (temp1>0.5) temp2+=temp1-1.0;
			else if (temp1>-0.5) temp2+=temp1;
			else
				temp2+=temp1+1.0;
		}
		else {
			temp2+=temp1;
		};
	};
	// update_method=even: divided by size+1, odd: divided by size.
	return temp2/(param.update_method%2 ? size : size+1);
};
