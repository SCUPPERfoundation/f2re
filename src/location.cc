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

#include "location.h"

#include <iostream>
#include <sstream>

#include "event_number.h"
#include "fedmap.h"
#include "misc.h"
#include "msg_number.h"
#include "output_filter.h"
#include "player.h"

Location::Location()
{
	loc_no = INVALID_LOC;
	name = "";
	desc = "";
	for(int count = 0;count < MAX_EXITS;count++)
		exits[count] = NO_EXIT;
	no_exit = 0;
	for(int count = 0;count < MAX_EVENTS;count++)
		events[count] = 0;
	cabinet = 0;
}

Location::Location(int loc_num)
{
	loc_no = loc_num;
	name = "";
	desc = "";
	for(int count = 0;count < MAX_EXITS;count++)
		exits[count] = NO_EXIT;
	no_exit = 0;
	for(int count = 0;count < MAX_EVENTS;count++)
		events[count] = 0;
	cabinet = 0;
}

Location::~Location()
{
	delete no_exit;
	for(int count = 0;count < MAX_EVENTS;count++)
		delete events[count];
}


void	Location::AddDesc(const std::string& text,int how)
{
	if(how == REPLACE_DESC)
		desc = "\n" + text;
	else
	{
		if(text.length() < 2)
			desc += text;
		else
		{
			desc += "\n";
			desc += text;
		}
	}
}

void	Location::AddFlags(std::string& text)
{
	std::string::iterator	iter;
	for(iter = text.begin();iter != text.end();iter++)
	{
		switch(*iter)
		{
			case 'a':	flags.set(PICKUP);	break;
			case 'b':	flags.set(BAR);		break;
			case 'c':	flags.set(COURIER);	break;
			case 'e':	flags.set(EXCHANGE);	break;
			case 'h':	flags.set(HOSPITAL);	break;
			case 'i':	flags.set(INSURE);	break;
			case 'k':	flags.set(CUSTOM);	break;
			case 'l':	flags.set(LINK);		break;
			case 'p':	flags.set(PEACE);		break;
			case 'r':	flags.set(REPAIR);	break;
			case 's':	flags.set(SPACE);		break;
			case 't':	flags.set(TELEPORT);	break;
			case 'u':	flags.set(UNLIT);		break;
			case 'y':	flags.set(SHIPYARD);	break;
			case 'w':	flags.set(WEAPONS);	break;
			case 'f':	flags.set(FIGHTING);	break;
		}
	}
}

const std::string&	Location::AsciiOnly(const std::string& text)
{
	static std::string	ascii_text;
	std::ostringstream	buffer;
	int length = text.length();

	for (int count = 0;count < length;count++)
	{
		if(isascii(text[count]) == 0)
			buffer << "#";
		else
			buffer << text[count];
	}
	ascii_text = buffer.str();
	return(ascii_text);
}

void	Location::Description(std::ostringstream& buffer,int extent)
{
	buffer << name;
	if(FlagIsSet(FIGHTING))
		buffer << " (Not patrolled)";
	if(extent == FULL_DESC)
		buffer << desc;
	buffer << "\n";
}

void	Location::Description(Player *player,int extent)
{
	if(player->CommsAPILevel() > 0)
	{
		XMLNewLoc(player,extent);
		return;
	}

	std::ostringstream	buffer("");
	buffer << name;
	if(FlagIsSet(FIGHTING))
		buffer << " (Not patrolled)";
	if(extent == DEFAULT)
	{
		if(!player->WantsBrief())
			buffer << desc;
	}
	else
	{
		if(extent == FULL_DESC)
			buffer << desc;
	}
	buffer << std::endl;
	player->Send(buffer);
}

void	Location::Dump()
{
	std::cout << "Location number: " << loc_no << std::endl;
	std::cout << "  Name: '" << name << "'" << std::endl;
	std::cout << "  Desc: '" << desc << "'" << std::endl;
	std::cout << "  Exits: ";
	for(int count = 0;count < MAX_EXITS;count++)
		std::cout << exits[count] << "  ";
	std::cout << std::endl;
}

LocRec	*Location::Move(Player *player,FedMap *home_map,int dir)
{
	static LocRec	rec;

	if(exits[dir] == NO_EXIT)
	{
		if(events[NOEXIT] != 0)
		{
			events[NOEXIT]->Process(player);
			return(0);					// event will handle any updating player
		}

		if(no_exit == 0)
		{
			if(player->IsInSpace())
				player->Send(Game::system->GetMessage("location","move",2));
			else
				player->Send(Game::system->GetMessage("location","move",1));
		}
		else
			player->Send(no_exit->Find(home_map));
		return(0);
	}

	rec.fed_map = 0;
	rec.loc_no = exits[dir];
	return(&rec);
}

bool	Location::ProcessVocab(Player *player,const std::string& command)
{
	LocVocab::iterator iter = vocab.find(command);
	if(iter != vocab.end())
	{
		iter->second->Process(player);
		return(true);
	}
	else
		return(false);
}

