// 
//	CONETSIM - Input.C
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


#include "Input.h"
#include <string>
#include <cstring>
#include <map>
#include <iostream>
#include <cstdlib>

#ifndef MAX_CHARS
#define MAX_CHARS 1000
#endif
#ifndef SUCCESS
#define SUCCESS true
#endif
#ifndef NO_KEY
#define NO_KEY false
#endif

#define MAX_CHARS 1000

namespace conet {

using namespace std;

// Constructors.
Input::Input(const string &file): filename(file) {
	ifile.open(filename.c_str(),ios::in);
	if (!ifile) {
		cerr << "# Error: Input file, " << filename;
		cerr << ", doesn't exist.\n";
		exit(1);
	};
};
Input::Input(const char *file): filename(file) {
	ifile.open(filename.c_str(),ios::in);
	if (!ifile) {
		cerr << "# Error: Input file, " << filename;
		cerr << ", doesn't exist.\n";
		exit(1);
	};
};

void Input::open(string &file) {
	if (ifile.is_open())
		ifile.close();
	ifile.open(file.c_str(),ios::in);
	if (!ifile) {
		filename=file;
		cerr << "# Error: Input file, " << filename;
		cerr << ", doesn't exist.\n";
		exit(1);
	};
};

// Read the given number of lines and ignore.
bool Input::read_lines(int nlines) {
	char tmpstr[MAX_CHARS+1];
	while (nlines--) {
		if (!ifile.getline(tmpstr, MAX_CHARS, '\n'))
			return false;
	};
	return true;
};

// Read the input file and save the info in the map.
void InputParam::read() {
	char tmpstr[MAX_CHARS+1];
	string key, value;

	do {
		if(ifile.peek()!='#' && ifile >> key >> value)
			key_val[key] = value;
	} while (ifile.getline(tmpstr, MAX_CHARS, '\n'));
};

// Print key-value pairs read.
void InputParam::print_key_val() {
	for(map<string,string>::const_iterator i=key_val.begin();\
		i!=key_val.end();i++)
		cout << i->first << '\t' << i->second << endl;
};
     
// Return the value of a key in the second argument
bool InputParam::get(const string str, string & str1, ostream &ofile) {
	if(key_val.find(str)!=key_val.end()) {
		str1 = key_val[str];
		return SUCCESS;
	};
	ofile << "# Parameter not found: " << str << '.' << endl;
	return NO_KEY;
};

bool InputParam::get(const string str, bool & tf, ostream &ofile) {
	if(key_val.find(str)!=key_val.end()) {
		char c=*(key_val[str].c_str());
		if (c=='t' || c=='T' || (c>='1' && c<='9')) {
			tf=true;
			return SUCCESS;
		}
		else if (c=='f' || c=='F' || c=='0') {
			tf=false;
			return SUCCESS;
		}
		else {
			ofile << "# Paramter, " << str;
			ofile << ", has invalid value." << '.' << endl;
			return NO_KEY;
		};
	};
	ofile << "# Parameter not found: " << str << '.' << endl;
	return NO_KEY;
};

bool InputParam::get(const string str, short & num, ostream &ofile) {
	if(key_val.find(str)!=key_val.end()) {
		num = atoi(key_val[str].c_str());
		return SUCCESS;
	};
	ofile << "# Parameter not found: " << str << '.' << endl;
	return NO_KEY;
};

bool InputParam::get(const string str, int & num, ostream &ofile) {
	if(key_val.find(str)!=key_val.end()) {
		num = atoi(key_val[str].c_str());
		return SUCCESS;
	};
	ofile << "# Parameter not found: " << str << '.' << endl;
	return NO_KEY;
};

bool InputParam::get(const string str, long & num, ostream &ofile) {
	if(key_val.find(str)!=key_val.end()) {
		num = atol(key_val[str].c_str());
		return SUCCESS;
	};
	ofile << "# Parameter not found: " << str << '.' << endl;
	return NO_KEY;
};

bool InputParam::get(const string str, unsigned long & num, ostream &ofile) {
	if(key_val.find(str)!=key_val.end()) {
		num = atol(key_val[str].c_str());
		return SUCCESS;
	};
	ofile << "# Parameter not found: " << str << '.' << endl;
	return NO_KEY;
};

bool InputParam::get(const string str, float & num, ostream &ofile) {
	if(key_val.find(str)!=key_val.end()) {
		num = atof(key_val[str].c_str());
		return SUCCESS;
	};
	ofile << "# Parameter not found: " << str << '.' << endl;
	return NO_KEY;
};

bool InputParam::get(const string str, double & num, ostream &ofile) {
	if(key_val.find(str)!=key_val.end()) {
		num = atof(key_val[str].c_str());
		return SUCCESS;
	};
	ofile << "# Parameter not found: " << str << '.' << endl;
	return NO_KEY;
};

bool InputParam::get(const string str, map<long,string> & times, ostream &ofile) {
	if(key_val.find(str)!=key_val.end() && !(key_val[str].empty())) {
		char temp_str[MAX_CHARS];
		char tok[]="-_/,:;";
		char *p;
		strcpy(temp_str,key_val[str].c_str());
		times.clear();
		p=strtok(temp_str,tok);
		times[atol(p)]=string("_t")+p+".net";
		while ((p=strtok(0,tok))!=0)
			times[atol(p)]=string("_t")+p+".net";
		return SUCCESS;
	};
	ofile << "# Parameter not found: " << str << '.' << endl;
	return NO_KEY;
};

};
