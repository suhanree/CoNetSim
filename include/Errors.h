// 
//	CONETSIM - Errors.h
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

// errors for the CONETSIM project.

#include <string>

#ifndef ERRORS_H
#define ERRORS_H

namespace conet {

// Edge is bad (two vertices are the same, non-existing vertex, etc)
struct Bad_Edge {}; 

// Maximum number of random number for a given generator has been reached.
// To be effective the number of random numbers should be lower than randmax.
struct TOO_MANY_RN_NEEDED {}; 

// Maximum number of random number for each agent has been reached.
struct MAX_RN_REACHED {}; 

// General input.
struct Bad_Parameter {};
struct Bad_Input_File {
	Bad_Input_File(): filename() {};
	Bad_Input_File(std::string file): filename(file) {};
	std::string filename;
};
struct Bad_Input_Value {};

// NodeInput.
struct Bad_Input_ID {}; // ID for input node is bad.
struct Bad_Input_Time {}; // Time for input is bad.
struct Bad_Input_Type {}; // Time for input is bad.

//  For TimeSeq class. 
struct TimeSeq_Range_Error {}; // Not allowed value of t is used.
struct TimeSeq_Size_Set {};	// DRing size should be set only once.
struct TimeSeq_Size_Not_Set {};	// DRing size should be set only once.

// For Nodes and Links
struct Bad_Node_ID {};
struct Bad_Link_ID {};
struct Bad_Node_Type {};
struct Bad_Link_Type {};
struct Wrong_Node_Type {};
struct Wrong_Link_Type {};
struct NodeID_None_Available {};
struct LinkID_None_Available {};

// For torus definition
struct Bad_Torus_Size {};

}; // End of namespace conet.

#endif
