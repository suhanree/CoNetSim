// 
//	CONETSIM - Links.h
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


#ifndef LINKS_H
#define LINKS_H

#include "Link.h"
#include "TimeSeq.h"
#include "Types.h"
#include <istream>
#include <ostream>
#include <iostream>
#include <map>

namespace conet {

using namespace std;

// Base classes that contain all of current link objects with their ID's (only counts will be stored for ID-less links).
// (ID-less links will not be included in this container).
// Here more data like ID with types (maap) or types with ID sets (map) can be stored for later usages,
//	but decided not to, to save the memory (in case there are too many links).
// 	Subclasses can add those information for problems with manageable number of links.
// 
// We will implement 3 heirarchies of Links classes here.
//	(1) Links (Links with 0 state, not a template class): it can contain one or more out of {LinkD, LinkBR, LinkDR}.
//	(2) Links1S (Links with 1 state, template class): it can contain one out of 
//		{LinkFB, LinkFD, LinkVB, LinkVD, LinkFBR, LInkFDR, LinkVBR, LinkVDR}
//	(3) Links2S (Links with 2 states, template class): it can contain two out of 
//		{LinkFB, LinkFD, LinkVB, LinkVD, LinkFBR, LInkFDR, LinkVBR, LinkVDR}
//	There can be more with 3 states and so on (not implemented here), and it can be coded as below easily.


// struct for storing link type information
struct TypeInfo {
	DelayType max_delay;
	bool	noID; // true if links don't need IDs, false otherwise.

	// constructor
	TypeInfo(DelayType md=1, bool noid=false): max_delay(md), noID(noid) {};
};

// Abstract class for all Links subclasses.
// Subclasses of this class should overwrite below member functions.
//	get_type, clear, add_link, remove_link, get_delay, set_delay, write_T, write_ID_T, if_ID (from Links)
class Links {
   public:
   	// Constructor.
   	Links(TimeType &t): time(t), nlinks(), typeinfos(), max_ID(-1), current_greatest_ID(-1) {};
   	Links(TimeType &t, const std::map<LinkType,DelayType> &mds): time(t), typeinfos(), max_ID(-1), current_greatest_ID(-1) {
		for (std::map<LinkType,DelayType>::const_iterator i=mds.begin();i!=mds.end();i++) {
			nlinks[i->first]=0;
			typeinfos[i->first]=TypeInfo(i->second,false); // assuming links have IDs here.
		};
	};

	// Virtual destructor.
	virtual ~Links() {};
	
	// Add the type with max_delay for the type.
	bool add_type(LinkType lt, DelayType md, bool noid=false) {
		if (nlinks.find(lt)!=nlinks.end()) return false;
		nlinks[lt]=0;
		typeinfos[lt]=TypeInfo(md,noid);
		return true;
	};

	// Check if the given type exist.
	bool if_type(LinkType lt) const {
		return (nlinks.find(lt)!=nlinks.end());
	};

	// Check if the given type doesn't need link IDs (when delay=1, and no link state).
	bool if_type_noID(LinkType lt) const {
		if (this->if_type(lt)) return (typeinfos.find(lt)->second).noID;
		else throw Bad_Link_Type();
	};

	// Get the current time.
	TimeType get_time() const {
		return time;
	};

	// Get the number of LinkType's.
	LinkType get_ntypes() const {
		return nlinks.size();
	};

	// Get the number of links of the given type
	LinkType get_nlinks() const {
		LinkID sum=0;
		for (std::map<LinkType,LinkID>::const_iterator i=nlinks.begin();i!=nlinks.end();i++)
			sum+=(i->second);
		return sum;
	};
	LinkType get_nlinks(LinkType lt) const {
		std::map<LinkType,LinkID>::const_iterator i=nlinks.find(lt);
		if (i!=nlinks.end()) return (i->second);
		else throw Bad_Link_Type();
	};

	// Get the max_delay of the given link type.
	DelayType get_max_delay(LinkType lt) const {
		std::map<LinkType,TypeInfo>::const_iterator i=typeinfos.find(lt);
		if (i!=typeinfos.end()) return (i->second).max_delay;
		else throw Bad_Link_Type();
	};

	// Set the max_delays for all link types.
	bool set_max_delays(const std::vector<DelayType> &md) {
		if (md.size()!=typeinfos.size()) return false;
		for (LinkType lt=0;lt<typeinfos.size();lt++) {
			if (md[lt]>=1)  typeinfos[lt]=md[lt];
			else return false;
		};
		return true;
	};

