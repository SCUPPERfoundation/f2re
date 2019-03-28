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

#include "misc.h"

#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

#include <fcntl.h>
#include <pwd.h>
#include <unistd.h>

#include <sys/dir.h>
#include <sys/resource.h>
#include <sys/time.h>

#include "blish_info.h"
#include "db_object.h"
#include "db_player.h"
#include "fed.h"
#include "fedmap.h"
#include	"fight_list.h"
#include "galaxy.h"
#include "global_player_vars_table.h"
#include "ipc.h"
#include "mail.h"
#include "player_index.h"
#include "production.h"
#include "review.h"
#include "syndicate.h"
#include	"unload.h"

namespace Game
{
	sig_atomic_t				wrap_up = 0;
	std::string					load_billing_info = "";
	std::string					start_up = "";

	BusinessRegister			*business_register = 0;
	ChannelManager				*channel_manager = 0;
	CityBuildInfo				*city_build_info = 0;
	CmdParser					*parser = 0;
	Commodities					*commodities = 0;
	CompanyRegister			*company_register = 0;
	Courier						*courier = 0;
	DBObject						*db_object = 0;
	Fed							*fed = 0;
	FedMail						*fed_mail = 0;
	FedMap						*system = 0;
	FightList					*fight_list = 0;
	Forbidden					*forbidden = 0;
	Galaxy						*galaxy = 0;
	GlobalPlayerVarsTable	*global_player_vars_table = 0;
	IPC							*ipc = 0;
	Syndicate					*syndicate = 0;
	NavComp						*nav_comp = 0;
	NoticeBoard					*notices = 0;
	PlayerIndex					*player_index = 0;
	Production					*production = 0;
	Review						*review = 0;
	Review						*financial = 0;
	Unload						*unload = 0;

//	bool							has_a_newbie = false;
	bool							test = false;
}


void AlarmHandler(int)
{
	static int	ticks = 0;	// every half second
	static int	mins = 0;
	const int	saved_errno = errno;

	IPC::IncTicks();
	if((++ticks % 2) != 0)
		return;

	Game::unload->ProcessList();
	Game::player_index->GrimReaper();
	FedMap::ProcessDelayList();

	if(ticks == 120)
	{
		if(++mins == 60)
		{
			mins = 0;
			if(mins == 0)
				Game::galaxy->InformerRingBust();
		}
		ticks = 0;
	}

	switch(ticks/2)
	{
		case  0:	ResetTimer();
					Game::galaxy->UpdateExchanges();
					Game::production->Execute();
					Game::player_index->UpdateCompanyTime();
					Game::player_index->UpdateGraph();
					Game::galaxy->MoveMobiles();
					switch(mins)
					{
						case  0:	Game::player_index->WriteGraph();	break;
					}
					break;

		case 10:	//	if(!Game::has_a_newbie)
						Game::syndicate->CreateJobs();
					break;
		case 15:	Game::galaxy->UpdateExchanges();						break;
		case 25:	Game::player_index->KeepAlive();						break;
		case 30:	ResetTimer();
					Game::galaxy->UpdateExchanges();
					Game::galaxy->MoveMobiles();
					break;
		case 45:	Game::galaxy->UpdateExchanges();						break;

	}
	errno = saved_errno;
}

const std::string&	EscapeXML(const std::string& text)
{
	static std::string	xml_text;
	std::ostringstream	buffer;
	int length = text.length();

	for (int count = 0;count < length;count++)
	{
		switch(text[count])
		{
			case  '<':	buffer << "&lt;";			break;
			case  '>':	buffer << "&gt;";			break;
			case  '&':	buffer << "&amp;";		break;
			case '\'':	buffer << "&apos;";		break;
			case '\"':	buffer << "&quot;";		break;

			default:		buffer << text[count];	break;
		}
	}
	xml_text = buffer.str();
	return(xml_text);
}

