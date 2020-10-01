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

/* Portability utilities for turning the binary accounts database into
   an XML file, for loading that file into a new database */

#include "xml_dump_load.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <cstring>
#include <ctime>

#include <unistd.h>
#include <db_cxx.h>
#include <sys/dir.h>

#include "db_player.h"
#include "player.h"


void	XmlDumpLoad::DumpComputer(const ComputerRec& comp,std::ofstream& dump_file)
{
	dump_file << "        <computer level='" << comp.level;
	dump_file << "' cur_level='" << comp.cur_level;
	dump_file << "' sensors='" << comp.sensors;
	dump_file << "' jammers='" << comp.jammers << "'/>\n";
}

void	XmlDumpLoad::DumpJob(DbJob& job,std::ofstream& dump_file)
{
	dump_file << "      <job commod='" << job.commod << "'";
	dump_file << "      from='" << job.from << "'";
	dump_file << "      to='" << job.to << "'\n";
	dump_file << "        quantity='" << job.quantity << "'";
	dump_file << "      time_available='" << job.time_available << "'";
	dump_file << "      time_taken='" << job.time_taken << "'\n";
	dump_file << "        payment='" << job.payment << "'";
	dump_file << "      credits='" << job.credits << "'/>\n";
}

void	XmlDumpLoad::DumpLoc(DBLocRec& loc,std::ofstream& dump_file)
{
	dump_file << "      <location star_name='" << loc.star_name;
	dump_file << "' map_name='" << loc.map_name;
	dump_file << "' loc_no='" << loc.loc_no << "'/>\n";
}

bool XmlDumpLoad::DumpOneAccount(Player *player,std::ofstream& dump_file)
{
	// Don't save groundhogs who have not been on for over 3 months
	static time_t	three_months_ago = 0;
	if (three_months_ago == 0)
	{
		time_t	now = std::time(0);
		three_months_ago = now - 3 * 30 * 24 * 60 * 60;
	}
	if((player->Rank() == 0) && (player->LastTimeOn() < three_months_ago))
		return false;

	DBPlayer	*rec = player->CreateDBRec();
	dump_file << "  <player\n";
	dump_file << "    name='" << rec->name << "' ";
	dump_file << "    ib_account='" << rec->ib_account << "' ";
	dump_file << "    email='" << rec->email << "'\n";

	dump_file << "    password='";
	dump_file.setf(std::ios::uppercase);
	for(int count = 0;count < Player::MAX_PASSWD;++count)
	{
		unsigned int	temp = static_cast<unsigned char>(rec->password[count]);
		dump_file << std::hex << std::setw(2) << std::setfill('0') << temp << " ";
	}
	dump_file << "'\n";
	dump_file.unsetf(std::ios::uppercase);
	dump_file << std::dec;

	dump_file << "    desc='" << rec->desc << "'\n";
	dump_file << "    race='" << rec->race << "' ";
	dump_file << "    gender='" << rec->gender << "'\n";

	dump_file << "    strength0='" << rec->strength[0] << "' ";
	dump_file << "    strength1='" << rec->strength[1] << "' ";
	dump_file << "    stamina0='" << rec->stamina[0] << "' ";
	dump_file << "    stamina1='" << rec->stamina[1] << "'\n";
	dump_file << "    dexterity0='" << rec->dexterity[0] << "' ";
	dump_file << "    dexterity1='" << rec->dexterity[1] << "' ";
	dump_file << "    intelligence0='" << rec->intelligence[0] << "' ";
	dump_file << "    intelligence1='" << rec->intelligence[1] << "'\n";

	dump_file << "    rank='" << rec->rank << "' ";
	dump_file << "    cash='" << rec->cash << "' ";
	dump_file << "    loan='" << rec->loan << "' ";
	dump_file << "    trader_pts='" << rec->trader_pts << "'\n";
	dump_file << "    courier_pts='" << rec->courier_pts << "' ";
	dump_file << "    reward='" << rec->reward << "' ";
	dump_file << "    games='" << rec->games << "' ";
	dump_file << "    killed='" << rec->killed << "'\n";

	dump_file << "    inv_flags='" << rec->inv_flags << "' ";
	dump_file << "    flags='" << rec->flags << "' ";
	dump_file << "    man_flags='" << rec->man_flags << "'\n";
	dump_file << "    status_flags='" << rec->status_flags << "' ";
	dump_file << "    ship_flags='" << rec->ship_flags << "' ";
	dump_file << "    medallions='" << rec->medallions << "'\n";

	dump_file << "    customs_cert='" << rec->customs_cert << "' ";
	dump_file << "    keyring='" << rec->keyring << "' ";
	dump_file << "    price_check_sub='" << rec->price_check_sub << "'\n";
	dump_file << "    wearing='" << rec->wearing << "' ";
	dump_file << "    slithy_xform='" << rec->slithy_xform << "'\n";

	for(int count = 0;count < Player::MAX_COUNTERS;++count)
		dump_file << "    counters" << count << "='" << rec->counters[count] << "' ";
	dump_file << "\n";

	dump_file << "    last_on='" << rec->last_on << "' ";
	dump_file << "    last_payment='" << rec->last_payment << "' ";
	dump_file << "    ip_address='" << rec->ip_address << "' ";
	dump_file << "    line_length='" << rec->line_length << "'\n";
	dump_file << "    start='" << rec->start << "' ";
	dump_file << "    gifts='" << rec->gifts << "' ";
	dump_file << "    spouse='" << rec->spouse << "' ";
	dump_file << "    tp_rental='" << rec->spouse << "'>\n";

	DumpLoc(rec->loc,dump_file);
	DumpShip(rec->ship,dump_file);
	if(rec->rank == Player::ADVENTURER)
		DumpTask(rec->task,dump_file);
	else
		DumpJob(rec->job,dump_file);

	dump_file << "  </player>\n\n";

	delete rec;
	return true;
}

