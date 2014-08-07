// 
//	CONETSIM - Input.h
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

#ifndef INPUT_H
#define INPUT_H

#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include "Graph.h"

class ParameterSet; // Forward declaration.

namespace conet {

// A class that handles the input file.

class Input {
   public:
   	// Constructors
   	Input() {}; // the default constructor.
   	Input(const std::string &file);
   	Input(const char *file);
   	~Input() {
		ifile.close();
	};

	// open and close the file.
	void open(std::string &file);	// open it with a new file.
	void close() {
		ifile.close();
	};

	// is_open
	bool is_open() {
		return ifile.is_open();
	};

	// eof
	bool eof() {
		return ifile.eof();
	};

	// Relocate/tell the pointer.
	void seek(long pos) {
		ifile.clear();	// if it read the eof, and failed, it has to be reset.
		ifile.seekg(pos);
	};
	void rewind() {
		ifile.clear();	// if it read the eof, and failed, it has to be reset.
		ifile.seekg(0);
	};
	long tell() {
		return ifile.tellg();
	};

	// Read objects of fundamental types.
	bool read(bool &tf) {
		return ifile >> tf;
	};
	bool read(short &sh) {
		return ifile >> sh;
	};
	bool read(int &in) {
		return ifile >> in;
	};
	bool read(long &lo) {
		return ifile >> lo;
	};
	bool read(unsigned long &lo) {
		return ifile >> lo;
	};
	bool read(char &c) {
		return ifile >> c;
	};
	bool read(std::string &str) {
		return ifile >> str;
	};
	bool read(double &a) {
		return ifile >> a;
	};
	bool read(float &a) {
		return ifile >> a;
	};

	// Peek
	int peek() {
		return ifile.peek();
	};

	// get the filename.
	std::string get_filename() const {
		return filename;
	};

	// Read the given number of lines and ignore them
	bool read_lines(int nlines);

	// Read the graph from the input file.
	void read_graph(Graph &gra) {
		gra.read1(ifile);
	};
	void read_graph2(Graph &gra) {
		gra.read2(ifile);
	};
	  
   private:
   	std::string filename; // Input file name.
   protected:
	std::ifstream ifile; // Input file stream.

};

// A class that handles the input file for input parameters.
// Usage:	Input inp(arg[1]); // Input filename given.
//		inp.read(); // Read all the key-value pairs.
//
//		// When there are two kinds of keys, "ABC" and "DFG",
//		// representing a (int) and d (double) respectively,
//		// a and d will be obtained like this.
//		inp.get("ABC", a);
//		inp.get("DFG", d);
class InputParam : public Input {
   public:
	// "infile" is the filename of the input file.
   	InputParam(const std::string &infile, ParameterSet &p): Input(infile), key_val(), param(p) {};
   	InputParam(const char *infile, ParameterSet &p): Input(infile), key_val(), param(p) {};
   	~InputParam() {};

	// Read all parameters from the given file and store in the ParameterSet structure.
	// This member function has to be overwritten in a subclass to use this function.
	// "struct ParameterSet" has to be defined first.
	// Here Input::get() and InputParam::read() will be used to assign values to the parameters.
	void read_all_parameters() {};

	// Read the input file and save the info in the map.
	// When the same key appears more than once, the last one overwrite
	// the previous values.
	void read();

	// Print key-value pairs read.
	void print_key_val();

	// Delete the all values stored.
	void clear() {
		key_val.clear();
	};

  	// Return the value of a key in the second argument
	bool get(const std::string str,  std::string &str1, std::ostream &ofile);
	bool get(const std::string str,  bool &tf, std::ostream &ofile);
	bool get(const std::string str,  short &num, std::ostream &ofile);
	bool get(const std::string str,  int &num, std::ostream &ofile);
	bool get(const std::string str,  long &num, std::ostream &ofile);
	bool get(const std::string str,  unsigned long &num, std::ostream &ofile);
	bool get(const std::string str,  float &num, std::ostream &ofile);
	bool get(const std::string str,  double &num, std::ostream &ofile);
	bool get(const std::string str,  std::map<long,std::string> &times, std::ostream &ofile);

   private:
	// Input parameters will be given as a key value pair inside
	// the input file.
	std::map<std::string,std::string> key_val;
	
   public:
	// Parameter set that has all the parameters in the structure.
	ParameterSet &param;
};

}; // End of namespace conet.
#endif