	// Check if the delay is in the range.
	bool delay_check(LinkType lt, DelayType d) {
		if (typeinfos.find(lt)!=typeinfos.end()) return (d>0 && d<=typeinfos[lt].max_delay);
		else return false;
	};

	// Get the LinkType of the given link. (should be redefined in the subclasses).
	virtual LinkType get_type(LinkID id) const=0;

   	// Find if the given ID already exists.
   	virtual bool if_ID(LinkID id) const=0;
	
   	// Deleting all links (but type information remains).
	void clear_all() {
		for (std::map<LinkType,LinkID>::iterator i=nlinks.begin();i!=nlinks.end();i++)
			this->clear(i->first);
	};
	virtual void clear(LinkType lt) {
		if (!this->if_type(lt)) throw Bad_Link_Type();
		nlinks[lt]=0;
	};

	// Advancing the time of all current link objects upto the given time (if needed).
	// Some (or all) links might have been already advanced to the given time, but 
	//	if there are some links that are not advanced, then it will advance without change.
	virtual void advance(TimeType t) {};
	virtual void advance_L(LinkType lt, TimeType t) {};

	// Adding a link at the current time.
	// For links with ID, when id is given.
	virtual bool add_link(LinkID id, LinkType lt, TimeType t, DelayType d=1) {
		if (!if_type(lt) || if_type_noID(lt)) throw Bad_Link_Type();
		if (id<0 || this->if_ID(id)) return false; // ID not allowed
		if (!Links::delay_check(lt,d)) return false;
		nlinks[lt]++;
		if (current_greatest_ID<id) current_greatest_ID=id;
		return true;
	};
	// For links with ID, when id will be chosen automatically.
	bool add_link_generatedID(LinkID &id, LinkType lt, TimeType t, DelayType d=1) { // id represent a new ID
		if (!if_type(lt) || if_type_noID(lt)) throw Bad_Link_Type();
		LinkID new_id=Links::find_new_ID();
		if (new_id>=0) {
			id=new_id;
			return this->add_link(new_id,lt,t,d);
		}
		else	false;
	};
	// For links with no ID (special case when there is no link state and delays are all 1).
	bool add_link_noID(LinkType lt, TimeType t) {
		if (!if_type_noID(lt)) throw Bad_Link_Type();
		nlinks[lt]++;
		return true;
	};

	// Removing a link with the given ID (the chosen link will be erased at the current time, and 
	//	all information including the past states will disappear, too).
	// There will be no delayed removal as in nodes because a link removed at time t will not be needed at time >t; 
	// 	hence past states of that link can be deleted.
	virtual bool remove_link(LinkID id, LinkType lt, TimeType t) {
		if (!if_type(lt) || if_type_noID(lt)) throw Bad_Link_Type();
		if (if_ID(id)) nlinks[lt]--;
		return true;
	};
	// For links with no ID.
	bool remove_link_noID(LinkType lt, TimeType t) {
		if (!if_type_noID(lt)) throw Bad_Link_Type();
		nlinks[lt]--;
		return true;
	};

	// Get the delay of the given link.
	virtual DelayType get_delay(LinkID id, LinkType lt) const=0;

	// Set the delay of the given link.
	// Return false, if failed.
	virtual bool set_delay(LinkID id, LinkType lt, DelayType d)=0;

	// Write the links info.
	void write(std::ostream &of) {
		this->write_T(of,time);
	};
	virtual void write_T(std::ostream &of, TimeType t) const=0;
	virtual void write_ID_T(std::ostream &of, LinkID id, TimeType t) const=0;

   protected:
   	// Finding a new ID (new ID should not already exist).
	// Returns -1, if there is no available ID.
   	LinkID find_new_ID() {
		// Before the current_greates_ID reaches the maximum value for the given LinkID type.
		if (max_ID==-1) { // Maximum possible value hasn't been reached
			if (++current_greatest_ID<0)  // Maximum possible value has been reached here.
				max_ID=current_greatest_ID--; 	// Max_ID is set as the maximum possible value.
			else return current_greatest_ID;
		}
		// After the max_ID has been reached, we have to find an unused ID.
		//	(Expensive, it can be better.)
		else 
			for (LinkID id=0;id<=max_ID;id++)
				if (!(this->if_ID(id))) return id; 
		return -1;
	};

