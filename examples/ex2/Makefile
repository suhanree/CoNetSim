#
#	CONETSIM - Makefile
#
#
#	Copyright (C) 2014	Suhan Ree
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


# Makefile for the Example 1 of conet libray.

ifndef COMPILER
   COMPILER=GCC
endif

# When MPI is NOT used, assigning the compiler (GCC=g++)
# When MPI is used, assigning the MPI compilers (hcc, mpicc)
ifeq ($(MPI),none)
   ifeq ($(COMPILER),GCC)
   	CXX = g++
   else 
      	CXX = $(COMPILER)
   endif
else
   ifeq ($(MPI),LAM)
   	CXX = /usr/bin/hcp
   else
   	ifeq ($(MPI),MPICH)
	   CXX = /usr/local/mpich/bin/mpiCC
	endif
   endif
endif

# defining the name of the object files.
objects := Ex2.o Ex2Network.o
executable := ../../bin/Ex2

# defining the name of the header files.
headers := Ex2.h Ex2Network.h

# The name of the archives.
name_conet = ../../lib/libconet.a

# For debugging, names of object files will have the '.debug.o' extension 
# (not implemented at this point, execute 'make clean' first before using DEBUG=1)
ifeq ($(DEBUG),1)
#   objects := $(patsubst %.o, %.debug.o, $(objects))
#   name := $(addsuffix .debug, $(name))
   CXXFLAGS := $(CXXFLAGS) -g
else
   CXXFLAGS := -O$(OPTIMIZE) 	# When not debugging, turn the optimization on.
endif

# path for the header files.
CXXFLAGS += -I../../include

# Primary Targets:

$(executable) : $(objects) $(name_conet)
	@ echo $(executable) " (linking...)"
	@ if [ ! -d "../../bin" ]; then mkdir ../../bin; fi
	@ $(CXX) $(objects) $(name_conet) -o $(executable)

Ex2.o : Ex2.C Ex2.h Ex2Network.h
	@ echo $(@F)
	@ $(CXX) $(CXXFLAGS) -c $< -o $@

Ex2Network.o : Ex2Network.C Ex2Network.h Ex2.h
	@ echo $(@F)
	@ $(CXX) $(CXXFLAGS) -c $< -o $@

# phony targets:
.PHONY : clean

clean :
	@ echo "Deleting"  $(objects) $(executable)
	@ rm -rf $(objects) $(executable)
