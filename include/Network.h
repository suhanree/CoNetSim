// 
//	CONETSIM - Network.h
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


#ifndef NETWORK_H
#define NETWORK_H

#include "Graph.h"
#include "Graphs.h"
#include "Link.h"
#include "Links.h"
#include "Node.h"
#include "Nodes.h"
#include "NodeInput.h"
#include "NodeInputs.h"
#include "Input.h"
#include "Output.h"
#include "Types.h"
#include "Errors.h"

#include <set>
#include <map>
#include <vector>
#include <iostream>

class ParameterSet; // Forward declarartion.

namespace conet {

// A class that represents a whole network.
// Networks consist of nodes (with or without state), links (with or without state), and their connections.
// Nodes are represented by ID's, and their states are represented by Node class (if there is Node states).
// Links can be represented by ID's and Link class if there is Link states and link delays can be greater than 1.
// 	(If links have delay 1 and no state, links are only represented by their connecting nodes without ID's.)
// A network can contain many types of nodes and links, and link connections can be represented by Graph classes.

// Base class that represents Nodes only (no Node state).
class NetworkBaseNodes {
   public :
	// Constructors.
	NetworkBaseNodes(Nodes &nn): nodes(nn) {};

	// Destructor.
	~NetworkBaseNodes() {};

	// Initialize nodes (from a file or randomly).
	void initialize_nodes() {};

	// Output nodes (to a set of output files).
	void write_nodes_T(Output &outputs, TimeType t, short method=0) {};

	// Create nodes (rule-based, has to be overwritten in subclasses)
	void create_nodes(NodeType nt, TimeType t) {};

	// Remove nodes (rule-based, has to be overwritten in subclasses)
	void remove_nodes(NodeType nt, TimeType t) {};

	// Update node states (rule-based, has to be overwritten in subclasses)
	void update_node_states(NodeType nt, TimeType t) {};

	// Update all nodes using create_nodes, remove_nodes, update_node_states.
	// (rule-based, has to be overwritten in subclasses)
	void update_nodes(TimeType t) {};

	// Create a node (if it's associated with a state, a default value will be given).
	bool create_node_generatedID(NodeID &id, NodeType nt, TimeType t) { // ID will be chosen automaically.
		return nodes.add_node_generatedID(id,nt,t);
	};

	bool create_node(NodeID id, NodeType nt, TimeType t) {
		return nodes.add_node(id,nt,t);
	};

	// Remove a node.
	bool remove_node(NodeID id, NodeType nt, TimeType t) {
		return nodes.remove_node(id,nt,t);
	};
	
	// Access RNG of a node
	RNG_Limit & access_node_rng(NodeID id, NodeType nt) {
		return nodes.access_rng(id,nt);
	};