bool	GetRunLock(const char *lock_file)
{
	int	fd;
	char	file_name[MAXNAMLEN +1], pid[16];

	std::snprintf(file_name,MAXNAMLEN +1,"%s/%s",HomeDir(),lock_file);
	if((fd = open(file_name,O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP)) < 0)
	{
		std::fprintf(stderr,"Unable to open lock file.\n");
		return(false);
	}

	// lock the file
	if(lockf(fd,F_TLOCK,0) < 0)
		return(false);

	// write out the pid and make it read only for cron etc
	std::snprintf(pid,16,"%d\n",getpid());
	write(fd,pid,std::strlen(pid));
	fcntl(fd,F_SETFL,(fcntl(fd,F_GETFL) | O_RDONLY) & ~O_WRONLY);

	return(true);
}

const char	*HomeDir()
{
	static char	home[MAXNAMLEN +1];
	static bool	is_set = false;

	if(!is_set)
	{
		getcwd(home,MAXNAMLEN +1);
		is_set = true;
	}

	return(home);
}

void	InputBuffer(std::string& buffer,std::string& text,std::string& line)
{
	buffer += text;
	std::string::size_type idx = buffer.find('\n');
	if(idx == std::string::npos)
	{
		line = "";
		return;
	}
	line = buffer.substr(0,idx);
	buffer.erase(0,idx + 1);

	// strip trailing spaces
	int count = line.length() -1;
	if(line[count] == ' ')
	{
		count--;
		while(line[count] == ' ')
			count--;
		line.erase(++count);
	}
}

bool	IPCCallBack(int status,int sd,char *text)
{
	std::string	input_text(text);
	int length = input_text.length();
	for(int count = 0;count < length;count++)
	{
		if(isascii(input_text[count]) == 0)
			input_text[count] = '-';
		if((iscntrl(input_text[count]) != 0) && (input_text[count] != '\n'))
			input_text[count] = '-';
	}

	bool	ret_status = true;
	switch(status)
	{
		case IPC::NEW:			if(std::strstr(text,"74.34.101.192") != NULL)
									{
										WriteLog("Nkir (74.34.101.192) kicked off!");
										return(false);
									}
		case IPC::READ:		ret_status = Game::player_index->ProcessInput(sd,input_text);
									return(ret_status);
		case IPC::LOST:		Game::player_index->LostLine(sd);
									return(true);
	}
	return(true);
}

bool	IsAVowel(const char letter)
{
	if((letter == 'a') || (letter == 'e') || (letter == 'i') ||
										(letter == 'o') || (letter == 'u'))
		return(true);
	else
		return(false);
}

void	MakeMoneyString(long amount,std::ostringstream& buffer)
{
	if(std::abs(amount) < 1000L)
		buffer << amount << "ig";
	else
	{
		if(std::abs(amount) < 1000000L)
			buffer << amount/1000 << "Kig";
		else
			buffer << amount/1000000 << "Mig";
	}
}

void	MakeNumberString(long amount,std::ostringstream& buffer)
{
	std::ostringstream	buff;
	buff << amount;
	std::string	number(buff.str());
	for(int count = number.length() -3;count > 0;count -= 3)
		number.insert(count,",");
	if((number[0] == '-') && (number[1] == ','))
		number.erase(1,1);
	buffer << number;
}

// only works between +999,999,999 and -999,999,999 - should be enough!
const std::string& MakeNumberString(long amount)
{
	static std::string	ret_string;
	std::ostringstream	buffer;

	if(amount > 0)
	{
		if(amount > 1000000)
			buffer << amount/1000000 << ",";
		amount %= 1000000;
	}
	else
	{
		if(amount < -1000000)
			buffer << amount/1000000 << ",";
		amount = std::abs(amount % 1000000);
	}
	if(amount > 1000)
		buffer << amount/1000 << ",";
	buffer << amount % 1000;

	ret_string = buffer.str();
	return(ret_string);
}

bool	NoCaseCmp(const std::string& s1,const std::string& s2)
{
	if(strcasecmp(s1.c_str(),s2.c_str()) == 0)
		return(true);
	else
		return(false);
}