   protected:
	const TimeType &time;	// Reference for the current time (keeping track of the current time of the whole system).
	
	std::map<LinkType,LinkID> nlinks;	// Current number of links for each type.

	std::map<LinkType,TypeInfo> typeinfos; // Maximum delays for eacy type.

	LinkID max_ID;	// maximum value for the LinkID type.
	LinkID current_greatest_ID;	// current greatest ID.
};

// A derived template class of "Links" for links that only has one Link state involved (Links1S).
// For convenience, we will call the class that represents Link State as "State".
// Subclasses of this class need to overwrite below member functions
//	get_type, clear, advance, advance_L, add_link, remove_link, get_delay, set_delay, write_T, write_ID_T, if_ID (from Links)
//	add_link_S, get_state, access_state, set_state, initialize_random, initialize_file (from Links1S)
template<class State> class Links1S: public Links {
   public:
   	// Constructor.
   	Links1S(TimeType &t, DelayType md=1, LinkType lt=0): Links(t), type(lt) {
		Links::add_type(lt,md);
	};
   	Links1S(TimeType &t, const std::vector<DelayType> &mds, DelayType md=1, LinkType lt=0): Links(t,mds), type(lt) {
		if (Links::typeinfos.find(lt)==Links::typeinfos.end()) Links::add_type(lt,md);
	};

	// Virtual destructor.
	~Links1S() {};
	
	// Get the type of the associated state.
	NodeType get_type_S() const {
		return type;
	};

	// Adding a link at the current time.
	// (To provide the specific state of the link, define the member function 
	//	in the subclasses of Links1S, using the appropriate subclass of Link.)
	// For links with ID, when id is given.
	bool add_link(LinkID id, LinkType lt, TimeType t, DelayType d=1) {
		if (!if_type() || if_type_noID(lt)) throw Bad_Link_Type();
		if (lt==get_type_S()) return this->add_link_S(id,lt,t,State(),d);
		return Links::add_link(id,lt,t,d);
	};
	virtual bool add_link_S(LinkID id, LinkType lt, TimeType t, const State &s, DelayType d=1)=0;
	bool add_link_S_generatedID(LinkID &id, LinkType lt, TimeType t, const State &s, DelayType d=1) { // id represent a new ID
		LinkID new_id=Links::find_new_ID();
		if (new_id>=0) {
			id=new_id;
			return this->add_link_S(new_id,lt,t,s,d);
		}
		else	false;
	};

	// Get the state of the given link.
	virtual State get_state(LinkID id, LinkType lt, TimeType t) const=0;

	// Access the state of the given link.
	virtual State & access_state(LinkID id, LinkType lt, TimeType t)=0;

	// Set the Link state of the given link (should be overwritten in subclasses).
	virtual bool set_state(LinkID id, LinkType lt, TimeType t, const State &s, DelayType d=1)=0;

	// Initialize all links.
	virtual void initialize_random(LinkType lt) {};
	virtual void initialize_file(LinkType lt, const std::string &filename) {};

   protected:
   	LinkType type; // Type associated with given state.
};

// A derived template class of "Links1S" for links that has two Link states involved (Links2S).
// For convenience, we will call the class that represents Link States as "State1" and "State2".
// Subclasses of this class need to overwrite below member functions
//	get_type, clear, advance, advance_L, add_link, remove_link, get_delay, set_delay, write_T, write_ID_T, if_ID (from Links)
//	add_link_S, get_state, access_state, set_state, initialize_random, initialize_file (from Links1S)
//	add_link_S2, get_state2, access_state2, set_state2, (from Links2S)
template<class State1, class State2> class Links2S: public Links1S<State1> {
   public:
   	// Constructor.
   	Links2S(TimeType &t, DelayType md=1, LinkType lt=0, DelayType md2=1, LinkType lt2=1): Links1S<State1>(t,md,lt), type2(lt2) {
		if (type2==Links1S<State1>::type) throw Bad_Link_Type();
		Links::add_type(lt2,md2);
	};
   	Links2S(TimeType &t, const std::vector<DelayType> &mds, DelayType md=1, LinkType lt=0, DelayType md2=1, LinkType lt2=1): Links1S<State1>(t,mds,lt), type2(lt2) {
		if (type2==Links1S<State1>::type) throw Bad_Link_Type();
		if (Links::if_type(lt2)) Links::add_type(lt2,md2);
	};

	// Virtual destructor.
	~Links2S() {};
	
	// Get the type of the associated state.
	NodeType get_type_S2() const {
		return type2;
	};

	// Adding a link at the current time.
	// For links with ID, when id is given.
	bool add_link(LinkID id, LinkType lt, TimeType t, DelayType d=1) {
		if (!Links::if_type(lt) || Links::if_type_noID(lt)) throw Bad_Link_Type();
		if (lt==Links1S<State1>::get_type_S()) return this->add_link_S(id,lt,t,State1(),d);
		if (lt==get_type_S2()) return this->add_link_S2(id,lt,t,State2(),d);
		return Links::add_link(id,lt,t,d);
	};
	virtual bool add_link_S2(LinkID id, LinkType lt, TimeType t, const State2 &s, DelayType d=1)=0;
	bool add_link_S2_generatedID(LinkID &id, LinkType lt, TimeType t, const State2 &s, DelayType d=1) { // id represent a new ID
		LinkID new_id=Links::find_new_ID();
		if (new_id>=0) {
			id=new_id;
			return this->add_link_S2(new_id,lt,t,s,d);
		}
		else	false;
	};

	// Get the state of the given link.
	virtual State2 get_state2(LinkID id, LinkType lt, TimeType t) const=0;

	// Access the state of the given link.
	virtual State2 & access_state2(LinkID id, LinkType lt, TimeType t)=0;

	// Set the Link state of the given link (should be overwritten in subclasses).
	virtual bool set_state2(LinkID id, LinkType lt, TimeType t, const State2 &s, DelayType d=1)=0;

   protected:
   	LinkType type2; // Type associated with given state (State2).
};

// Example1:
// Subclass of Links: it contains one or more types of varying number of links with delay=1 and no state (IDless). (no RNG)
//	Here this class only keeps the number of links for each type.
class LinksPlain : public Links {
   public:
   	// Constructor.
   	LinksPlain(TimeType &t): Links(t) {};
   	LinksPlain(TimeType &t, LinkType ntypes): Links(t) {
		for (LinkType lt=0;lt<ntypes;lt++) Links::add_type(lt,1,true); // two types are all IDless.
	};

