/*----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-20179
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
----------------------------------------------------------------------*/

#include "work.h"

#include <algorithm>
#include <iostream>
#include <sstream>

#include <cstring>

#include "cartel.h"
#include "commodities.h"
#include "fedmap.h"
#include "galaxy.h"
#include "loc_rec.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "ship.h"
#include "star.h"

const int		Work::NO_JOB = 0;
const size_t	Work::MIN_JOBS = 10;
const int		Work::NEW_JOBS = 20;
const int		Work::FIVE_MIN = 5 * 60;

Work::~Work()
{
	for(JobIndex::iterator iter = job_index.begin();iter != job_index.end();iter++)
		delete iter->second;
}


void	Work::Accept(Player *player,int job_no)
{
	JobIndex::iterator iter = job_index.find(job_no);
	if(iter == job_index.end())
		player->Send(Game::system->GetMessage("work","accept",1));
	else
	{
		iter->second->collected = false;
		player->AddJob(iter->second);
		std::ostringstream	buffer("");
		buffer << "Your bid is accepted. Please report to the offices of Armstrong Cuthbert, Inc on ";
		buffer << iter->second->from << " to arrange collection. Thank you." << std::endl;
		player->Send(buffer);
		job_index.erase(iter);
	}
}

void	Work::AddPlayer(Player *player)
{
	NotifyList::iterator iter = std::find(notify_list.begin(),notify_list.end(),player);
	if(iter == notify_list.end())
		notify_list.push_back(player);
}

int	Work::CalcPayment(Job *job,int multiplier)
{
	job->payment = (rand() % 5) + 8;
	if(multiplier > 1)
	{
		if(multiplier < 5)
			job->payment = (job->payment * 3)/4;
		else
		{
			if(multiplier < 8)
				job->payment = (job->payment * 2)/3;
			else
				job->payment = (job->payment * 3)/5;
		}
	}
	return(job->payment);
}

int	Work::CalcTime(Job *job)
{
	if((rand() % 100) < 4)
		job->time_available = (rand() % 6)	+ 9;
	else
		job->time_available = (rand() % 4)	+ 6;
	return(job->time_available);
}

void	Work::CreateDbJob(DbJob *db_job,Job *job)
{
	if(job == 0)
		db_job->quantity = NO_JOB;
	else
	{
		std::strcpy(db_job->commod,job->commod.c_str());
		std::strcpy(db_job->from,job->from.c_str());
		std::strcpy(db_job->to,job->to.c_str());
		db_job->quantity = job->quantity;
		db_job->time_available = job->time_available;
		if(!job->collected)
			db_job->time_available *= -1;
		db_job->time_taken = job->time_taken;
		db_job->payment = job->payment;
		db_job->credits = job->credits;
	}
}

Job	*Work::CreateJob(DbJob *db_job)
{
	if(db_job->quantity == NO_JOB)
		return(0);
	else
	{
		Job	*job = new Job;
		job->commod = db_job->commod;
		job->from = db_job->from;
		job->to = db_job->to;
		job->quantity = db_job->quantity;
		if(db_job->time_available < 0)
		{
			job->collected = false;
			db_job->time_available *= -1;
		}
		else
			job->collected = true;
		job->time_available = db_job->time_available;
		job->time_taken = db_job->time_taken;
		job->payment = db_job->payment;
		job->credits = db_job->credits;
		job->created = 0;
		return(job);
	}
}

void	Work::CreateJobs()
{
	ExpireJobs();
	if(job_index.size() < MIN_JOBS)
	{
		for(NotifyList::iterator iter = notify_list.begin();iter != notify_list.end();iter++)
		{
			Ship	*ship = (*iter)->GetShip();
			if(ship == 0)
				continue;
			int	multiplier = (ship->MaxCargo() + 1)/75;
			if(multiplier == 0)
				continue;
			Job	*job = RandomJob(multiplier);
			if(job == 0)
				continue;
			
			job_index[next_job++] = job;
		}

		if(notify_list.size() < (unsigned)NEW_JOBS)
		{
			for(int count = notify_list.size();count < NEW_JOBS;count++)
			{
				Job	*job = RandomJob(1);
				if(job == 0)
					continue;
			
				job_index[next_job++] = job;
			}
		}
		NotifyPlayers();
	}
}