std::string&	Normalise(std::string& name)
{
	int len = name.length();
	name[0] = std::toupper(name[0]);
	for(int count = 1;count < len;count++)
		name[count] = std::tolower(name[count]);
	return(name);
}

std::string&	NormalisePlanetTitle(std::string& name)
{
	int len = name.length();
	name[0] = std::toupper(name[0]);
	for(int count = 1;count < len;count++)
	{
		if(name[count -1] == ' ')
			name[count] = std::toupper(name[count]);
		else
			name[count] = std::tolower(name[count]);
	}
	return(name);
}

void	SignalSetUp()
{
	sigset_t	set;

	sigemptyset(&set);
	sigaddset(&set,SIGALRM);
	sigaddset(&set,SIGTERM);
	sigaddset(&set,SIGPIPE);
	sigaddset(&set,SIGUSR1);
	sigaddset(&set,SIGUSR2);
	sigprocmask(SIG_BLOCK,&set,0);

	struct sigaction	act;

	//	alarm handler for background timer to run daemons etc
	act.sa_handler = AlarmHandler;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask,SIGTERM);	// blockSigterm while processing Sigalarm
	act.sa_flags = 0;
#if defined(SA_RESTART)
	act.sa_flags |= SA_RESTART;
#endif
	sigaction(SIGALRM,&act,0);

	// ignore Sighup and Sigpipe
	act.sa_handler = SIG_IGN;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
#if defined(SA_RESTART)
	act.sa_flags |= SA_RESTART;
#endif
	sigaction(SIGHUP,&act,0);
	sigaction(SIGPIPE,&act,0);

	// USR1 & 2 handler - close down warnings
	act.sa_handler = SigUSRHandler;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask,SIGUSR1);
	act.sa_flags = 0;
#if defined(SA_RESTART)
	act.sa_flags |= SA_RESTART;
#endif
	sigaction(SIGUSR1,&act,0);

	act.sa_handler = SigUSRHandler;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask,SIGUSR2);
	act.sa_flags = 0;
#if defined(SA_RESTART)
	act.sa_flags |= SA_RESTART;
#endif
	sigaction(SIGUSR2,&act,0);

	// set up SigTerm for closing down
	act.sa_handler = TermHandler;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask,SIGALRM);	// block SIGALRM while we close
	act.sa_flags = 0;
#if defined(SA_RESTART)
	act.sa_flags |= SA_RESTART;
#endif
	sigaction(SIGTERM,&act,0);

	// now hook up a background timer with a one second period...
	struct itimerval	value;
	timerclear(&value.it_interval);
	timerclear(&value.it_value);
	value.it_interval.tv_usec = value.it_value.tv_usec = 500000;
	setitimer(ITIMER_REAL,&value,0);
}

void	SigUSRHandler(int num)
{
	std::string	text;
	std::string mssg;
	if(num == SIGUSR1)
	{
		text = "SIGUSR1 received";
		mssg = "Federation II will be closing down for a short while in ten minutes time.\n";
		Game::player_index->Broadcast(0,mssg);
	}
	else
	{
		text = "SIGUSR2 received";
		mssg = "Federation II will be closing down for a short while in six minutes time.\n";
		Game::player_index->Broadcast(0,mssg);
	}
	WriteLog(text);
}

const std::string&	Stardate()
{
	static std::string	stardate("");

	time_t	now = std::time(0);
	std::ostringstream	buffer("");
	buffer << "Stardate: " << now;
	stardate = buffer.str();
	stardate.insert(16,":");
	return(stardate);
}

void	TermHandler(int)
{
	Game::wrap_up = 1;
	std::string mssg("Federation II is about to close down for a short while.\n");
	Game::player_index->Broadcast(0,mssg);
}

void	WriteErrLog(const std::string& text)
{
	struct std::tm	*now;
	std::time_t		timer;
	std::time(&timer);
	now = gmtime(&timer);
	std::ostringstream	buffer;
	buffer << std::setw(2) << std::setfill('0') << now->tm_hour << ":";
	buffer << std::setw(2) << std::setfill('0') << now->tm_min << " " << text;
	std::cerr << buffer.str() << std::endl;
}

