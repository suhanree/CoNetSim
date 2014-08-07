// 
//	CONETSIM - Point.C
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

#include <set>
#include "Point.h"
#include "Errors.h"

namespace conet {

using namespace std;

// Assigning the initial value for the static data member.
long Point2D::xsize=10;
long Point2D::ysize=10;

// Constructor for Torus class
Torus::Torus(long xs, long ys) {
	if (xs>0 && ys>0) {
		xsize=xs;
		ysize=ys;
	}
	else 
		throw conet::Bad_Torus_Size();
};

// find the modularized point.
void Point2DTorus::torus() {
	(x>=0? x%=tor.xsize : x=tor.xsize+x%tor.xsize);
	(y>=0? y%=tor.ysize : y=tor.ysize+y%tor.ysize);
};

// Assigning the initial value for the static data member (should be redifined later).
Torus Point2DTorus::tor=Torus(10,10);

// distance between 2 Point2D's.
double distance_2DInt(const Point2D &p1, const Point2D &p2) {
	Point2D diff=p1-p2;
	return diff.x+diff.y;
};

// Shortest possible distance between 2 Point's.
// (When points are in a torus, there can be many values.)
double distance_2DInt(const Point2DTorus &p1, const Point2DTorus &p2) {
	Point2DTorus diff=p1-p2;
	long x=diff.x;
	long y=diff.y;
	if (x>Point2DTorus::get_xsize()-x) x=Point2DTorus::get_xsize()-x;
	if (y>Point2DTorus::get_ysize()-y) y=Point2DTorus::get_ysize()-y;
	return x+y;
};

// Find the neighbors given a distance for a Point.
void find_rel_neighbors(long d, int neighbor_type, std::set<Point2DTorus,Point2DTorusLessThan> &pset) {
	if (d<0) throw Bad_Parameter();
	pset.clear();
	long sizex=Point2D::get_xsize(); // size of lattice in x
	long sizey=Point2D::get_ysize(); // size of lattice in y
	long disx=(d<sizex ? d : sizex-1); // if d>=sizex, d in x should be sizex-1
	long disy=(d<sizey ? d : sizey-1); // if d>=sizey, d in y should be sizey-1
	switch (neighbor_type) {
	   case 1: // von Neumann + (0,0)
		for (long y=-disy;y<=disy;y++) {
			long tem=d-(y>=0 ? y : -y);
			long tem2=(tem<=disx ? tem : disx-1);
			for (long x=-tem2;x<=tem2;x++)
				pset.insert(Point2D(x,y));
		};
		break;
	   case 2: // von Neumann
		for (long y=-disy;y<=disy;y++) {
			long tem=d-(y>=0 ? y : -y);
			long tem2=(tem<=disx ? tem : disx-1);
			for (long x=-tem2;x<=tem2;x++)
				if (x!=0 || y!=0) {
					pset.insert(Point2DTorus(x,y));
				};
		};
		break;
	   case 3: // Moore + (0,0)
		for (long y=-disy;y<=disy;y++) {
			for (long x=-disx;x<=disx;x++)
				pset.insert(Point2DTorus(x,y));
		};
		break;
	   case 4: // Moore
		for (long y=-disy;y<=disy;y++) {
			for (long x=-disx;x<=disx;x++)
				if (x!=0 || y!=0) {
					pset.insert(Point2DTorus(x,y));
				};
		};
		break;
	   default: // case 0
		for (long y=-disy;y<=disy;y++) {
			long tem=d-(y>=0 ? y : -y);
			long tem2=(tem<=disx ? tem : disx-1);
			for (long x=-tem2;x<=tem2;x++)
				pset.insert(Point2DTorus(x,y));
		};
	};
};
     
};
