/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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