   public:
	Nodes &nodes;		// an object that contains nodes without state (it can be NodesNF or NodesNV, too).
};

// Base class that represents Nodes with one state (fixed number).
template<class NodeState> class NetworkBaseNodesNF1S: public NetworkBaseNodes {
   public:
	// constructor.
	NetworkBaseNodesNF1S(NodesNF1S<NodeState> &nn): NetworkBaseNodes(nn), nodes(nn) {};

	// Destructor.
	~NetworkBaseNodesNF1S() {};

	// Advance time for Nodes object.
	void advance_nodes(TimeType t) {
		nodes.advance(t);
	};
	void advance_nodes_N(NodeType nt, TimeType t) {
		nodes.advance_N(nt,t);
	};

	// Create a node (if it's associated with a state, a default value will be given).
	bool create_node_generatedID(NodeID &id, NodeType nt, TimeType t) { // ID will be chosen automaically.
		return nodes.add_node_generatedID(id,nt,t);
	};

	bool create_node(NodeID id, NodeType nt, TimeType t) {
		return nodes.add_node(id,nt,t);
	};

	// Remove a node.
	bool remove_node(NodeID id, NodeType nt, TimeType t) {
		return nodes.remove_node(id,nt,t);
	};

	// Access a node state.
	NodeState & access_node_state(NodeID id, NodeType nt, TimeType t) {
		return nodes.access_state(id,nt,t);
	};
	
	// Update a node state.
	bool update_node_state(NodeID id, NodeType nt, TimeType t, const NodeState &s) {
		return nodes.set_state(id,nt,t,s);
	};

	// Access RNG of a node
	RNG_Limit & access_node_rng(NodeID id, NodeType nt) {
		return nodes.access_rng(id,nt);
	};

   public:
   	NodesNF1S<NodeState> &nodes; // The same name with "NetworkBaseNodes::nodes" intentionally.
};

// Base class that represents Nodes with one state (varying number).
template<class NodeState> class NetworkBaseNodesNV1S: public NetworkBaseNodes {
   public:
   	// Constructor.
	NetworkBaseNodesNV1S(NodesNV1S<NodeState> &nn): NetworkBaseNodes(nn), nodes(nn) {};

	// Destructor.
	~NetworkBaseNodesNV1S() {};

	// Advance time for Nodes object.
	void advance_nodes(TimeType t) {
		nodes.advance(t);
	};
	void advance_nodes_N(NodeType nt, TimeType t) {
		nodes.advance_N(nt,t);
	};

	// Create a node (if it's associated with a state, a default value will be given).
	bool create_node_generatedID(NodeID &id, NodeType nt, TimeType t) { // ID will be chosen automaically.
		return nodes.add_node_generatedID(id,nt,t);
	};

	bool create_node(NodeID id, NodeType nt, TimeType t) {
		return nodes.add_node(id,nt,t);
	};

	// Remove a node.
	bool remove_node(NodeID id, NodeType nt, TimeType t) {
		return nodes.remove_node(id,nt,t);
	};

	// Access a node state.
	NodeState & access_node_state(NodeID id, NodeType nt, TimeType t) {
		return nodes.access_state(id,nt,t);
	};
	
	// Update a node state.
	bool update_node_state(NodeID id, NodeType nt, TimeType t, const NodeState &s) {
		return nodes.set_state(id,nt,t,s);
	};

	// Access RNG of a node
	RNG_Limit & access_node_rng(NodeID id, NodeType nt, TimeType t) {
		return nodes.access_rng(id,nt);
	};

   public:
   	NodesNV1S<NodeState> &nodes; // The same name with "NetworkBaseNodes::nodes" intentionally.
};

// Base class that represents Links and Graphs wihtout state (it also contains parameters for the system).
class NetworkBaseLinks {
   public:
   	// Constructor.
	NetworkBaseLinks(Links &ll, Graphs & gg, ParameterSet &p): links(ll), graphs(gg), param(p) {};

	// Destructor.
	~NetworkBaseLinks() {};

	// Initialize links and graphs (from a file or randomly).
	void initialize_links() {};

	// Output links (to an array of output files).
	void write_links(Output &outputs, TimeType t, short method=0) {};

	// Advance time for Links object.
	void advance_links(TimeType t) {
		links.advance(t);
		graphs.advance(t);
	};
	void advance_links_L(LinkType lt, TimeType t) {
		links.advance_L(lt,t);
		graphs.advance_L(lt,t);
	};

	// Create links (rule-based, has to be overwritten in subclasses).
	void create_links(LinkType lt, TimeType t) {};

	// Remove links (rule-based, has to be overwritten in subclasses).
	void remove_links(LinkType lt, TimeType t) {};

	// Rewire links (rule-based, has to be overwritten in subclasses).
	void rewire_links(LinkType lt, TimeType t) {};

	// Update link states (rule-based, has to be overwritten in subclasses).
	void update_link_states(LinkType lt, TimeType t) {};

	// Update all links using create_links, remove_links, rewire_links, update_link_states 
	// (rule-based, has to be overwritten in subclasses).
	void update_links(TimeType t) {};

	// Access a graph.
	Graph & access_graph(LinkType lt, TimeType t) {
		return graphs.access_graph(lt,t);
	};

