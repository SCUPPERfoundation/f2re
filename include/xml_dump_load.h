/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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
