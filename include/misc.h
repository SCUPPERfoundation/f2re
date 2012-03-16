/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-12
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef MISC_H
#define MISC_H

#include <sstream>
#include <string>

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
	const int				MAX_PLAYERS = 400;
	extern sig_atomic_t	wrap_up;
	extern std::string	load_billing_info;

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
	
	extern bool							test;		// true means we are running tests, not production
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
extern void	SignalSetUp();
extern void	SigUSRHandler(int num);
extern void	TermHandler(int);
extern void	WriteLog(const std::string& text);
extern void	WriteLog(const std::ostringstream& buffer);
extern void	WriteErrLog(const std::string& text);
extern void	WriteNavLog(const std::string& text);

#endif