	// Create a link with an ID (with the delay) (there is no link state in this class). (can be only used with links with ID).
	bool create_link_generatedID(LinkID &id, LinkType lt, NodeID ori, NodeID des, DelayType d, TimeType t) { // ID will be chosen automatically.
		if (links.add_link_generatedID(id,lt,t,d)) {
			if (graphs.add_link(id,lt,t,ori,des)) return true;
			else {
				links.remove_link(id,lt,t);
				return false;
			};
		}
		else return false;
	};
	// can be used with links with ID or links without ID
	bool create_link(LinkID id, LinkType lt, NodeID ori, NodeID des, DelayType d, TimeType t) {
		if(!links.if_type(lt) || !graphs.access_graph(lt,t).if_link_ID()) throw Bad_Link_Type();
		if (graphs.add_link(id,lt,t,ori,des)) {
			if (links.add_link(id,lt,t,d)) return true;
			else {
				graphs.remove_link(id,lt,t,ori,des); // If it fails to add link to Links, it has to remove that link from Graphs.
				return false;
			};
		}
		else return false;
	};
	bool create_link_noID(LinkType lt, NodeID ori, NodeID des, TimeType t) {
		if(!links.if_type(lt) || graphs.access_graph(lt,t).if_link_ID()) throw Bad_Link_Type();
		if (graphs.add_link_noID(lt,t,ori,des)) {
			if (links.add_link_noID(lt,t)) return true; // id is dummy here
			else {
				graphs.remove_link_noID(lt,t,ori,des); // If it fails to add link to Links, it has to remove that link from Graphs.
					return false;
			};
		}
		else return false;
	};

	// Remove a link (with ID or without ID).
	bool remove_link(LinkID id, LinkType lt, NodeID ori, NodeID des, TimeType t) {
		if(!links.if_type(lt) || !graphs.access_graph(lt,t).if_link_ID()) throw Bad_Link_Type();
		if (graphs.remove_link(id,lt,t,ori,des)) {
			if (links.remove_link(id,lt,t)) return true;
			else {
				graphs.add_link(id,lt,t,ori,des);
				return false;
			};
		}
		else return false;
	};
	bool remove_link_noID(LinkType lt, NodeID ori, NodeID des, TimeType t) {
		if(!links.if_type(lt) || graphs.access_graph(lt,t).if_link_ID()) throw Bad_Link_Type();
		if (graphs.remove_link_noID(lt,t,ori,des)) {
			links.remove_link_noID(lt,t);	// No ID needed, this only tells Links that one link will be deleted.
			return true;
		}
		else return false;
	};

	// Rewire a link with ID.
	bool rewire_link(LinkID id, LinkType lt, NodeID ori, NodeID des, NodeID newori, NodeID newdes, TimeType t) {
		if (graphs.remove_link(id,lt,t,ori,des)) {
			if (graphs.add_link(id,lt,t,newori,newdes)) return true;
			else {
				graphs.add_link(id,lt,t,ori,des);
				return false;
			};
		}
		else return false;
	};

	// Rewire an ID-less link.
	bool rewire_link_noID(LinkType lt, NodeID ori, NodeID des, NodeID newori, NodeID newdes, TimeType t) {
//		if (graphs.remove_link_noID(lt,t,ori,des)) {
//			if (graphs.add_link_noID(lt,t,newori,newdes)) return true;
//			else {
//				graphs.add_link_noID(lt,t,ori,des);
//				return false;
//			};
//		}
//		else return false;
		graphs.remove_link_noID(lt,t,ori,des);
		graphs.add_link_noID(lt,t,newori,newdes);
		return true;
	};

	// write outputs (if necessary).
	void write_output(TimeType t) {};
	void write_summary(TimeType t, bool if_converged) {};

   public:
   	Links &links;
	Graphs &graphs;

