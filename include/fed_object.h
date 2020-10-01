/*-----------------------------------------------------------------------
                          Federation 2
              Copyright (c) 1985-2018 Alan Lenton

This program is free software: you can redistribute it and /or modify 
it under the terms of the GNU General Public License as published by 
the Free Software Foundation: either version 2 of the License, or (at 
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY: without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
General Public License for more details.

You can find the full text of the GNU General Public Licence at
           http://www.gnu.org/copyleft/gpl.html

Programming and design:     Alan Lenton (email: alan@ibgames.com)
Home website:                   www.ibgames.net/alan
-----------------------------------------------------------------------*/

#ifndef FEDOBJECT_H
#define FEDOBJECT_H

#include <bitset>
#include <map>
#include <string>

#include "db_object.h"
#include "loc_rec.h"	// note we only use the loc_no and fed_map fields

class	EventNumber;
class FedMap;
class Player;

typedef	std::map<const std::string,EventNumber *,std::less<std::string> >	  ObjVocab;

class	FedObject	// virtual base class for objects
{
public:
	static const std::string	article_names[];
	static const std::string	vowel_article_names[];
	static const std::string	plural_article_names[];

	enum	// where the object is located, if it's not on the map
	{
		IN_INVENTORY = -1, IN_SHIP = -5, IN_DISPLAY_CAB = -6, ALL_IN_DATABASE = -7
	};

	enum	{ NO_ART, DEFINITE, INDEFINITE, PLURAL };		// none/definate/indefinate article
	enum	{ UPPER_CASE, LOWER_CASE };
	enum	{ STORABLE, CLIPPED, WORN, CARRIED, NO_TP, PRIVATE, MAX_FLAGS };	//	flags

protected:
	std::string	name;					// object name (non-unique)
	std::string	lc_name;				// lower case version of name
	std::string	id;					// object ID (unique to this map)
	std::string	desc;					// object description
	int			size;					// useful general purpose variable
	int			article;				// none/definate/indefinate article
	LocRec		home;					//	object's home map and start loc
	LocRec		loc;					//	object's current map and loc (loc = IN_INVENTORY carried by player)
	ObjVocab		vocab;				// vocab specific to this object

	std::bitset<MAX_FLAGS> 	flags;	// flags for objects

public:
	FedObject();
	FedObject(FedMap *home_map,int start_loc);
	virtual	~FedObject();

	FedMap	*HomeMap()													{ return(home.fed_map);	}

	const LocRec	*HomeLocRec() const								{ return(&home);			}

	const std::string&	c_str(int type_case = LOWER_CASE);
	const std::string&	Desc()										{ return(desc);			}
	const std::string&	ID()											{ return(id);				}
	const std::string&	Name()										{ return(name);			}
	const std::string&	LcName()										{ return(lc_name);		}

	int	ChangeSize(int amount,int max_size,bool add);
	int	Size() const													{ return(size);			}
	int	Where()	const													{ return(loc.loc_no);	}

	bool	IsCarried()	const							{ return(flags.test(CARRIED));	}
	bool	IsClipped()	const							{ return(flags.test(CLIPPED));	}
	bool	IsInLoc(int loc_no)						{ return(loc_no == loc.loc_no);	}
	bool	IsHomeMap(const std::string& star_name,const std::string& map_name);
	bool	IsPlural()									{ return(article == PLURAL);		}
	bool	IsPrivate()									{ return(flags.test(PRIVATE));	}
	bool	IsStorable() const						{ return(flags.test(STORABLE));	}
	bool	IsWorn()	const								{ return(flags.test(WORN));		}
	bool	NoTeleport() const						{ return(flags.test(NO_TP));		}
	bool	ProcessVocab(Player *player,const std::string& command,const std::string& text);

	void	Desc(const std::string& the_desc)						{ desc = the_desc + "\n";	}
	void	ClearInvFlags();
	void	ID(const std::string& the_id)								{ id = the_id;				}
	void	Location(int where)											{ loc.loc_no = where;	}
	void	Location(const LocRec& rec);
	void	Name(const std::string& the_name);
	void	ResetFlag(int which_flag)									{ flags.reset(which_flag);	}
	void	SetFlag(int which_flag)										{ flags.set(which_flag);	}
	void	Vocab(const std::string& key,EventNumber *ev_num)	{ vocab[key] = ev_num;	}

	virtual bool	IsVisible(Player *player)	{ return(true);	}
	virtual bool	IsAbstract()					{ return(true);	}

	virtual int		Weight()							{ return(-1);		}
	virtual void	Loc2Inventory()				{	}

	virtual FedObject	*Create(FedMap *dest_map,int dest_loc,Player *player,int sz = -1,int vis = -1,bool db_load = false) = 0;
	virtual void		Dump() = 0;
	virtual void		Move(FedMap *fed_map) = 0;
};

#endif

