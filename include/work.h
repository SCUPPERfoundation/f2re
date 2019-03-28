/*----------------------------------------------------------------------
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
----------------------------------------------------------------------*/

#ifndef WORK_H
#define WORK_H

#include <list>
#include <map>
#include <string>

#include <ctime>

class	Cartel;
class DbJob;
class Job;
class	Player;

typedef std::map<int,Job *,std::less<int> >	JobIndex;
typedef std::list<Player *>						NotifyList;

class Work									// class to handle the generation of jobs
{
public:
	static const int		NO_JOB;
	
	enum	JobType	{ PLANET_OWNED, PERSONAL, AUTO_GENERATED	};

private:
	static const size_t	MIN_JOBS;	// maintain at least this number of jobs on the board
	static const int		NEW_JOBS;	// number of jobs to generate each time triggered
	static const int		FIVE_MIN;	// number of seconds in five minutes

	Cartel		*cartel;					// home cartel for this board
	JobIndex		job_index;				// jobs available
	NotifyList	notify_list;			// players to be notified when jobs are created
	int			next_job;				// number of the next job generated

	Job	*RandomJob(int multiplier);
	     
	void	ExpireJobs();	

public:
	static Job	*CreateJob(DbJob *db_job);
	
	static void	CreateDbJob(DbJob *db_job,Job *job);
	static void	Deliver(Job *job);
	static void	DisplayJob(Player *player,Job *job,Job *pending);

	Work(Cartel *home_cartel) : cartel(home_cartel), next_job(1) { 	}
	~Work();

	Job	*CreateSpecialJob(const std::string& commodity,const std::string& from,const std::string& where_to,JobType job_type);
	
	int	CalcPayment(Job *job,int multiplier);
	int	CalcTime(Job *job);

	void	Accept(Player *player,int job_no);
	void	AddPlayer(Player *player);
	void	CreateJobs();
	void	DisplayWork(Player *player);
	void	NotifyPlayers();
	void	RemoveLastJob();
	void	RemovePlayer(Player *player);
};

struct Job
{
	std::string		commod;				// what's being freighted
	std::string		from;					// where it's being picked up
	std::string		to;					// where it's being delivered
	int				quantity;			// how much
	int				time_available;	// how long to deliver
	int				time_taken;			// how long it is so far
	int				payment;				// how much we will pay, per ton
	int				credits;				// how many trader credits per pallet (75t)
	bool				collected;			// cargo picked up yet?
	time_t			created;				// time job created (used by work board only)
	Work::JobType	planet_owned;		//	see enum earlier
};

struct DbJob
{
	char	commod[16];						// what's being freighted
	char	from[24];						// where it's being picked up
	char	to[24];							// where it's being delivered
	int	quantity;						// how much
	int	time_available;				// how long to deliver
	int	time_taken;						// how long it is so far
	int	payment;							// how much we will pay, per ton
	int	credits;							// how many trader credits per pallet (75t)
};

#endif