	// Virtual destructor.
	~LinksPlain() {};
	
	// Get the LinkType of the given link. (no ID for links here).
	LinkType get_type(LinkID id) const {
		throw Bad_Link_ID();
	};

   	// Find if the given ID already exists.
   	virtual bool if_ID(LinkID id) const {
		throw Bad_Link_ID();
	};
	
   	// Deleting all links at the current time.
	void clear(LinkType lt) {
		Links::clear(lt);
	};
	
	// Advancing the time of all current link objects upto the current time (no need because no state).
	void advance(TimeType t) {};
	void advance_L(LinkType lt, TimeType t) {};

	// Adding a link.
	bool add_link(LinkID id, LinkType lt, TimeType t, DelayType d=1) {
		throw Bad_Link_ID();
	};

	// Removing a link with the given type. (ID-less, id can be any value here).
	bool remove_link(LinkID id, LinkType lt, TimeType t) {
		throw Bad_Link_ID();
	};

	// Get the delay of the given link.
	DelayType get_delay(LinkID id, LinkType lt) const {
		return 1;
	};

	// Set the delay of the given link.
	// Return false, if failed.
	bool set_delay(LinkID id, LinkType lt, DelayType d) {
		return false;
	};

	// Write the links info.
	void write_T(std::ostream &of, TimeType t) const {};
	void write_ID_T(std::ostream &of, LinkID id, TimeType t) const {};
};

// Example2:
// Subclass of Links: it contains one type of varying number of links with inhomogeneous delays and no state. (no RNG)
class LinkDs : public Links {
   public:
   	// Constructor.
	// There is only one type.
	LinkDs(TimeType &t, DelayType md, LinkType lt=0): Links(t), type(lt), linkset() {
		Links::add_type(lt,md,false); // It needs ID, because links have inhomogeneous delays.
	}; 

	// Destructor
	~LinkDs() {};

	// Get the LinkType of the given link.
	LinkType get_type(LinkID id) const {
		if (!if_ID(id)) throw Bad_Link_ID();
		return type;	// Only type is type.
	};

   	// Find if the given ID already exists.
   	bool if_ID(LinkID id) const {
		return (linkset.find(id)!=linkset.end());
	};

   	// Deleting all links at the current time.
	virtual void clear(LinkType lt) {
		Links::clear(lt);
		linkset.clear();
	};
	