Job	*Work::CreateSpecialJob(const std::string& commodity,const std::string& from,
															const std::string& where_to,JobType job_type)
{
	if((job_index.size() >= 40) && (job_type == PLANET_OWNED))
		return(0);

	Job	*job = new Job;
	job->commod = commodity;
	job->commod[0] = std::toupper(job->commod[0]);
	job->from = from;
	job->to = where_to;
	job->quantity = 75;
	job->time_available = CalcTime(job);
	job->time_taken = 0;
	job->payment = (rand() % 5) + 8;
	job->credits = (rand() % 3) + 4;
	job->created = std::time(0) + ((std::rand() % 60) - 30); // cause variation in the job vanishing times
	job->planet_owned = job_type;
	
	if(job_type == PLANET_OWNED)
	{
		job_index[next_job++] = job;
		NotifyPlayers();
	}
	return(job);
}

void	Work::Deliver(Job *job)
{
	if(job->planet_owned == AUTO_GENERATED)
		return;

	FedMap	*from = Game::galaxy->FindMap(job->from);
	FedMap	*to = Game::galaxy->FindMap(job->to);
	if((from == 0) || (to == 0))
		return;
	long price = to->SellCommodity(job->commod);
	from->ChangeTreasury(price);
	to->ChangeTreasury(-price);
	std::string	owner(from->Owner());
	std::ostringstream	buffer;
	buffer << "Your consignment of " << job->quantity << " tons of ";
	buffer << job->commod << " has been sold on the " << job->to;
	buffer << " market for a total of " << price << "ig, which has ";
	buffer << "been credited to " << job->from << "'s treasury.\n";
	Player *player = Game::player_index->FindName(owner);
	if(player != 0)
		player->SendOrMail(buffer,"Your broker");
}

void	Work::DisplayJob(Player *player,Job *job,Job *pending)
{
	std::ostringstream	buffer;
	if(job != 0)
	{
		buffer << "Details of current contract:\n";
		buffer << "  Cargo is " << job->quantity << " tons of " << job->commod << "\n";
		buffer << "  Collect from " << job->from << " and deliver to " << job->to << "\n";
		buffer << "  Time allowed for completion is " << job->time_available << " gtu" << "\n";
		buffer << "  Time elapsed is " << job->time_taken << " gtu" << "\n";
		buffer << "  Value of contract is " << job->payment * job->quantity << "ig - ";
		buffer << job->credits << " hauling credits" << "\n";
		if(job->collected)
			buffer << "  The cargo is in your hold, awaiting delivery to " << job->to << "\n";
		else
			buffer << "  The cargo is awaiting your collection at " << job->from << "\n";
		if(job->planet_owned != AUTO_GENERATED)
			buffer << "  --- This job has been provided by the owner of " << job->from << " ---\n";
		player->Send(buffer);
	}

	if(pending != 0)
	{
		buffer.str("");
		buffer << "Details of contract offer awaiting your decision:\n";
		buffer << "  Cargo is " << pending->quantity << " tons of " << pending->commod << "\n";
		buffer << "  Collect from " << pending->from << " and deliver to " << pending->to << "\n";
		buffer << "  Time allowed for completion is " << pending->time_available << " gtu" << "\n";
		buffer << "  Time elapsed is " << pending->time_taken << " gtu" << "\n";
		buffer << "  Value of contract is " << pending->payment * pending->quantity << "ig - ";
		buffer << pending->credits << " hauling credits" << "\n";
		buffer << "  --- This job has been offered by the owner of " << pending->from << ", type <ACCEPT> to accept it ---\n";
		player->Send(buffer);
	}
}

