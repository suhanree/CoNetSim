// 
//	CONETSIM - Point.h
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

#ifndef POINT_H
#define POINT_H

#include <iostream>
#include <istream>
#include <ostream>
#include <fstream>
#include <set>


namespace conet {

// Defines the classes for 2D points and toroidal 2D points (integer).

// class for 2D integer.
class Point2D {
   public :
   	// Constructor.
	Point2D() : x(0), y(0) {};
	Point2D(long ix, long iy)
		: x(ix), y(iy) {};
	// Destructor.
	~Point2D() {};

	// Change x and y values.
	void update_x(long lx) {
		x=lx;
	};
	void update_y(long ly) {
		y=ly;
	};
	void update_pos(long ix, long iy) {
		x=ix; y=iy;
	};
	void update_pos(const Point2D & pos) {
		x=pos.x; y=pos.y;
	};
	
	void get_right() {};

	// Read the point from a file.
	bool read(std::ifstream &ifile) {
		return ifile >> x >> y;
	};

	// get the values.
	long get_x() const {return x;};
	long get_y() const {return y;};

	// Set the size of the square grid.
	static void set_size(long xs, long ys) {
		xsize=xs;
		ysize=ys;
	};

	// Get the size.
	static long get_xsize() {
		return xsize;
	};
	static long get_ysize() {
		return ysize;
	};

	// check the point is inside the grid.
	bool if_inside() const {
		return (x>=0 && x<xsize && y>=0 && y<ysize);
	};

	// Print the values.
	void print() const {
		std::cout << '(' << x << ',' << y << ")\n";
	};
	void write(std::ofstream &of) const {
		of << x << '\t' << y << '\n';
	};

	// overloading the unary operators +=, -=, -
	Point2D & operator+=(Point2D p2) {
		x+=p2.x; y+=p2.y;
		return *this;
	};
	Point2D & operator-=(Point2D p2) {
		x-=p2.x; y-=p2.y;
		return *this;
	};
	Point2D operator-() {
		return Point2D(-x,-y);
	};
	
	// overloading the binary operators, ==, +, -.
	bool operator==(const Point2D p2) const {
		return (x==p2.x && y==p2.y);
	};
	Point2D operator+(Point2D p2) const {
		return (p2+=*this);
	};
	Point2D operator-(Point2D p2) const {
		return (-(p2-=*this));
	};

   public : // To access the point values easier.
   	long x, y;
   
   private :
	static long xsize;
	static long ysize;
};

// A class that defines a 2D torus.
class Torus {
   public:
   	Torus(): xsize(10), ysize(10) {};
   	Torus(long xs, long ys);
	~Torus() {};

	void set_size(long xs, long ys) {
		xsize=xs;
		ysize=ys;
	};

	long xsize;
	long ysize;
};

// A class that defines a point in a 2D torus.
class Point2DTorus : public Point2D {
   public :
   	// Constructors.
   	Point2DTorus(): Point2D() {};
	Point2DTorus(long ix, long iy): Point2D(ix,iy) {
		this->torus();
	};
	Point2DTorus(const Point2D &pos1): Point2D(pos1) {
		this->torus();
	};
	Point2DTorus(const Point2DTorus &pos1): Point2D(pos1.x, pos1.y) {};

	// Destructor.
	~Point2DTorus() {};

	// Change x and y values.
	void update_x(long lx) {
		x=lx;
		(x>=0? x%=tor.xsize : x=tor.xsize+x%tor.xsize);
	};
	void update_y(long ly) {
		y=ly;
		(y>=0? y%=tor.ysize : y=tor.ysize+y%tor.ysize);
	};
	void update_pos(long ix, long iy) {
		x=ix; y=iy;
		this->torus();
	};
	void update_pos(const Point2D & pos) {
		x=pos.x; y=pos.y;
		this->torus();
	};
	void update_pos(const Point2DTorus & pos) {
		x=pos.x; y=pos.y;
	};

	void get_right() {
		torus();
	};

	// Read the point from a file.
	bool read(std::ifstream &ifile) {
		bool tf = (ifile >> x >> y);
		this->torus();
		return tf;
	};

	// find the modularized point.
	void torus();

	// Set the torus size.
	static void set_size(long xs, long ys) {
		Point2D::set_size(xs,ys);
		tor.set_size(xs,ys);
	};

	// Get the torus size.
	static long get_xsize() {
		return tor.xsize;
	};
	static long get_ysize() {
		return tor.ysize;
	};

	// check the point is inside the grid. (always true for the torus).
	bool if_inside() const {
		return true;
	};

	// Print the values.
	void print() const {
		std::cout << '(' << x << ',' << y << ")\n";
	};
	void write(std::ofstream &of) const {
		of << x << '\t' << y << '\n';
	};

	// overloading the unary operators +=, -=, -
	Point2DTorus & operator+=(Point2DTorus p2) {
		x+=p2.x; y+=p2.y;
		this->torus();
		return *this;
	};
	Point2DTorus & operator-=(Point2DTorus p2) {
		x-=p2.x; y-=p2.y;
		this->torus();
		return *this;
	};
	Point2DTorus operator-() {
		return Point2DTorus(-x,-y);
	};
	
	// overloading the binary operators, ==, +, -.
	bool operator==(const Point2DTorus p2) const {
		return (x==p2.x && y==p2.y);
	};
	Point2DTorus operator+(Point2DTorus p2) const {
		p2+=*this;
		p2.torus();
		return (p2);
	};
	Point2DTorus operator-(Point2DTorus p2) const {
		p2-=*this;
		return p2=-p2;
	};

   public :
	static Torus tor;
};

// A class that defines the "less than" function for the 2D Point class.
class Point2DLessThan {
public :
	inline bool operator() (const Point2D & p1, const Point2D & p2) {
		return (p1.y==p2.y ? p1.x < p2.x : p1.y < p2.y);
	};
};

// A class that defines the "less than" function for the toroidal 2D Point class.
class Point2DTorusLessThan {
public :
	inline bool operator() (const Point2DTorus & p1, const Point2DTorus & p2) {
		return (p1.y==p2.y ? p1.x < p2.x : p1.y < p2.y);
	};
};


// Calculate the distance between 2 Point2D's.
double distance_2DInt(const Point2D &p1, const Point2D &p2);
double distance_2DInt(const Point2DTorus &p1, const Point2DTorus &p2);

// Find the relative neighbors for movement given a distance and a neighbor_type for a Point2DTorus.
void find_rel_neighbors(long d, int neighbor_type, std::set<Point2DTorus,Point2DTorusLessThan> &pset);

}; // End of namespace conet.

// instream/outstream operator overloading for Point2D.
inline std::istream & operator>>(std::istream &s, conet::Point2D &p) {
	return s >> p.x >> p.y;
};
inline std::ostream & operator<<(std::ostream &s, const conet::Point2D &p) {
	return s << p.x << '\t' << p.y;
};

// instream/outstream operator overloading for Point2DTorus.
inline std::istream & operator>>(std::istream &s, conet::Point2DTorus &p){
	return s >> p.x >> p.y;
};
inline std::ostream & operator<<(std::ostream &s, const conet::Point2DTorus &p) {
	return s << p.x << '\t' << p.y;
};

#endif
