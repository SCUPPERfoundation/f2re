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

#include <cctype>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <unistd.h>
#include <db_cxx.h>
#include <sys/dir.h>

#include "avatar.h"

void	ProcessFile(char* name);
void	ProcessRecords(Db *db,std::ifstream& file);	// Db is a Berkeley DB class (db_cxx.h)
void	WriteDb(DBPlayer *rec,Db *db);

int main(int argc,char* argv[])
{
	ProcessFile(argv[1]);
}


void	ProcessFile(char* name)
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

	char	dest_name[32];
	std::strncpy(dest_name,name,26);
	dest_name[26] = '\0';
	std::strcat(dest_name,".asc");
	std::ifstream	file(dest_name);
	if(!file)
	{
		std::cerr << "Can't open input file '" << dest_name << "'" << std::endl;
		return;
	}
	
	ProcessRecords(db,file);

	db->sync(0);
	db->close(0);
	delete db;
}

void	ProcessRecords(Db *db,std::ifstream& file)
{
	struct	DBPlayer	rec;
	int	total = 0;
	std::string	buffer;
	for(;;)
	{
		std::getline(file,buffer);
		if(file.eof())
			break;
		std::strcpy(rec.name,buffer.c_str());
		std::getline(file,buffer);
		std::strcpy(rec.ib_account,buffer.c_str());
		std::getline(file,buffer);
		std::strcpy(rec.desc,buffer.c_str());
		std::getline(file,buffer);
		std::strcpy(rec.race,buffer.c_str());
		std::getline(file,buffer);
		rec.gender = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.strength[0] = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.strength[1] = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.stamina[0] = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.stamina[1] = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.dexterity[0] = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.dexterity[1] = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.intelligence[0] = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.intelligence[1] = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.rank = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.cash = std::atol(buffer.c_str());
		std::getline(file,buffer);
		rec.loan = std::atol(buffer.c_str());
		std::getline(file,buffer);
		rec.trader_pts = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.courier_pts = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.reward = std::atol(buffer.c_str());
		std::getline(file,buffer);
		rec.games = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.killed = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.inv_flags = static_cast<unsigned long>(std::atol(buffer.c_str()));
		std::getline(file,buffer);
		rec.flags = static_cast<unsigned long>(std::atol(buffer.c_str()));
		std::getline(file,buffer);
		rec.man_flags = static_cast<unsigned long>(std::atol(buffer.c_str()));
		std::getline(file,buffer);
		rec.status_flags = static_cast<unsigned long>(std::atol(buffer.c_str()));
		std::getline(file,buffer);
		rec.ship_flags = static_cast<unsigned long>(std::atol(buffer.c_str()));
		std::getline(file,buffer);
		rec.medallions = static_cast<unsigned long>(std::atol(buffer.c_str()));
		std::getline(file,buffer);
		rec.customs_cert = static_cast<time_t>(std::atoi(buffer.c_str()));
		std::getline(file,buffer);
		rec.keyring = static_cast<unsigned long>(std::atol(buffer.c_str()));
		std::getline(file,buffer);
		rec.price_check_sub = static_cast<time_t>(std::atoi(buffer.c_str()));
		std::getline(file,buffer);
		rec.wearing = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.slithy_xform = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.counters[0] = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.counters[1] = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.last_on = static_cast<time_t>(std::atoi(buffer.c_str()));
		std::getline(file,buffer);
		rec.last_payment = static_cast<time_t>(std::atoi(buffer.c_str()));
		std::getline(file,buffer);
		std::strcpy(rec.ip_address,buffer.c_str());
		std::getline(file,buffer);
		rec.line_length = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		std::strcpy(rec.loc.star_name,buffer.c_str());
		std::getline(file,buffer);
		std::strcpy(rec.loc.map_name,buffer.c_str());
		std::getline(file,buffer);
		rec.loc.loc_no = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		std::strcpy(rec.ship.registry,buffer.c_str());
		std::getline(file,buffer);
		rec.ship.ship_class = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.ship.max_hull = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.ship.cur_hull = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.ship.max_shield = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.ship.cur_shield = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.ship.max_engine = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.ship.cur_engine = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.ship.computer.level = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.ship.computer.cur_level = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.ship.computer.sensors = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.ship.computer.jammers = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.ship.max_fuel = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.ship.cur_fuel = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.ship.max_hold = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.ship.cur_hold = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.ship.magazine = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.ship.missiles = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.ship.weapons[0].type = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.ship.weapons[0].damage = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.ship.weapons[1].type = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.ship.weapons[1].damage = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.ship.weapons[2].type = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.ship.weapons[2].damage = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.ship.weapons[3].type = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		rec.ship.weapons[3].damage = std::atoi(buffer.c_str());
		std::getline(file,buffer);

		// Job/Work union
		if(rec.rank ==3)
			rec.task.pkg_index = -1;
		else
			rec.job.quantity = 0;
		
		rec.start = static_cast<time_t>(std::atoi(buffer.c_str()));
		std::getline(file,buffer);
		rec.gifts = std::atoi(buffer.c_str());
		std::getline(file,buffer);
		std::strcpy(rec.spouse,buffer.c_str());
		std::getline(file,buffer);
		rec.tp_rental = static_cast<time_t>(std::atoi(buffer.c_str()));

		WriteDb(&rec,db);
		
		std::cout << rec.name << "\n";
		++total;
	}
	std::cout << total << " record written\n";
}

void	WriteDb(DBPlayer *rec,Db *db)
{
	Dbt	*key = new Dbt(rec->ib_account,24);
	Dbt	*data = new Dbt(rec,sizeof(DBPlayer));

	int	ret_val = db->put(0,key,data,0);
	if(ret_val != 0)
		std::cerr << "***ERROR: Can't save player " << rec->name << " to database!" << std::endl;

	db->sync(0);
	delete data;
	delete key;
}

