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

#ifndef MISC_H
#define MISC_H

#include <sstream>
#include <string>

#include <ctime>
#include <signal.h>

class BusinessRegister;
class ChannelManager;
class	CityBuildInfo;
class	CmdParser;
class	Commodities;
class	CompanyRegister;
class	Courier;
class DBObject;
class	Fed;
class FedMail;
class	FedMap;
class FightList;
class	Forbidden;
class	Galaxy;
class GlobalPlayerVarsTable;
class	IPC;
class Syndicate;
class NavComp;
class	NoticeBoard;
class	PlayerIndex;
class Production;
class	Review;
class	Unload;

namespace Game
{
	const int	MAX_PLAYERS = 400;
	const int	CYCLE_TIME = 22;

	extern sig_atomic_t	wrap_up;
	extern std::string	load_billing_info;
	extern std::string	start_up;

	extern BusinessRegister			*business_register;
	extern ChannelManager			*channel_manager;
	extern CityBuildInfo				*city_build_info;
	extern CmdParser					*parser;
	extern Commodities				*commodities;
	extern CompanyRegister			*company_register;
	extern Courier						*courier;
	extern DBObject					*db_object;
	extern Fed							*fed;
	extern FedMail						*fed_mail;
	extern FedMap						*system;
	extern FightList					*fight_list;
	extern Forbidden					*forbidden;
	extern Galaxy						*galaxy;
	extern GlobalPlayerVarsTable	*global_player_vars_table;
	extern IPC							*ipc;
	extern Syndicate					*syndicate;
	extern NavComp						*nav_comp;
	extern NoticeBoard				*notices;
	extern PlayerIndex				*player_index;
	extern Production					*production;
	extern Review						*review;
	extern Review						*financial;
	extern Unload						*unload;

//	extern bool							has_a_newbie;	// Are we currently processing a newbie?
	extern bool							test;				// true means we are running tests, not production
}

extern const std::string&	EscapeXML(const std::string& text);
extern const std::string&	MakeNumberString(long amount);
extern const std::string&	Stardate();

extern std::string&	Normalise(std::string& buffer);
extern std::string&	NormalisePlanetTitle(std::string& buffer);

extern const char	*HomeDir();

extern bool	GetRunLock(const char *lock_file);
extern bool	IPCCallBack(int status,int sd,char *text);
extern bool	IsAVowel(const char letter);
extern bool	NoCaseCmp(const std::string& s1,const std::string& s2);

extern void	AlarmHandler(int);
extern void	InputBuffer(std::string& buffer,std::string& text,std::string& line);
extern void	MakeMoneyString(long amount,std::ostringstream& buffer);
extern void	MakeNumberString(long amount,std::ostringstream& buffer);
extern void	ResetTimer();
extern void	SignalSetUp();
extern void	SigUSRHandler(int num);
extern void	TermHandler(int);
extern void	WriteLog(const std::string& text);
extern void	WriteLog(const std::ostringstream& buffer);
extern void	WriteErrLog(const std::string& text);
extern void	WriteNavLog(const std::string& text);

#endif