void	WriteLog(const std::string& text)
{
	static std::ofstream	log_file;
	static bool is_open = false;

	if(!is_open)
	{
		char	file_name[MAXNAMLEN +1];
		std::snprintf(file_name,MAXNAMLEN +1,"%s/log/fed2.log",HomeDir());
		log_file.open(file_name,std::ios::out | std::ios::app);
		is_open = true;
		std::fprintf(stderr,"Home directory is '%s'\n",file_name);

	}

	struct std::tm	*now;
	std::time_t		timer;
	std::time(&timer);
	now = gmtime(&timer);
	if(is_open)
	{
		std::ostringstream	buffer;
		buffer << (now->tm_year + 1900) << std::setw(2) << std::setfill('0') << (now->tm_mon + 1);
		buffer << std::setw(2) << std::setfill('0') << now->tm_mday << " ";
		buffer << std::setw(2) << std::setfill('0') << now->tm_hour << ":";
		buffer << std::setw(2) << std::setfill('0') << now->tm_min << " " << text;
		log_file << buffer.str() << std::endl;
	}
	else
		std::cerr <<  now->tm_hour << ":" << now->tm_min << text << std::endl;
}

void	WriteLog(const std::ostringstream& buffer)
{
	WriteLog(buffer.str());
}

void	WriteNavLog(const std::string& text)
{
	static std::ofstream	nav_file;
	static bool is_open = false;

	std::time_t		timer;
	std::time(&timer);
	struct std::tm	*now;
	now = gmtime(&timer);

	if(!is_open)
	{
		char	file_name[MAXNAMLEN +1];
		std::snprintf(file_name,MAXNAMLEN +1,"%s/log/nav.log",HomeDir());
		nav_file.open(file_name,std::ios::out | std::ios::app);
		nav_file << std::endl <<  std::asctime(now);
		is_open = true;
	}

	if(is_open)
	{
		std::ostringstream	buffer;
		buffer << (now->tm_year + 1900) << std::setw(2) << std::setfill('0') << (now->tm_mon + 1);
		buffer << std::setw(2) << std::setfill('0') << now->tm_mday << " ";
		buffer << std::setw(2) << std::setfill('0') << now->tm_hour << ":";
		buffer << std::setw(2) << std::setfill('0') << now->tm_min << " " << text;
		nav_file << buffer.str() << std::endl;
	}
	else
		std::cerr <<  now->tm_hour << ":" << now->tm_min << text << std::endl;
}


void	ResetTimer()
{
	static int	ten_min_hours = 12;
	static int	ten_min_mins = 55;
	static bool	ten_min_sent = false;

	static int	five_min_hours = 12;
	static int	five_min_mins = 59;
	static bool	five_min_sent = false;

	static int	reset_hours = 13;
	static int	reset_mins = 5;

	static bool hour_changed = false;
	static int	old_hour = -1;

	std::time_t	now = std::time(0);
	std::tm		*l_time = localtime(&now);

	if(old_hour == -1)	// first time thru
	{
		old_hour = l_time->tm_hour;

		return;
	}

	// just in case the reset is short enuff to result in a fire up that is
	// still the same hour and minute as the reset! Note that the order of
	// evaluation is important for efficiency...
	if((hour_changed == false) && (old_hour != l_time->tm_hour))
	{
		hour_changed = true;
	}

	// Check for the ten minute reset warning
	if((ten_min_hours == l_time->tm_hour) && !ten_min_sent && (ten_min_mins == l_time->tm_min) && hour_changed)
	{
		raise(SIGUSR1);
		ten_min_sent = true;
		return;
	}

	// Check for the five minute reset warning
	if((five_min_hours == l_time->tm_hour) && !five_min_sent && (five_min_mins == l_time->tm_min) && hour_changed)
	{
		raise(SIGUSR2);
		five_min_sent = true;
		return;
	}

	// Check for the reset
	if((reset_hours == l_time->tm_hour) && (reset_mins == l_time->tm_min) && hour_changed)
	{
		raise(SIGTERM);
	}
}