void	XmlDumpLoad::DumpShip(DbShip& ship,std::ofstream& dump_file)
{
	dump_file << "      <ship registry='" << ship.registry << "'";
	dump_file << "      class='" << ship.ship_class << "'\n";
	dump_file << "        max_hull='" << ship.max_hull << "'";
	dump_file << "      cur_hull='" << ship.cur_hull << "'";
	dump_file << "      max_shield='" << ship.max_shield << "'";
	dump_file << "      cur_shield='" << ship.cur_shield << "'\n";
	dump_file << "        max_engine='" << ship.max_engine << "'";
	dump_file << "      cur_engine='" << ship.cur_engine << "'";
	dump_file << "      max_fuel='" << ship.max_fuel << "'";
	dump_file << "      cur_fuel='" << ship.cur_fuel << "'\n";
	dump_file << "        max_hold='" << ship.max_hold << "'";
	dump_file << "      cur_hold='" << ship.cur_hold << "'";
	dump_file << "      magazine='" << ship.magazine << "'";
	dump_file << "      missiles='" << ship.missiles << "'>\n";

	DumpComputer(ship.computer,dump_file);
	DumpWeapons(ship.weapons,dump_file);

	dump_file << "      </ship>\n";
}

void	XmlDumpLoad::DumpTask(DbTask& task,std::ofstream& dump_file)
{
	dump_file << "      <task pickup_map='" << task.pickup_map << "'";
	dump_file << "      pickup_loc='" << task.pickup_loc << "'\n";
	dump_file << "        delivery_map='" << task.delivery_map << "'";
	dump_file << "      delivery_loc='" << task.delivery_loc << "'\n";
	dump_file << "        pkg_index='" << task.pkg_index << "'";
	dump_file << "      payment='" << task.payment << "'";
	dump_file << "      collected='" << (task.collected ? "true" : "false") << "'/>\n";
}

void	XmlDumpLoad::DumpWeapons(const WeaponRec *rec,std::ofstream& dump_file)
{
	for(int count = 0;count < Ship::MAX_HARD_PT;++count)
		dump_file << "        <weapon type='" << rec[count].type << "' efficiency='" << rec[count].efficiency << "'/>\n";
}

