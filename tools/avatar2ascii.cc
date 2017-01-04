/*----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
----------------------------------------------------------------------*/

#include <fstream>
#include <iostream>
#include <sstream>

#include <cstdlib>
#include <cstring>
#include <ctime>

#include <unistd.h>
#include <db_cxx.h>
#include <sys/dir.h>

#include "avatar.h"

void	ProcessFile(char* name);
void	ProcessRecords(Db *db,char *name);	// Db is a Berkeley DB class (db_cxx.h)
void	WriteAscii(DBPlayer& rec,std::ofstream& file);

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
	DBPlayer	rec;

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

void	WriteAscii(DBPlayer& rec,std::ofstream& file)
{
	std::cout << rec.name << "\n";

	file << rec.name << "\n";
	file << rec.ib_account << "\n";
	file << rec.desc << "\n";
	file << rec.race << "\n";
	file << rec.gender << "\n";
	file << rec.strength[0] << "\n";
	file << rec.strength[1] << "\n";
	file << rec.stamina[0] << "\n";
	file << rec.stamina[1] << "\n";
	file << rec.dexterity[0] << "\n";
	file << rec.dexterity[1] << "\n";
	file << rec.intelligence[0] << "\n";
	file << rec.intelligence[1] << "\n";
	file << rec.rank << "\n";
	file << rec.cash <<"\n";
	file << rec.loan << "\n";
	file << rec.trader_pts << "\n";
	file << rec.courier_pts << "\n";
	file << rec.reward << "\n";
	file << rec.games << "\n";
	file << rec.killed << "\n";
	file << rec.inv_flags << "\n";
	file << rec.flags << "\n";
	file << rec.man_flags << "\n";
	file << rec.status_flags << "\n";
	file << rec.ship_flags << "\n";
	file << rec.medallions << "\n";
	file << rec.customs_cert << "\n";
	file << rec.keyring << "\n";
	file << rec.price_check_sub << "\n";
	file << rec.wearing << "\n";
	file << rec.slithy_xform << "\n";
	file << rec.counters[0] << "\n";
	file << rec.counters[1] << "\n";
	file << rec.last_on << "\n";
	file << rec.last_payment << "\n";
	file << rec.ip_address << "\n";
	file << rec.line_length << "\n";
	file << rec.loc.star_name << "\n";
	file << rec.loc.map_name << "\n";
	file << rec.loc.loc_no << "\n";
	file << rec.ship.registry << "\n";
	file << rec.ship.ship_class << "\n";
	file << rec.ship.max_hull << "\n";
	file << rec.ship.cur_hull << "\n";
	file << rec.ship.max_shield << "\n";
	file << rec.ship.cur_shield << "\n";
	file << rec.ship.max_engine << "\n";
	file << rec.ship.cur_engine << "\n";
	file << rec.ship.computer.level << "\n";
	file << rec.ship.computer.cur_level << "\n";
	file << rec.ship.computer.sensors << "\n";
	file << rec.ship.computer.jammers << "\n";
	file << rec.ship.max_fuel << "\n";
	file << rec.ship.cur_fuel << "\n";
	file << rec.ship.max_hold << "\n";
	file << rec.ship.cur_hold << "\n";
	file << rec.ship.magazine << "\n";
	file << rec.ship.missiles << "\n";
	file << rec.ship.weapons[0].type << "\n";
	file << rec.ship.weapons[0].damage << "\n";
	file << rec.ship.weapons[1].type << "\n";
	file << rec.ship.weapons[1].damage << "\n";
	file << rec.ship.weapons[2].type << "\n";
	file << rec.ship.weapons[2].damage << "\n";
	file << rec.ship.weapons[3].type << "\n";
	file << rec.ship.weapons[3].damage << "\n";
	file << rec.start << "\n";
	file << rec.gifts << "\n";
	file << rec.spouse << "\n";
	file << rec.tp_rental << "\n";
}