	// Adding a link with the default link state.
	// (Will be used only when initializing at t=0)
	bool add_link(LinkID id, LinkType lt, TimeType t, DelayType d=1) {
		if (Links::add_link(id,lt,t,d)) {
			pair<map<LinkID,LinkD>::iterator,bool> temp= \
				linkset.insert(map<LinkID,LinkD>::value_type(id,d));  
			return true;
		}
		else	return false; // cannot be added.
	};

	// Removing a link with the given ID. (will not be used by assumption)
	bool remove_link(LinkID id, LinkType lt, TimeType t) {
		if (Links::remove_link(id,lt,t)) {
			linkset.erase(id);
			return true;
		}
		else	return false; // cannot be removed.
	};

	// Get the delay of the given link.
	DelayType get_delay(LinkID id, LinkType lt) const {
		if (lt!=type) throw Bad_Link_Type();
		if (!Links::if_ID(id)) throw Bad_Link_ID();
		return (linkset.find(id)->second).get_delay();
	};

	// Set the delay of the given link.
	// Return false, if failed.
	bool set_delay(LinkID id, LinkType lt, DelayType d) {
		if (lt!=type) throw Bad_Link_Type();
		if (!Links::if_ID(id)) throw Bad_Link_ID();
		if (!Links::delay_check(lt,d)) return false;
		linkset[id].set_delay(d);
		return true;
	};

