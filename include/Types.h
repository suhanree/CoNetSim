// 
//	CONETSIM - Types.h
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

#ifndef TYPES_H
#define TYPES_H

#include "Random4.h"
#include "Point.h"
#include <vector>
#include <set>
#include <map>

namespace conet {


typedef long NodeID;	// ID for a node.
typedef long LinkID;	// ID for a link.

typedef short NodeType;	// Type for Node.
typedef short InputType;// Type for Inputs for input nodes. 
typedef short LinkType;	// Type for Link.

typedef long TimeType;	// Time is represented by long.
typedef short DelayType;// Delay is represented by short.
typedef double WeightType;// WeightType is represented by double.

// Set of Node ID's
typedef std::vector<NodeID> NodeIDVec;
typedef std::set<NodeID> NodeIDSSet;
typedef std::multiset<NodeID> NodeIDMSet; // Multiset can be necesary when storing neighbors.

// Set of Link ID's
typedef std::vector<LinkID> LinkIDVec;
typedef std::set<LinkID> LinkIDSet;

// Map of Node ID's associated with LinkID 
typedef std::map<NodeID,LinkID> NodeIDLinkIDSMap;
typedef std::multimap<NodeID,LinkID> NodeIDLinkIDMMap;// Multimap can be necesary when storing neighbors.

// Random number generator for each object (Each has a limited number of outputs).
typedef Random4L RNG_Limit;
typedef Random4 RNG;

// Point
typedef std::set<Point2DTorus,Point2DTorusLessThan> PointSet;

}; // End of namespace conet.
#endif