	ParameterSet &param;
};

// Base class that represents Links and Graphs with one state.
template<class LinkState> class NetworkBaseLinks1S: public NetworkBaseLinks {
   public:
   	// Constructor.
	NetworkBaseLinks1S(Links1S<LinkState> &ll, Graphs & gg, ParameterSet &p): NetworkBaseLinks(ll,gg,p), links(ll) {};

	// Destructor.
	~NetworkBaseLinks1S() {};

	// Advance time for Links object.
	void advance_links(TimeType t) {
		links.advance(t);
		graphs.advance(t);
	};
	void advance_links_L(LinkType lt, TimeType t) {
		links.advance_L(lt,t);
		graphs.advance_L(lt,t);
	};

	// Create a link with an ID (with the delay) (there is no link state in this class). (can be only used with links with ID).
	bool create_link_generatedID(LinkID &id, LinkType lt, NodeID ori, NodeID des, DelayType d, TimeType t) { // ID will be chosen automatically.
		if (links.add_link_generatedID(id,lt,t,d)) {
			if (graphs.add_link(id,lt,t,ori,des)) return true;
			else {
				links.remove_link(id,lt,t);
				return false;
			};
		}
		else return false;
	};
	// can be used with links with ID or links without ID
	bool create_link(LinkID id, LinkType lt, NodeID ori, NodeID des, DelayType d, TimeType t) {
		if(!links.if_type(lt) || !graphs.access_graph(lt,t).if_link_ID()) throw Bad_Link_Type();
		if (graphs.add_link(id,lt,t,ori,des)) {
			if (links.add_link(id,lt,t,d)) return true;
			else {
				graphs.remove_link(id,lt,t,ori,des); // If it fails to add link to Links, it has to remove that link from Graphs.
				return false;
			};
		}
		else return false;
	};
	bool create_link_noID(LinkType lt, NodeID ori, NodeID des, TimeType t) {
		if(!links.if_type(lt) || graphs.access_graph(lt,t).if_link_ID()) throw Bad_Link_Type();
		if (graphs.add_link_noID(lt,t,ori,des)) {
			if (links.add_link_noID(lt,t)) return true; // id is dummy here
			else {
				graphs.remove_link_noID(lt,t,ori,des); // If it fails to add link to Links, it has to remove that link from Graphs.
					return false;
			};
		}
		else return false;
	};

	// Remove a link (with ID or without ID).
	bool remove_link(LinkID id, LinkType lt, NodeID ori, NodeID des, TimeType t) {
		if(!links.if_type(lt) || !graphs.access_graph(lt,t).if_link_ID()) throw Bad_Link_Type();
		if (graphs.remove_link(id,lt,t,ori,des)) {
			if (links.remove_link(id,lt,t)) return true;
			else {
				graphs.add_link(id,lt,t,ori,des);
				return false;
			};
		}
		else return false;
	};
	bool remove_link_noID(LinkType lt, NodeID ori, NodeID des, TimeType t) {
		if(!links.if_type(lt) || graphs.access_graph(lt,t).if_link_ID()) throw Bad_Link_Type();
		if (graphs.remove_link_noID(lt,t,ori,des)) {
			links.remove_link_noID(lt,t);	// No ID needed, this only tells Links that one link will be deleted.
			return true;
		}
		else return false;
	};

	// Rewire a link with ID.
	bool rewire_link(LinkID id, LinkType lt, NodeID ori, NodeID des, NodeID newori, NodeID newdes, TimeType t) {
		if (graphs.remove_link(id,lt,t,ori,des)) {
			if (graphs.add_link(id,lt,t,newori,newdes)) return true;
			else {
				graphs.add_link(id,lt,t,ori,des);
				return false;
			};
		}
		else return false;
	};

	// Rewire an ID-less link.
	bool rewire_link_noID(LinkType lt, NodeID ori, NodeID des, NodeID newori, NodeID newdes, TimeType t) {
		if (graphs.remove_link_noID(lt,t,ori,des)) {
			if (graphs.add_link_noID(lt,t,newori,newdes)) return true;
			else {
				graphs.add_link_noID(lt,t,ori,des);
				return false;
			};
		}
		else return false;
	};

	// Access a link state.
	LinkState & access_link_state(LinkID id, LinkType lt, TimeType t) {
		return links.access_state(id,lt,t);
	};
	
	// Update a link state.
	bool update_link_state(LinkID id, LinkType lt, const LinkState &s, TimeType t) {
		return links.set_state(id,lt,t,s);
	};

   public:
   	Links1S<LinkState> &links;
	Graphs &graphs;
};

//=================================================================================================================
// Below are examples of subclasses from above base classes using multiple inheritance.
// We can choose a Network class from below and inherit it with definitions of undefined member functions, some of which
//	depend on dynamical rules given to the system.
// Here 4 examples are given, but any other type can be added later.

// Example1:
// Network with no Node state, no Link state, and no Input state.
// These member functions should be overwritten in subclasses: advance_nodes, advance_links, create_nodes, 
//	remove_nodes, update_nodes, create_links, remove_links, update_links, rewire_links (rule-based so far).
//	initialize_nodes, initialize_links, write_nodes_T, write_links_T, write_output, write_summary.
class Network1: public NetworkBaseNodes, public NetworkBaseLinks {
   public:
   	// Constructor.
	Network1(Nodes &nn, Links &ll, Graphs & gg, ParameterSet &p): \
		NetworkBaseNodes(nn), NetworkBaseLinks(ll,gg,p) {};

