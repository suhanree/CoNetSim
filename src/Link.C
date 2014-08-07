// 
//	CONETSIM - Link.C
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


#include "Link.h"
#include "Types.h"
#include <istream>
#include <ostream>
#include <iostream>

// Defining static data members here. (It should be changed later inside the simulator if necessary.)
conet::LinkType conet::LinkD::type;
conet::LinkType conet::LinkBR::type;
conet::LinkType conet::LinkDR::type;