void	Work::DisplayWork(Player *player)
{
	player->Send(Game::system->GetMessage("work","displaywork",1));
	Job	*job;
	std::ostringstream	buffer("");
	for(JobIndex::iterator iter = job_index.begin();iter != job_index.end();iter++)
	{
		buffer.str("");
		job = iter->second;
		buffer << "  " << iter->first << ". From " << job->from << " to " << job->to <<" - ";
		buffer << job->quantity << " tons of " << job->commod << " - ";
		buffer << job->time_available << "gtu " << job->payment << "ig/tn ";
		buffer << job->credits << "hcr"; 
		if(job->planet_owned != AUTO_GENERATED)
			buffer << " *";
		buffer << std::endl;
		player->Send(buffer);
	}
	if(job_index.size() == 0)
		player->Send("  None available - please try again in a minute or so.\n");
}

void	Work::ExpireJobs()
{
	time_t	now = std::time(0);
	for(JobIndex::iterator iter = job_index.begin();iter != job_index.end();++iter)
	{
		if((now - iter->second->created) >= FIVE_MIN)
		{
			Job	*job = iter->second;
			job_index.erase(iter);
			if(job->planet_owned != AUTO_GENERATED)
				Deliver(job);
			delete job;
			return;
		}
	}
}

void	Work::NotifyPlayers()
{
	for(NotifyList::iterator iter = notify_list.begin();iter != notify_list.end();)
	{
		if((*iter)->CurrentCartel() == cartel)
		{
			(*iter)->Send(Game::system->GetMessage("work","notifyplayers",1));
			++iter;
		}
		else	// not in this cartel - remove from the list 
			notify_list.erase(iter++);
	}
}

Job	*Work::RandomJob(int multiplier)
{
	Job	*job = new Job;
	job->commod =  Game::commodities->GetRandomCommodity();
	FedMap	*from_map = 0;
	FedMap	*to_map = 0;

	if((cartel->Name() == "Sol") && ((std::rand() % 100) < 75))
	{
		from_map = Game::galaxy->FindRandomMap("Sol");
		to_map = Game::galaxy->FindRandomMap("Sol",from_map);
	} 
	else
	{ 
		from_map = cartel->FindRandomMap();
		to_map = cartel->FindRandomMap(from_map);
	}

	if((from_map == 0) || (to_map == 0))
	{
		std::ostringstream	buffer;
		buffer << "Create Jobs: Unable to get an open map in " << cartel->Name();
		WriteLog(buffer);
		delete job;
		return(0);
	}

	job->from = from_map->Title();		
	job->to = to_map->Title();
	job->quantity = 75 * multiplier;
	job->time_available = CalcTime(job);
	job->time_taken = 0;
	job->payment = CalcPayment(job,multiplier);
	job->credits = 2 * multiplier;
	job->created = std::time(0) + ((std::rand() % 60) - 30); // some variation in the job vanishing times
	job->planet_owned = AUTO_GENERATED;

	// bonus jobs -  leave the brackets alone
	int roll = rand() % 100;
	if(roll == 0)		// high paying
	{
		job->payment *= 5;
		job->time_available = (job->time_available * 2)/3;
	}
	if(roll == 1)		// high tc
	{
		job->credits *= 5;
		job->time_available = (job->time_available * 3)/4;
	}

	return(job);
}

void	Work::RemovePlayer(Player *player)
{
	NotifyList::iterator iter = std::find(notify_list.begin(),notify_list.end(),player);
	if(iter != notify_list.end())
		notify_list.erase(iter);
}



void	Work::RemoveLastJob()
{
	if(job_index.size() > 0)
	{
		JobIndex::iterator	iter = --job_index.end();
		Job	*temp = iter->second;
		job_index.erase(iter);
		delete temp;
	}
}

