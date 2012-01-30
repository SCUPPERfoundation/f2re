/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 1985-12
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef PLAYERINDEX_H
#define PLAYERINDEX_H

#include <fstream>
#include <list>
#include <map>
#include <string>
#include <utility>

#include "ipc.h"


class Db;
class	Login;
class	LoginRec;
class	Newbie;
class Player;
class Tokens;
class	XMLLoginRec;

// NOTE: The player records are 'owned' by NameIndex, which handles deletions
typedef	std::map<const std::string,Player *,std::less<const std::string> >	NameIndex;
typedef	std::multimap<const std::string,Player *,std::less<const std::string> >	EmailIndex;
typedef	std::map<int,Player *,std::less<int> >	DescIndex;
typedef	std::list<Player *>	Reaper;

class	PlayerIndex		// manage the player database
{
public:
	static const int	DISCARD;				// use to discard unwanted billing replies
	enum { WITH_OBJECTS, NO_OBJECTS };	// save/don't save out the locker this time

private:
	NameIndex	player_index;			// all players in the DB keyed by name
	NameIndex	account_index;			// all players in the DB keyed by account name
	NameIndex	current_index;			// players in game keyed by name
	EmailIndex	email_index;			// all players in the DB keyed by email (may be more than one)
	DescIndex	desc_index;				// players in game keyed by socket descriptor
	Reaper		reaper;					// list of player who have died in the last one second

	Db				*db;						// Berkeley db database of players
	Login			*login;					// login handler
	Newbie		*newbie;					// persona set up handler
	std::string	billing_buffer;		// buffer for answers from billing

	int			max_players;			// maximum number of players in the last hour
	int			min_players;			// minimum number of players in the last hour
	int			total_player_time;	// total amount of time all players have been in the game
	int			session_max;			// highest simultaious usage in this session

	std::ofstream	graph_file;

	void	LoadIndices();
	void	Terminate(Player *player,std::string& name);

public:
	PlayerIndex(char *file_name);
	~PlayerIndex();

	Player	*FindAccount(const std::string& name);	// Find in all players     - key = account name
	Player	*FindCurrent(const std::string& name);	// Find in current players - key = name
	Player	*FindCurrent(int desc);						// Find in current players - key = socket descriptor
	Player	*FindName(const std::string& name);		// Find in all players     - key = name

	std::pair<int,int>	NumberOutOfDate(int days = 365);

	int	FindAllAlts(Player *player,const std::string& ip_address);
	int	FindAlts(Player *player,const std::string& ip_address);
	int	NumberOfPlayers()									{ return(current_index.size());	}
	int	NumberOfPlayersAtRank(int rank);

	bool	ProcessInput(int sd,std::string& text);
	bool	ProcessBilling(std::string& input_text);
	bool	SendStaffMssg(const std::string& mssg);
	bool	ValidatePlayerRecord(Player *player);

	void	AccountOK(LoginRec *rec);
	void	AccountOK(XMLLoginRec *rec);
	void	Broadcast(Player *player,std::string mssg);
	void	CallNightWatch(Player *player,Player *target);
	void	Com(Player *player,std::string mssg);
	void	DisplayAccount(Player *player,const std::string& id);
	void	DisplaySameEmail(Player *player,const std::string& email);
	void	DisplayStaff(Player *player,Tokens *tokens,const std::string& line);
	void	DisplayShipOwners(Player *player,const std::string& regname);
	void	GrimReaper();
	void	InitMapPointers();
	void	JobInfo(std::string mssg);
	void	KeepAlive();
	void	LoadInventoryObjects();
	void	LoadLockerObjects();
	void	LogOff(Player *player);
	void	LostLine(int sd);
	void	NewPlayer(Player *player);
	void	PromotePlayers();
	void	QueueForReaper(Player *player);
	void	Qw(Player *player);
	void	ReportSocketError(int sd,int error_number);
	void	Save(Player *player,int which_bits);
	void	SaveAllPlayers(int which_bits = WITH_OBJECTS);
	void	SaveTeleporterPlayers();
	void	SendPlayerInfo(Player *player);
	void	SpynetNotice(const std::string& text);
	void	Suicide(Player *player);
	void	UpdateCompanyTime();
	void	UpdateFleets();
	void	UpdateGraph();
	void	Who(Player *player,int rank = -1);
	void	WhoIs(Player *player,const std::string& who);
	void	WhoIsAccount(Player *player,const std::string& who);
	void	WriteGraph();
	void	WriteGraphSummary();
	void	XmlPlayerLeft(Player *player);
	void	XmlPlayerStart(Player *player);
	void	Zap(Player *player,Player *who_by);

	// temporary stuff to merge old billing info with player record
	static int MAX_BUFFER;
	void	UpdateBillingInfo(std::string& input_file);
	bool	LoadInputFile(std::string& input_file,std::list<std::string>& billing_list);
	bool	ProcessBillingLine(std::string& line);
};

#endif