	// Destructor.
	~Network1() {};
};

// Example2:
// Network with one Node state (fixed number), no Link state, and no Input state.
// These member functions should be overwritten in subclasses: advance_nodes, advance_links, create_nodes, 
//	remove_nodes, update_nodes, create_links, remove_links, update_links, rewire_links (rule-based so far).
//	initialize_nodes, initialize_links, write_nodes_T, write_links_T, write_output, write_summary.
template<class NodeState> class Network2: public NetworkBaseNodesNF1S<NodeState>, public NetworkBaseLinks {
   public:
   	// Constructor.
	Network2(NodesNF1S<NodeState> &nn, Links &ll, Graphs & gg, ParameterSet &p): \
		NetworkBaseNodesNF1S<NodeState>(nn), NetworkBaseLinks(ll,gg,p) {};

	// Destructor.
	~Network2() {};
};

// Example3:
// Network with one Node state (fixed number), one Link state, and no Input state.
// These member functions should be overwritten in subclasses: advance_nodes, advance_links, create_nodes, 
//	remove_nodes, update_nodes, create_links, remove_links, update_links, rewire_links (rule-based so far).
//	initialize_nodes, initialize_links, write_nodes_T, write_links_T, write_output, write_summary.
template<class NodeState, class LinkState> class Network3: public NetworkBaseNodesNF1S<NodeState>, public NetworkBaseLinks1S<LinkState> {
   public:
   	// Constructor.
	Network3(NodesNF1S<NodeState> &nn, Links1S<LinkState> &ll, Graphs & gg, ParameterSet &p): \
		NetworkBaseNodesNF1S<NodeState>(nn), NetworkBaseLinks1S<LinkState>(ll,gg,p) {};

	// Destructor.
	~Network3() {};
};

// Example4:
// Network with one Node state (fixed number), one Link state, and one Input state.
// These member functions should be overwritten in subclasses: advance_nodes, advance_links, create_nodes, 
//	remove_nodes, update_nodes, create_links, remove_links, update_links, rewire_links (rule-based so far).
//	initialize_nodes, initialize_links, write_nodes_T, write_links_T, write_output, write_summary.
template<class NodeState, class LinkState, class InputState> class Network4: public NetworkBaseNodesNF1S<NodeState>, public NetworkBaseLinks1S<LinkState> {
   public:
   	// Constructor.
	Network4(NodesNF1S<NodeState> &nn, Links1S<LinkState> &ll, GraphsFixed & gg, NodeInputs1S<InputState> &ii): 
		NetworkBaseNodesNF1S<NodeState>(nn), NetworkBaseLinks1S<LinkState>(ll,gg), inputs(ii) {};

	// Destructor.
	~Network4() {};

   public:
	NodeInputs1S<InputState> &inputs; // an object that contains all inputs for one type.
};

// Other types of networks can be added later on...

}; // End of namespace conet.
#endif
