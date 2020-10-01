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

#include "scr_checkstat.h"

#include "event_number.h"
#include "player.h"

CheckStat::CheckStat(const char **attrib,FedMap *fed_map) : Script(fed_map)
{
	who = FindWhoAttribute(attrib);
	stat = FindSkillAttribute(attrib,&value);
	higher = FindEventAttribute(attrib,"higher",fed_map);
	equals = FindEventAttribute(attrib,"equals",fed_map);
	lower = FindEventAttribute(attrib,"lower",fed_map);
}

CheckStat::~CheckStat()
{
	if(!higher->IsNull())	delete higher;
	if(!equals->IsNull())	delete equals;
	if(!lower->IsNull())		delete lower;
}


int	CheckStat::Process(Player *player)
{
	switch(stat)
	{
		case STR:
		case STA:
		case DEX:
		case INT:
		case  AK:
		case  TC:
		case TOTAL:
		case REMOTE_CHECK:	
		case RANK:		return(ProcessNonCash(player));
		case CASH:
		case REWARD:
		case C_CASH:	return(ProcessCash(player));
	}

	return(CONTINUE);
}

int	CheckStat::ProcessCash(Player *player)
{
	long	cash;
	switch(stat)
	{
		case CASH:		cash = player->Cash();			break;
		case REWARD:	cash = player->Reward();		break;
		case C_CASH:	cash = player->CompAndBusCash();	break;
		default:			cash = 0L;							break;
	}
	long cash_value = value;
	if(cash > cash_value)		return(higher->Process(player));
	if(cash == cash_value)		return(equals->Process(player));
	if(cash < cash_value)		return(lower->Process(player));
	return(CONTINUE);
}

int	CheckStat::ProcessNonCash(Player *player)
{
	int cur_stat;
	switch(stat)
	{
		case STR:				cur_stat = player->CurStrength();	break;
		case STA:				cur_stat = player->CurStamina();		break;
		case DEX:				cur_stat = player->CurDex();			break;
		case INT:				cur_stat = player->CurIntel();		break;
		case  AK:				cur_stat = player->AKJobs();			break;
		case  TC:				cur_stat = player->TraderJobs();		break;
		case TOTAL: 
			cur_stat = player->CurStrength() + player->CurStamina()
											+ player->CurDex() + player->CurIntel();
			break;
		case RANK:				cur_stat = player->Rank();				break;
		case REMOTE_CHECK:	cur_stat = player->RemoteCheck();	break;
		default:					cur_stat = 999;							break;
	}

	if(cur_stat != 999)
	{
		if(cur_stat > value)		return(higher->Process(player));
		if(cur_stat == value)	return(equals->Process(player));
		if(cur_stat < value)		return(lower->Process(player));
	}
	return(CONTINUE);
}



