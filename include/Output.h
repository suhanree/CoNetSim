// 
//	CONETSIM - Output.h
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

#ifndef OUTPUT_H
#define OUTPUT_H

#include "Graph.h"
#include <fstream>
#include <string>

namespace conet {

// An base class that handles the output.
// This class will be inherited to a derived class,
// which will contain more specific member functions
// for outputs.

class Output {
   public:
   	// Constructors.
	Output(const std::string &file);
	Output(const char *file);
	// Destructor.
   	~Output() {
		if (ofile) ofile.close();
	};	

	// Write objects of fundamental types.
	void write(bool tf) {
		ofile << tf;
	};
	void write(short sh) {
		ofile << sh;
	};
	void write(int in) {
		ofile << in;
	};
	void write(long lo) {
		ofile << lo;
	};
	void write(unsigned long lo) {
		ofile << lo;
	};
	void write(char c) {
		ofile << c;
	};
	void write(const char *str) {
		ofile << str;
	};
	void write(std::string &str) {
		ofile << str;
	};
	void write(double a) {
		ofile << a;
	};
	void write(float a) {
		ofile << a;
	};


	// Write the information about a graph.
	// Format 1: (Time) Ori Des (ID)
	// Format 2: (Time) Des Ori1 Ori2...
	// Format 3: Pajek .net
	void write_graph1(const Graph &gra, long time=-1) {
		gra.write1(ofile,time);
	};
      
	void write_graph2(const Graph &gra, long time=-1) {
		gra.write2(ofile,time);
	};

	void write_graph3(const Graph &gra) {
		gra.write3(ofile);
	};

	// Close the file.
	void close() {
		ofile.close();
	};

   public:
	static std::string noout; // To be used when there is no output for this file.

   	std::string filename; // Output file name.

	std::ofstream ofile; // Output file stream.

   private:
   	Output() {}; // Prevent using the default constructor.
};

}; // End of namespace conet.
#endif
