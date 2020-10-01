/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include <fstream>
#include <iostream>
#include <sstream>

#include <cstdlib>
#include <cstring>
#include <ctime>

#include <unistd.h>
#include <db_cxx.h>
#include <sys/dir.h>

#include "billing.h"


void	ProcessFile(char* name);
void	ProcessRecords(Db *db,char *name);	// Db is a Berkeley DB class (db_cxx.h)
void	WriteAscii(DBAccount& rec,std::ofstream& file);

int main(int argc,char* argv[])
{
	ProcessFile(argv[1]);
}


void ProcessFile(char* name)
{
	char	src_name[32];
	std::strncpy(src_name,name,26);
	src_name[26] = '\0';
	std::strcat(src_name,".dat");

	Db *db = new Db(0,DB_CXX_NO_EXCEPTIONS);
	int ret_val = db->open(0,src_name,0,DB_BTREE,DB_CREATE,0);
	if(ret_val < 0)
	{
		std::cout << "Unable to open player database file '" << src_name << "'" << std::endl;
		return;
	}

	ProcessRecords(db,name);

	db->sync(0);
	db->close(0);
	delete db;
}

void	ProcessRecords(Db *db,char *name)
{
	Dbc	*dbc;
	db->cursor(0,&dbc,0);
	Dbt	*key = new Dbt();
	Dbt	*data = new Dbt;
	DBAccount	rec;

	char	dest_name[32];
	std::strncpy(dest_name,name,26);
	dest_name[26] = '\0';
	std::strcat(dest_name,".asc");
	std::ofstream	file(dest_name);
	if(!file)
	{
		std::cerr << "Can't open output file '" << dest_name << "'" << std::endl;
		std::exit(1);
	}
	
	int total = 0;
	while(dbc->get(key,data,DB_NEXT) != DB_NOTFOUND)
	{
		std::memcpy(&rec,data->get_data(),data->get_size());
		if(rec.name[0] == '\0')
			continue;
		else
		{
			++total;
			WriteAscii(rec,file);
		}
	}
	std::cout << total << " records processed\n";

	delete key;
	delete data;
	dbc->close();
}

void	WriteAscii(DBAccount& rec,std::ofstream& file)
{
	std::cout << rec.name << "\n";

	file << "[**START**]\n";
	file << rec.name << "\n";
	file << "********" << "\n";
	file << rec.status << "\n";
	file << rec.created << "\n";

	file << rec.terminated_d << "\n";
	file << rec.terminated_m << "\n";
	file << rec.terminated_y << "\n";

	file << rec.been_billed << "\n";
	
	file << rec.current_on << "\n";
	file << rec.last_on << "\n";
	file << rec.current_game << "\n";
	file << rec.last_game << "\n";

	file << "[**END**]\n";
	file << std::endl;
}
