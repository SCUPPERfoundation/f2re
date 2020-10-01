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

#ifndef XML_DUMP_LOAD_H
#define XML_DUMP_LOAD_H

#include <fstream>

#include "xml_parser.h"

class Player;

struct	ComputerRec;
struct	DbJob;
struct	DBLocRec;
struct	DBPlayer;
struct	DbShip;
struct	DbTask;
struct	WeaponRec;

class XmlDumpLoad : public XMLParser
{
private:


	void	DumpComputer(const ComputerRec& loc,std::ofstream&	dump_file);
	void	DumpJob(DbJob& job,std::ofstream&	dump_file);
	void	DumpLoc(DBLocRec& comp,std::ofstream&	dump_file);
	void	DumpShip(DbShip& ship,std::ofstream& dump_file);
	void	DumpTask(DbTask& task,std::ofstream& dump_file);
	void	DumpWeapons(const WeaponRec *rec,std::ofstream& dump_file);

	void	StartElement(const char *element,const char **attrib)	{	}
	void	EndElement(const char *element)								{	}
	void	TextData(const char *text,int textlen)						{	}

public:
	XmlDumpLoad()	{	}
	~XmlDumpLoad()	{	}

	bool	DumpOneAccount(Player *player,std::ofstream&	dump_file);
};

#endif // ASCII_DUMP_LOAD_H