void	Location::Write(std::ofstream& file)
{
	file << "   <location num='" << loc_no << "'";
	WriteFlags(file);
	file << ">\n";
	file << "      <name>" << EscapeXML(AsciiOnly(name)) << "</name>\n";
	WriteDesc(file);
	WriteExits(file);
	WriteEvents(file);
	WriteVocab(file);
	file << "   </location>\n\n";
}

void	Location::WriteDesc(std::ofstream& file)
{
	std::string::size_type	index = desc.find('\n');
	std::string::size_type	last = 0;
	while(index != std::string::npos)
	{
		if((index - last) > 1)
			file << "      <desc>" << EscapeXML(AsciiOnly(desc.substr(last,index - last))) << "</desc>\n";
		if((last = ++index) != std::string::npos)
			index = desc.find('\n',last);
	}
	if((last + 1) != std::string::npos)
		file << "      <desc>" << EscapeXML(AsciiOnly(desc.substr(last))) << "</desc>\n";
}

void	Location::WriteEvents(std::ofstream& file)
{
	static const std::string	triggers[] = { "enter", "no-exit", "in-room", "search", ""	};

	bool	has_events = false;
	for(int count = 0;count < MAX_EVENTS;count++)
	{
		if(events[count] != 0)
		{
			has_events = true;
			break;
		}
	}

	if(!has_events)
		return;

	file << "      <events";
	for(int count = 0;count < MAX_EVENTS;count++)
	{
		if(events[count] != 0)
			file << " " << triggers[count] << "='" << events[count]->Display() << "'";
	}
	file << "/>\n";
}

void	Location::WriteExits(std::ofstream& file)
{
	const std::string directions[] =
		{ "n", "ne", "e", "se", "s", "sw", "w", "nw", "up", "down", "in", "out", "" };

	bool	has_exits = false;
	for(int count  = 0;count < MAX_EXITS;count++)
	{
		if(exits[count] != NO_EXIT)
		{
			has_exits = true;
			break;
		}
	}
	if(!has_exits && (no_exit == 0))
		return;

	file << "      <exits";
	for(int count  = 0;count < MAX_EXITS;count++)
	{
		if(exits[count] != NO_EXIT)
			file << " " << directions[count] << "='" << exits[count] << "'";
	}

	if(no_exit != 0)
		file << " no-exit='" << no_exit->Display() << "'";

	file << "/>\n";
}

void	Location::WriteFlags(std::ofstream& file)
{
	if(flags.none())
		return;

	file << " flags='";
	if(flags.test(SPACE))		file << "s";
	if(flags.test(LINK))			file << "l";
	if(flags.test(EXCHANGE))	file << "e";
	if(flags.test(SHIPYARD))	file << "y";
	if(flags.test(REPAIR))		file << "r";
	if(flags.test(HOSPITAL))	file << "h";
	if(flags.test(INSURE))		file << "i";
	if(flags.test(PEACE))		file << "p";
	if(flags.test(BAR))			file << "b";
	if(flags.test(UNLIT))		file << "u";
	if(flags.test(CUSTOM))		file << "k";
	if(flags.test(TELEPORT))	file << "t";
	if(flags.test(COURIER))		file << "c";
	if(flags.test(PICKUP))		file << "a";
	if(flags.test(WEAPONS))		file << "w";
	if(flags.test(FIGHTING))	file << "f";
	file << "'";
}

void	Location::WriteVocab(std::ofstream& file)
{
	if(vocab.size() > 0)
	{
		for(LocVocab::iterator iter = vocab.begin();iter != vocab.end();iter++)
			file << "      <vocab cmd='" << iter->first << "' event='" << iter->second->Display() << "'/>\n";
	}
}

void	Location::XMLFlags(std::ostringstream& buffer)
{
	if(flags.test(SPACE))		buffer << "s";
	if(flags.test(LINK))			buffer << "l";
	if(flags.test(PEACE))		buffer << "p";
	// Add other flags as needed
}

void	Location::XMLNewLoc(Player *player,int extent)
{
	static const std::string exit_names[] =
		{ "n", "ne", "e", "se", "s", "sw", "w", "nw", "u", "d", ""	};

	std::ostringstream	buffer;
	AttribList attribs;
	buffer << loc_no;
	attribs.push_back(std::make_pair("loc-num",buffer.str()));
	buffer.str("");
	buffer << name;
	if(FlagIsSet(FIGHTING))
		buffer << " (Not patrolled)";
	attribs.push_back(std::make_pair("name",buffer.str()));
	buffer.str("");
	XMLFlags(buffer);
	if(buffer.str().size() > 0)
		attribs.push_back(std::make_pair("flags",buffer.str()));
	for(int count = 0;exit_names[count] != "";count++)
	{
		if(exits[count] >= 0)
		{
			buffer.str("");
			buffer << exits[count];
			attribs.push_back(std::make_pair(exit_names[count],buffer.str()));
		}
	}

	std::string	text;
	if(((extent == DEFAULT) && !player->WantsBrief()) || (extent == FULL_DESC))
		text = desc.substr(1);

	player->Send(text,OutputFilter::NEW_LOC,attribs);
}