	// Write the links info.
	void write_T(std::ostream &of, TimeType t) const {
		for (std::map<LinkID,LinkD>::const_iterator i=linkset.begin();i!=linkset.end();i++)
			of << i->first << '\t' << (i->second).get_delay() << '\n';
	};
	void write_ID_T(std::ostream &of, LinkID id, TimeType t) const {
		if (Links::if_ID(id)) of << id << '\t' << (linkset.find(id)->second).get_delay() << '\n';
	};

  
   protected:
	LinkType type;
   	std::map<LinkID,LinkD> linkset;
};

// Example3:
// Subclass of Links1S: it contains one type of the fixed number of links with inhomogeneous delays and State. (no RNG)
template<class State> class LinkDVSs : public Links1S<State> {
   public:
   	// Constructor.
	// There is only one type.
	LinkDVSs(TimeType &t, DelayType md, LinkType lt=0): Links1S<State>(t,md,lt), linkset() {};

	// Destructor
	~LinkDVSs() {};

	// Get the LinkType of the given link.
	LinkType get_type(LinkID id) const {
		if (!if_ID(id)) throw Bad_Link_ID();	// non-existing ID. 
		return Links1S<State>::get_type_S();
	};

   	// Find if the given ID already exists.
   	bool if_ID(LinkID id) const {
		return (id<linkset.size() && id>=0);
	};
  
   	// Deleting all links at the current time.
	virtual void clear(LinkType lt) {
		Links::clear(lt);
		linkset.clear();
	};
	
	// Advancing the time to the given time for all current link objects, if needed.
	void advance(TimeType t) {
		if (Links1S<State>::time>t) return;
		for (LinkID id=0;id<Links::get_nlinks(Links1S<State>::get_type_S());id++) {
			if (linkset[id].get_last_time()<t) { // outside the range of the time sequence
				while (linkset[id].get_last_time()<t)
					linkset[id].advance_without_change();
			}
			else { 	// If the time is still inside the range of the time sequence.
				// Assumes that time advances with 1 and 
				// that the state value is kept from the previous valule.
				linkset[id].set_state_T(linkset[id].access_state_T(t-1),t);
			};
		};
	};
	void advance_L(LinkType lt, TimeType t) {
		if (this->get_type_S()==lt) this->advance(t);
	};

	// Adding a link with the default link state.

	// Adding a link with the default link state.
	// (Will be used only when initializing at t=0)
	// ID should increase by one.
	bool add_link(LinkID id, LinkType lt, TimeType t, DelayType d=1) {
		if (lt!=Links1S<State>::get_type_S()) throw Bad_Link_Type(); // There is only one type.
		return this->add_link_S(id,lt,t,State(),d);
	};
	// Can be added when id=current_greatest_id+1(=nlinks[lt]) when time=0.
	bool add_link_S(LinkID id, LinkType lt, TimeType t, const State &s, DelayType d=1) {
		if (lt!=Links1S<State>::get_type_S()) throw Bad_Link_Type(); // There is only one type.
		if (t!=0 || id!=Links1S<State>::nlinks[lt]) return false;
		if (Links::add_link(id,lt,t,d)) {
			linkset.resize(id+1);
			linkset[id]=LinkDV<State>(Links1S<State>::typeinfos[lt].max_delay+1,s,t,d);
			return true;
		}
		else	return false; // cannot be added.
	};

	// Removing a link with the given ID (not allowed here).
	bool remove_link(LinkID id, LinkType lt, TimeType t) {
		if (lt!=Links1S<State>::get_type_S()) throw Bad_Link_Type();
		return false; // not allowed
	};
	
	// Get the delay of the given link.
	DelayType get_delay(LinkID id, LinkType lt) const {
		if (lt!=Links1S<State>::get_type_S()) throw Bad_Link_Type();
		if (!if_ID(id)) throw Bad_Link_ID();	// non-existing ID. 
		return linkset[id].get_delay();
	};

	// Set the delay of the given link.
	// Return false, if failed.
	bool set_delay(LinkID id, LinkType lt, DelayType d) {
		if (lt!=Links1S<State>::get_type_S()) throw Bad_Link_Type();
		if (!Links::delay_check(lt,d)) return false;
		if (!if_ID(id)) throw Bad_Link_ID();	// non-existing ID. 
		linkset[id].set_delay(d);
		return true;
	};

	// Get the Link state of the given link.
	State get_state(LinkID id, LinkType lt, TimeType t) const {
		if (lt!=Links1S<State>::get_type_S()) throw Bad_Link_Type();
		if (!if_ID(id)) throw Bad_Link_ID();	// non-existing ID. 
		return linkset[id].access_state_T(t);
	};

	// Access the Link state of the given link.
	State & access_state(LinkID id, LinkType lt, TimeType t) {
		if (lt!=Links1S<State>::get_type_S()) throw Bad_Link_Type();
		if (!if_ID(id)) throw Bad_Link_ID();	// non-existing ID. 
		return linkset[id].access_state_T(t);
	};

	// Set the Link state of the given link using a double.
	bool set_state(LinkID id, LinkType lt, TimeType t, const State &s) {
		if (lt!=Links1S<State>::get_type_S()) throw Bad_Link_Type();
		if (!if_ID(id)) throw Bad_Link_ID();	// non-existing ID. 
		linkset[id].set_state_T(s,t);
		return true;
	};

	// Write the links info.
	void write_T(std::ostream &of, TimeType t) const {
		for (long id=0;id<linkset.size();id++)
			write_ID_T(of,id,t);
	};
	void write_ID_T(std::ostream &of, LinkID id, TimeType t) const {
		if (Links::if_ID(id)) {
			of << id << '\t' << get_delay(id,Links1S<State>::get_type_S()) << '\t';
			if (linkset[id].range(t))
				of << access_state_T(id,Links1S<State>::get_type_S(),t) << '\n';
			else
				of << "NA" << '\n';
		};
	};

   protected:
   	std::vector<LinkDV<State> > linkset;
};

// Example4:
// Subclass of Links1S: it contains one type of the VARYING number of links with inhomogeneous delays and changing state (with RNG).
template<class State> class LinkDVRSs : public Links1S<State> {
   public:
   	// Constructor.
	// There is only one type.
	LinkDVRSs(TimeType &t, DelayType md, LinkType lt=0): Links1S<State>(t,md,lt), linkset() {};

	// Destructor
	~LinkDVRSs() {};

	// Get the LinkType of the given link.
	LinkType get_type(LinkID id) const {
		if (!if_ID(id)) throw Bad_Link_ID();	// non-existing ID. 
		return Links1S<State>::get_type_S();
	};

   	// Find if the given ID already exists.
   	bool if_ID(LinkID id) const {
		return (linkset.find(id)!=linkset.end());
	};
  
   	// Deleting all links at the current time.
	void clear(LinkType lt) {
		Links::clear(lt);
		linkset.clear();
	};
	
	// Advancing the time to the given time for all current link objects, if needed.
	void advance(TimeType t) {
		if (Links1S<State>::time>t) return;
		for (typename std::map<LinkID,LinkDVR<State> >::iterator i=linkset.begin();i!=linkset.end();i++) {
			if ((i->second).get_last_time()<t) { // outside the range of the time sequence
				while ((i->second).get_last_time()<t)
					(i->second).advance_without_change();
			}
			else { 	// If the time is still inside the range of the time sequence.
				// Assumes that time advances with 1 and 
				// that the weight value is kept from the previous valule.
				(i->second).set_state_T((i->second).access_state_T(t-1),t);
			};
		};
	};
	void advance_L(LinkType lt, TimeType t) {
		if (this->get_type_S()==lt) this->advance(t);
	};

	// Adding a link with the default link state
	// ID should be new, and it will create a new link.
	bool add_link(LinkID id, LinkType lt, TimeType t, DelayType d=1) {
		if (lt!=Links1S<State>::get_type_S()) throw Bad_Link_Type(); // There is only one type.
		return this->add_link_S(id,lt,t,State(),d);
	};
	bool add_link_S(LinkID id, LinkType lt, TimeType t, const State &s, DelayType d=1) {
		if (lt!=Links1S<State>::get_type_S()) throw Bad_Link_Type(); // There is only one type.
		if (Links::add_link(id,lt,t,d)) {
			pair<typename map<LinkID,LinkDVR<State> >::iterator,bool> temp= \
				linkset.insert(typename map<LinkID,LinkDVR<State> >::value_type(id,LinkDVR<State>(Links1S<State>::typeinfos[lt].max_delay+1,s,t,d)));  
			return true;
		}
		else	return false; // cannot be added.
	};

	// Removing a link with the given ID. (will not be used by assumption)
	bool remove_link(LinkID id, LinkType lt, TimeType t) {
		if (lt!=Links1S<State>::get_type_S()) throw Bad_Link_Type();
		if (Links::remove_link(id,lt,t)) {
			linkset.erase(linkset.find(id));
			return true;
		}
		else	return false; // none existing.
	};

	// Get the delay of the given link.
	DelayType get_delay(LinkID id, LinkType lt) const {
		if (lt!=Links1S<State>::get_type_S()) throw Bad_Link_Type();
		if (!if_ID(id)) throw Bad_Link_ID();	// non-existing ID. 
		return linkset[id].get_delay();
	};

	// Set the delay of the given link. Return false, if failed.
	bool set_delay(LinkID id, LinkType lt, DelayType d) {
		if (lt!=Links1S<State>::get_type_S()) throw Bad_Link_Type();
		if (!if_ID(id)) throw Bad_Link_ID();	// non-existing ID. 
		if (!Links::delay_check(lt,d)) return false;
		linkset[id].set_delay(d);
	};
	
	// Get the Link state of the given link.
	State get_state(LinkID id, LinkType lt, TimeType t) const {
		if (lt!=Links1S<State>::get_type_S()) throw Bad_Link_Type();
		if (!if_ID(id)) throw Bad_Link_ID();	// non-existing ID. 
		return linkset[id].access_state_T(t);
	};

	// Access the Link state of the given link.
	State & access_state(LinkID id, LinkType lt, TimeType t) {
		if (lt!=Links1S<State>::get_type_S()) throw Bad_Link_Type();
		if (!if_ID(id)) throw Bad_Link_ID();	// non-existing ID. 
		return linkset[id].access_state_T(t);
	};

	// Set the Link state of the given link using a double.
	bool set_state(LinkID id, LinkType lt, TimeType t, const State &s) {
		if (lt!=Links1S<State>::get_type_S()) throw Bad_Link_Type();
		if (!if_ID(id)) throw Bad_Link_ID();	// non-existing ID. 
		linkset[id].set_state_T(s,t);
		return true;
	};

	// Write the links info.
	void write_T(std::ostream &of, TimeType t) const {
		for (typename std::map<LinkID,LinkDVR<State> >::const_iterator i=linkset.begin();i!=linkset.end();i++) {
			of << i->first << '\t' << get_delay(i->first,Links1S<State>::get_type_S()) << '\t';
			if ((i->second).range(t))
				of << (i->second).access_state_T(t) << '\n';
			else
				of << "NA" << '\n';
		};
	};
	void write_ID_T(std::ostream &of, LinkID id, TimeType t) const {
		if (if_ID(id)) {
			of << id << '\t' << get_delay(id,Links1S<State>::get_type_S()) << '\t';
			if (linkset[id].range(t))
				of << linkset[id].access_state_T(t) << '\n';
			else
				of << "NA" << '\n';
		};
	};

   protected:
   	std::map<LinkID,LinkDVR<State> > linkset;
};

}; // End of namespace conet.
#endif
