// 
//	CONETSIM - Nodes.C
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


#include "Nodes.h"
#include "Random4.h"
#include "NodeInput.h"
#include "Types.h"
#include <istream>
#include <ostream>
#include <iostream>
#include <map>
#include <set>
#include <vector>

// Set the random seeds if the given NodeType is the derived from NodeR. 
// (rseed: random seed, max_rn: the maximun number of random nunmbers a node can have).
void conet::Nodes::initialize_random_seeds(NodeType nt, unsigned long rseed, long max_rn) {
	if (!if_type(nt)) throw Bad_Node_Type();
	long nnodes=nodes_type_idset[nt].size(); // Number of nodes for the given type.
	double total_rn=(double) nnodes*max_rn; // total number of random numbers.
	Random4 rn1; // See "Types.h" for the RNG.
	rn1.seed(rseed);
	
	double cnt_double=0; // counting random numbers generated so far (double because it can be more than 2*10^9)..
	long cnt_node=0; // counting nodes for standard output showing progress.
	long cnt2=0; // counter for showing progress.
	double max_ran_nums=rn1.period()/5;
	if (total_rn>1.0e+7) 
		std::cout << "\tInitializing the random seeds for all " << "nodes of type, " << nt << "." << std::endl << "\t   ";
	for (NodeIDSSet::iterator i=nodes_type_idset[nt].begin();i!=nodes_type_idset[nt].end();i++) {
		this->set_seed(*i,nt,rn1.show_seed());
		double ratio=cnt_double/total_rn; // ratio of random numbers generated so far.
		for (long j=0;j<max_rn;j++) {
			rn1.draw();
			cnt_double+=1.0;
			if (max_ran_nums < cnt_double) throw TOO_MANY_RN_NEEDED();
		};
		cnt_node++;
		if (total_rn>1.0e+7 && cnt_node!=0 && cnt_node!=nnodes && cnt_node%(nnodes/10)==0) {
			cnt2++;
			std::cout << 10*cnt2 << "%..." << std::flush; 
		};
	};
	if (total_rn>1.0e+7) std::cout << "100%" << std::endl; 
};
