#
#	CONETSIM - Makefile
#
#
#	Copyright (C) 2011	Suhan Ree
#
#	This program is free software: you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation, either version 3 of the License, or
#	(at your option) any later version.
#	
#	This program is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#	
#	You should have received a copy of the GNU General Public License
#	along with this program.  If not, see <http://www.gnu.org/licenses/>.
#	
#	Author: suhanree@yahoo.com (Suhan Ree)

# Main makefile for conet.

# Options 
#	COMPILER (=GCC,CC) default=GCC
#	DEBUG=1 : if a user wants executables with debugging info. 
#		(the name of the executable will have '.debug' extension)
#	OPTIMIZE=N : setting the optimization level to N (default=2).


# Usage: (assuming the current directory is the same directory as this file exists.)
#	make buildall			: build all modules using default setting.
#		(should override COMPILER in Makefile.sim)
#	make MPI=MPICH		: make executables for SIM usig MPICH.
#	make DEBUG=1		: make executables for MOB using debugging,
#	make clean 		: delete all binaries and object files.


ifndef COMPILER
   COMPILER = GCC
endif

ifndef MPI
   MPI = none
endif

ifndef OPTIMIZE
   OPTIMIZE = 2
endif

# phony targets.
.PHONY : default clean

#exporting variables to sub-make's.
export COMPILER MPI DEBUG OPTIMIZE

default : 
	@ $(MAKE) -w -C src -f Makefile.SIM
clean : 
	@ $(MAKE) -w -C src -f Makefile.SIM clean

