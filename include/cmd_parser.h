/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef CMDPARSER_H
#define CMDPARSER_H

#include <string>
#include "player.h"

class	Admin;
class Assign;
class BuildParser;
class	BuyParser;
class	ChangeParser;
class	CmdHelp;
class Display;
class ExpParser;
class GiveParser;
class JoinParser;
class	Player;
class	PriceCheck;
class RepairParser;
class SellParser;
class SetParser;
class	Tokens;

class	CmdParser
{
private:
	static const std::string	vocab[];
	static const int	NO_CMD;

	Admin				*admin;
	Assign			*assign;
	BuildParser		*build_parser;
	BuyParser		*buy_parser;
	ChangeParser	*change_parser;
	CmdHelp			*help;
	Display			*display;
	ExpParser		*exp_parser;
	GiveParser		*give_parser;
	JoinParser		*join_parser;
	PriceCheck		*price_check;
	RepairParser	*repair;
	SellParser		*sell_parser;
	SetParser		*set_parser;
	Tokens			*tokens;

	int	GetCommand();

	bool	AcceptMember(Player *player,std::string& line);
	bool	CheckBusinessRegistration(Player *player,std::string& line);
	bool	ProcessLocation(Player *player);
	bool	ProcessObject(Player *player,std::string& line);

	void	Accept(Player *player,std::string& line);
	void	Act(Player *player,std::string& line);
	void	Address(Player *player);
	void	AllocateCity(Player *player,std::string& line);
	void	AlphaCrew(Player *player);
	void	Approve(Player *player);
	void	Bid4Shares(Player *player,std::string& line);
	void	Board(Player *player);
	void	Brief(Player *player);
	void	Broadcast(Player *player,std::string& line);
	void	Call(Player *player);
	void	CallNightwatch(Player *player);
	void	CallStaff(Player *player);
	void	Cancel(Player *player);
	void	Carry(Player *player,std::string& line);
	void	Check(Player *player,std::string& line);
	void	CheckPrice(Player *player,std::string& line);
	void	Claim(Player *player);
	void 	Clear(Player *player);
	void	Clip(Player *player,std::string& line);
	void	Com(Player *player,std::string& line);
	void	Comms(Player *player);
	void	Colonize(Player *player);
	void	Damage(Player *player,std::string& line);
	void	Declare(Player *player);
	void	Demolish(Player *player);
	void	Divert(Player *player);
	void	Doff(Player *player,std::string& line);
	void	Drop(Player *player,std::string & line);
	void	Emote(Player *player);
	void	Examine(Player *player,std::string& line);
	void	Execute(Player *player,int cmd, std::string& line);
	void	Exile(Player *player);
	void	Expel(Player *player,std::string& line);
	void	Fed2(Player *player);
	void	Fetch(Player *player);
	void	Flee(Player *player);
	void	Flush(Player *player);
	void	Follow(Player *player);
	void	Freeze(Player *player);
	void	Full(Player *player);
	void	Gag(Player *player);
	void	GeneralHelp(Player *player,std::string& line);
	void	Get(Player *player,std::string& line);
	void	Glance(Player *player);
	void	Goto(Player *player);
	void	Ignore(Player *player);
	void	IPO(Player *player,std::string & line);
	void	Issue(Player *player);
	void	Jobs(Player *player);
	void	Jump(Player *player,std::string& line);
	void	Launch(Player *player,std::string & line);
	void	Leave(Player *player);
	void	Liquidate(Player *player);
	void	Lock(Player *player);
	void	Marry(Player *player);
	void	Mood(Player *player, std::string& line);
	void	Move(Player *player, std::string& line);
	void	Offer(Player *player, std::string& line);
	void	Pardon(Player *player);
	void	Pocket(Player *player,std::string& line);
	void	Post(Player *player,std::string& line);
	void	PostJob(Player *player,std::string& line);
	void	PostNotice(Player *player,std::string& line);
	void	Ranks(Player *player);
	void	Read(Player *player,std::string& line);
	void	Register(Player *player,std::string& line);
	void	Reject(Player *player,std::string& line);
	void	Relay(Player *player);
	void	RemotePriceCheck(Player *player,std::string& line);
	void 	Remove(Player *player);
	void	Repay(Player *player);
	void	Report(Player *player);
	void	Reset(Player *player);
	void	Retrieve(Player *player,std::string& line);
	void	Save(Player *player);
	void	Say(Player *player,std::string& line);
	void	Send(Player *player,std::string& line);
	void	ShipStatus(Player *player);
	void	Smile(Player *player);
	void	Sponsor(Player *player);
	void	Spynet(Player *player);
	void	SpynetFinancial(Player *player);
	void	SpynetReport(Player *player);
	void	SpynetReview(Player *player);
	void	Start(Player *player);
	void	Stash(Player *player,std::string& line,bool hidden = false);
	void	StopCityProduction(Player *player,std::string& line);
	void	Store(Player *player);
	void	Target(Player* player);
	void	Teleport(Player *player,std::string& line);
	void	Tell(Player *player,std::string& line);
	void	TermWidth(Player *player);
	void	Unclip(Player *player);
	void	UnDivert(Player *player);
	void	UnGag(Player *player);
	void	UnIgnore(Player *player);
	void	UnLock(Player *player);
	void	UnPost(Player *player);
	void	Update(Player *player,std::string& line);
	void	UpdateEMail(Player *player,std::string& line);
	void	UpdatePassword(Player *player,std::string& line);
	void	Upgrade(Player *player);
	void	UpgradeFactory(Player *player);
	void	UpgradeStorage(Player *player);
	void	Wear(Player *player,std::string& line);
	void	WhereIs(Player *player,std::string& line);
	void	Who(Player *player,std::string& line);
	void	WhoElse(Player *player);
	void	WhoIs(Player *player,std::string& line);
	void	Xfer(Player *player,std::string& line);
	void	Xt(Player *player,std::string& line);
	void	Zap(Player *player);

public:
	CmdParser();
	~CmdParser();

	bool	IsInVocab(std::string& text);

	void	Parse(Player *player,std::string& line);
	void	WriteHelpFailures();
};

#endif
