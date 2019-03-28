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

#include "cmd_admin.h"

#include <sstream>
#include <utility>

#include "fedmap.h"
#include "galaxy.h"
#include "global_player_vars_table.h"
#include "inventory.h"
#include "misc.h"
#include "output_filter.h"
#include "player.h"
#include "scr_nomatch.h"
#include "tokens.h"


void	Admin::Add(Player *player,Tokens *tokens,std::string& line)
{
	static const std::string	cmds[] = { "slithy", "cash", "company", "treasury", "stat", "" };

	static const std::string	error("The command is 'admin add slithy|cash|company|treasury amount player_name'\n");
	static const std::string	name_error("I can't find that name!\n");
	static const std::string	no_company("That player doesn't seem to have a company!\n");

	if(tokens->Size() < 5)
	{
		player->Send(error);
		return;
	}

	if(tokens->Get(2) == "stat")

	{
		AddStat(player,tokens);
		return;
	}

	int amount = std::atoi(tokens->Get(3).c_str());
	if(amount == 0)
	{
		player->Send(error);
		return;
	}
	std::string	player_name(tokens->Get(4));
	Player	*target = 0;
	if(player_name == "pto")	// This play managed to get an uncapitalized name!
		target = Game::player_index->FindName(player_name);
	else
		target = Game::player_index->FindName(Normalise(player_name));
	std::string	title(tokens->GetRestOfLine(line,4,Tokens::PLANET));
	FedMap	*planet = Game::galaxy->FindMap(title);

	int which = 9999;
	for(int count = 0;cmds[count] != "";count++)
	{
		if(cmds[count] == tokens->Get(2))
		{
			which = count;
			break;
		}
	}

	if((which == 3) && (planet == 0))
	{
		player->Send(name_error);
		return;
	}
	else
	{
		if((which < 3) && (target == 0))
		{
			player->Send(name_error);
			return;
		}
	}

	std::ostringstream	buffer;
	bool ok = true;
	switch(which)
	{
		case 0:	target->AddSlithy(amount);
					buffer << target->Name() << " has been credited with " << amount << " slithy toves ";
					break;
		case 1:	target->ChangeCash(amount,true);
					buffer << target->Name() << " has been credited with " << amount << " personal cash ";
					break;
		case 2:	ok = target->ChangeCompanyCash(amount,true);
					buffer << target->Name() << " has been credited with " << amount << " company cash ";
					break;
		case 3:	planet->UpdateCash(amount);
					buffer << planet->Title() << " has been credited with " << amount << " treasury cash ";
					break;
		case 4:	AddStat(player,tokens);	return;

		default:	player->Send(error);		return;
	}

	WriteLog(buffer.str() + "by " + player->Name());
	if(ok)
	{
		buffer << "\n";
		player->Send(buffer);
	}
	else
		player->Send(no_company);

	if(target != 0)
		Game::player_index->Save(target,PlayerIndex::NO_OBJECTS);
	if(planet != 0)
		planet->SaveInfrastructure();
}

void	Admin::AddStat(Player *player,Tokens *tokens)
{
	static const std::string	stat_names[] = { "stam", "dex", "str", "int", ""	};
	static const std::string	wrong_stat("Options are 'stam', 'dex', 'str' and 'int'.\n");
	static const std::string	wrong_format("Format is: ADMIN ADD STAT statname amount playername.\n");
	static const std::string	wrong_name("Can't find a player with that name.\n");

	if(tokens->Size() < 6)
	{
		player->Send(wrong_format);
		return;
	}

	int stat = -1;
	for(int count = 0;stat_names[count] != "";count++)
	{
		if(stat_names[count] == tokens->Get(3))
		{
			stat = count;
			break;
		}
	}
	if(stat < 0)
	{
		player->Send(wrong_stat);
		return;
	}

	int amount = std::atoi(tokens->Get(4).c_str());
	if(amount == 0)
	{
		player->Send(wrong_format);
		return;
	}

	std::string	player_name(tokens->Get(5));
	Player	*target = Game::player_index->FindName(Normalise(player_name));
	if(target == 0)
	{
		player->Send(wrong_name);
		return;
	}

	switch(stat)
	{
		case	0:	target->ChangeStat(Script::STA,amount,true,false);
					if(amount > 0)
						target->ChangeStat(Script::STA,amount,true,true);
					break;
		case	1:	target->ChangeStat(Script::DEX,amount,true,false);
					if(amount > 0)
						target->ChangeStat(Script::DEX,amount,true,true);
					break;
		case	2:	target->ChangeStat(Script::STR,amount,true,false);
					if(amount > 0)
						target->ChangeStat(Script::STR,amount,true,true);
					break;
		case	3:	target->ChangeStat(Script::INT,amount,true,false);
					if(amount > 0)
						target->ChangeStat(Script::INT,amount,true,true);
					break;
	}
	player->Send("Stat Changed.\n");
}

void	Admin::Alter(Player *player,Tokens *tokens,std::string& line)
{
	static const std::string	format_error("The format is: ADMIN ALTER RACE player new race\n");
	static const std::string	name_error("I can't find that name!\n");
	static const std::string	stat_error("You can only change a person's race at the moment!\n");

	if(tokens->Size() < 5)
	{
		player->Send(format_error);
		return;
	}

	if(tokens->Get(2) == "race")
	{
		std::string	player_name(tokens->Get(3));
		Player	*target = Game::player_index->FindName(Normalise(player_name));
		if(target == 0)
		{
			player->Send(name_error);
			return;
		}
		std::string	new_race(tokens->GetRestOfLine(line,4,Tokens::RAW));
		target->ChangeRace(new_race,player);
	}
	else
		player->Send(stat_error);
}

void	Admin::Change(Player *player,Tokens *tokens)
{
	if(tokens->Size() < 4)
	{
		player->Send("Type 'ADMIN HELP' to get the correct format!\n");
		return;
	}

	std::ostringstream	buffer;
	Player *target = Game::player_index->FindAccount(tokens->Get(3));
	if(target == 0)
	{
		buffer << "I can't find an account with the name '" << tokens->Get(3) << "'\n";
		player->Send(buffer);
		return;
	}

	if(tokens->Get(2) == "email")
	{
		target->UpdateEMail(tokens->Get(4));
		player->Send("Email address updated.\n");
		return;
	}

	if(tokens->Get(2) == "password")
	{
		target->UpdatePassword(tokens->Get(4));
		player->Send("Password updated.\n");
		return;
	}
}

void	Admin::DeleteVariable(Player *player,Tokens *tokens)
{
	static const std::string	result("Variable no longer set\n");

	if(tokens->Get(2) == "variable")
	{
		std::ostringstream	buffer("");
		std::string	name(tokens->Get(3));
		std::string	var(tokens->Get(4));
		Game::global_player_vars_table->Delete(Normalise(name),var);
		player->Send(result);
	}
}

void	Admin::DisplayFlags(Player *player,Tokens *tokens)
{
	if(tokens->Size() < 3)
		player->Send(Game::system->GetMessage("admin","displayflags",1));
	else
	{
		std::string	name(tokens->Get(2));
		Player	*target = Game::player_index->FindName(Normalise(name));
		if(target == 0)
			player->Send(Game::system->GetMessage("admin","displayflags",2));
		else
			target->AdminFlags(player);
	}
}

void	Admin::Founder(Player *player,Tokens *tokens)
{
	std::string	player_name(tokens->Get(2));
	Player	*target = Game::player_index->FindName(Normalise(player_name));
	if(target == 0)
	{
		player->Send("Can't find that player!\n");
		return;
	}
	if(target->Rank() != Player::COMMANDER)
	{
		player->Send("Player must be a commander!\n");
		return;
	}

	if(target->Jump2Founder(player))
		player->Send("Promoted!\n");
	else
		player->Send("Unable to process promotion!\n");
}

void	Admin::Indy(Player *player,Tokens *tokens)
{
	std::string	player_name(tokens->Get(2));
	Player	*target = Game::player_index->FindName(Normalise(player_name));
	if(target == 0)
	{
		player->Send("Can't find that player!\n");
		return;
	}

	if(target->Rank() != Player::COMMANDER)
	{
		player->Send("Player must be a commander!\n");
		return;
	}

	if(target->Jump2Indy(player))
		player->Send("Promoted!\n");
	else
		player->Send("Unable to process promotion!\n");
}

void	Admin::Help(Player *player)
{
	static const std::string	lines[] =
	{
		"Administration commands available:\n"
		"   ADMIN ADD CASH|COMPANY|SLITHY|TREASURY amount player|planet name\n",
		"   ADMIN ADD STAT statname amount playername\n",
		"   ADMIN ALTER RACE player new race\n",
		"   ADMIN CHANGE EMAIL account_ID new_address\n",
		"   ADMIN CHANGE PASSWORD account_ID new_password\n",
		"   ADMIN CLEAR flag player\n",
		"      flags are: host, manager, nav, techie, alpha, builds\n",
		"   ADMIN DELETE VARIABLE player variablename\n",
		"   ADMIN DISPLAY player\n",
		"   ADMIN DUMP (NOTE: For Alan's use only!)\n",
		"   ADMIN FOUNDER player\n",
		"   ADMIN INDY player\n",
		"   ADMIN MERCHANT player\n",
		"   ADMIN NOMATCH\n",
		"   ADMIN PROMOTE player\n",
		"   ADMIN REPORT account_ID\n",
		"   ADMIN SAVE player\n",
		"   ADMIN SET flag player\n",
		"      flags are: nav, alpha, host, techie, manager, sponsor, tester \n",
		"   ADMIN WHOELSE player\n",
		"   ADMIN XMLDUMP filename (NOTE: For Alan's use only!)\n",
		"   ADMIN ZOMBIE number_of_days\n",

		"Whois commands:\n",
		"   WHOIS ACCOUNT account_ID\n",
		"   WHOIS EMAIL email_address\n",
		"   WHOIS playername\n",

		"Management commands:\n",
		"   GAG player\n",
		"   UNGAG player\n",
		"   LOCK player\n",
		""
	};

	for(int count = 0;lines[count] != "";count++)
		player->Send(lines[count]);
}

void	Admin::Merchant(Player *player,Tokens *tokens)
{
	std::string	player_name(tokens->Get(2));
	Player	*target = Game::player_index->FindName(Normalise(player_name));
	if(target == 0)
	{
		player->Send("Can't find that player!\n");
		return;
	}
	if(target->Rank() != Player::COMMANDER)
	{
		player->Send("Player must be a commander!\n");
		return;
	}

	if(target->Jump2Merchant(player))
		player->Send("Promoted!\n");
	else
		player->Send("Unable to process promotion!\n");
}

void	Admin::Parse(Player *player,Tokens *tokens,std::string& line)
{
	static const std::string	admin_cmds[] =
	{
		"report", "change", "set", "clear", "save", "display", 		//  0- 5
		"delete", "add", "nomatch", "help", "promote", "whoelse",	//	 6-11
		"alter", "founder", "indy", "merchant", "dump",	"zombie",	//	12-17
		"xmldump",
		""
	};

	if(!((player->Name() == "Hazed") || (player->Name() == "Bella") || (player->Name() == "Freya")))
	{
		player->Send(Game::system->GetMessage("cmdparser","parse",1));
		return;
	}

	tokens->UpdateTokenize(line);
	int cmd = 9999;
	for(int count = 0;admin_cmds[count] != "";count++)
	{
		if(admin_cmds[count] == tokens->Get(1))
		{
			cmd = count;
			break;
		}
	}

	switch(cmd)
	{
		case	 0:	Game::player_index->DisplayAccount(player,tokens->Get(2));	break;
		case	 1:	Change(player,tokens);					break;
		case	 2:
		case	 3:	if(tokens->Size() < 4)
							player->Send(Game::system->GetMessage("cmdparser","admin",2));
						else
							Set(player,tokens,tokens->Get(3),tokens->Get(2),(cmd == 2) ? SET_FLAG : CLEAR_FLAG);
						break;
		case	 4:	SavePlayer(player,tokens);				break;
		case 	 5:	DisplayFlags(player,tokens);			break;
		case	 6:	DeleteVariable(player,tokens);		break;
		case	 7:	Add(player,tokens,line);				break;
		case	 8:	NoMatch::ListFailures(player);		break;
		case	 9:	Help(player);								break;
		case	10:	Promote(player,tokens);					break;
		case	11:	WhoElse(player,tokens);					break;
		case	12:	Alter(player,tokens,line);				break;
		case	13:	Founder(player,tokens);					break;
		case	14:	Indy(player,tokens);						break;
		case	15:	Merchant(player,tokens);				break;
		case	16:	player->Send("Not available\n");				break;
		case  17:	Zombie(player,tokens);					break;
		case  18:	Game::player_index->DumpAccounts(tokens->Get(2));						break;
		default:		player->Send(Game::system->GetMessage("cmdparser","admin",1));		break;
	}
}

void	Admin::Promote(Player *player,Tokens *tokens)
{
	static const std::string	ok("Now ready to promote.\n");
	static const std::string	promoted("Promoted!\n");

	std::string	player_name(tokens->Get(2));
	Player	*target = Game::player_index->FindName(Normalise(player_name));
	if(target == 0)
	{
		player->Send("Can't find that player!\n");
		return;
	}

	switch(target->Rank())
	{
		case Player::COMMANDER:			target->ClearLoan();
												player->Send(ok);
												break;
		case Player::CAPTAIN:			target->SetTP(1000);
												player->Send(ok);
												break;
		case Player::ADVENTURER:		target->SetCP(1000);
												player->Send("Player needs 500K in bank account...\n");
												break;
		case Player::MERCHANT:			target->SetTP(1000);
												player->Send("Player needs 600K in bank account...\n");
												break;
		case Player::TRADER:				target->SetTP(600);
												player->Send("Player needs several meg in bank account...\n");
												break;
		case Player::INDUSTRIALIST:	target->SetToManufacturer();
												player->Send(ok);
												break;
		case Player::MANUFACTURER:		target->SetToFinancier();
												player->Send(ok);
												break;
		case Player::FINANCIER:			target->Financier2Founder();
												player->Send(promoted);
												break;
		case Player::FOUNDER:			target->Founder2Engineer();
												player->Send(promoted);
												break;
		case Player::ENGINEER:			target->Engineer2Mogul();
												player->Send(promoted);
												break;
		case Player::MOGUL:				target->Mogul2Technocrat();
												player->Send(promoted);
												break;
	}

	Game::player_index->Save(target,PlayerIndex::NO_OBJECTS);
}

void	Admin::SavePlayer(Player *player,Tokens *tokens)
{
	if(tokens->Size() < 3)
		player->Send(Game::system->GetMessage("admin","saveplayer",1));
	else
	{
		std::string	name(tokens->Get(2));
		Player	*target = Game::player_index->FindName(Normalise(name));
		if(target == 0)
			player->Send(Game::system->GetMessage("admin","saveplayer",2));
		else
		{
			Game::player_index->Save(target,PlayerIndex::WITH_OBJECTS);
			std::ostringstream	buffer("");
			buffer << name << " saved out to disk.\n";
			player->Send(buffer);
		}
	}
}

void	Admin::Set(Player *player,Tokens *tokens,const std::string& name,const std::string& flag,int which)
{
	static const std::string	set_cmds[] =
	{
		"nav", "alpha", "host", "techie", "manager", "sponsor", "builds", "tester",	//  0 - 7
		""
	};

	std::string	player_name(name);
	Player	*target = Game::player_index->FindName(Normalise(player_name));
	if(target == 0)
		player->Send(Game::system->GetMessage("cmdparser","adminset",3));
	else
	{
		int	which_flag = 9999;
		for(int count = 0;set_cmds[count] != "";count++)
		{
			if( flag == set_cmds[count])
			{
				which_flag = count;
				break;
			}
		}

		switch(which_flag)
		{
			case 0:	SetNav(player,tokens,target,which);			break;		// 'nav'
			case 1:	SetAlpha(player,tokens,target,which);		break;		// 'alpha'
			case 2:	SetHost(player,tokens,target,which);		break;		// 'host'
			case 3:	SetTechie(player,tokens,target,which);		break;		// 'host'
			case 4: 	SetManager(player,tokens,target,which);	break;		// 'manager'
			case 5:	SetSponsor(player,tokens,target,which);	break;		// 'sponsor'
			case 6:	SetBuild(player,tokens,target,which);		break;		// 'build'
			case 7:	SetTester(player,tokens,target);				break;		// 'tester'
			default:	player->Send(Game::system->GetMessage("cmdparser","adminset",4));
		}
		Game::player_index->Save(target,PlayerIndex::NO_OBJECTS);
	}
}

void	Admin::SetAlpha(Player *player,Tokens *tokens,Player *target,int which)
{
	if(which == SET_FLAG)
	{
		target->SetGenFlag(Player::ALPHA_CREW);
		player->Send(Game::system->GetMessage("admin","setalpha",1));
	}
	else
	{
		target->ClearGenFlag(Player::ALPHA_CREW);
		player->Send(Game::system->GetMessage("admin","setalpha",2));
	}
}

void	Admin::SetBuild(Player *player,Tokens *tokens,Player *target,int which)
{
	if(which == SET_FLAG)
		target->AllowBuilds(player);
	else
		target->DisallowBuilds(player);
}

void	Admin::SetHost(Player *player,Tokens *tokens,Player *target,int which)
{
	if(which == SET_FLAG)
	{
		target->SetManFlag(Player::HOST_FLAG);
		player->Send(Game::system->GetMessage("admin","sethost",1));
	}
	else
	{
		target->ClearManFlag(Player::HOST_FLAG);
		player->Send(Game::system->GetMessage("admin","sethost",2));
	}
}

void	Admin::SetManager(Player *player,Tokens *tokens,Player *target,int which)
{
	static const std::string	set("Manager flag set for player\n");
	static const std::string	cleared("Manager flag cleared for player\n");
	if(which == SET_FLAG)
	{
		target->SetManFlag(Player::MANAGER);
		player->Send(set);
	}
	else
	{
		target->ClearManFlag(Player::MANAGER);
		player->Send(cleared);
	}
}

void	Admin::SetNav(Player *player,Tokens *tokens,Player *target,int which)
{
	if(which == SET_FLAG)
	{
		target->SetNavFlag(player);
		player->Send(Game::system->GetMessage("cmdparser","adminset",2));
	}
	else
	{
		target->ClearManFlag(Player::NAV_FLAG);
		player->Send(Game::system->GetMessage("cmdparser","adminset",1));
	}
}

void	Admin::SetSponsor(Player *player,Tokens *tokens,Player *target,int which)
{
	static const std::string	is_set("Sponsor flag set!\n");
 	static const std::string	is_clear("Sponsor flag cleared.\n");

	if(which == SET_FLAG)
	{
		target->SetGenFlag(Player::SPONSOR);
		player->Send(is_set);
	}
	else
	{
		target->ClearGenFlag(Player::SPONSOR);
		player->Send(is_clear);
	}
}

void	Admin::SetTechie(Player *player,Tokens *tokens,Player *target,int which)
{
	if(which == SET_FLAG)
	{
		target->SetManFlag(Player::TECHIE);
		player->Send(Game::system->GetMessage("admin","settechie",1));
	}
	else
	{
		target->ClearManFlag(Player::TECHIE);
		player->Send(Game::system->GetMessage("admin","settechie",2));
	}
}

void	Admin::SetTester(Player *player,Tokens *tokens,Player *target)
{
	target->SetTempFlag(Player::TESTER);
	player->Send("Tester flag set until the end of the current session.\n");
}

void	Admin::WhoElse(Player *player,Tokens *tokens)
{
	static const std::string	error1("I don't know who you want to check for alts.\n");
	static const std::string	error2("I can't find a player with that name in the game.\n");

	if(tokens->Size() < 3)
		player->Send(error1);
	else
	{
		std::string	name(tokens->Get(2));
		Player	*target = Game::player_index->FindName(Normalise(name));
		if(target == 0)
			player->Send(error2);
		else
		{
			std::string	ip(target->IPAddress());
			std::ostringstream	buffer;
			buffer << name << " (IP address "<< target->IPAddress();
			buffer << ") has the following characters in the game database:\n";
			player->Send(buffer);
			buffer.str("");
			if(Game::player_index->FindAllAlts(player,ip) == 0)
				player->Send("  None!\n");
		}
	}
}

void	Admin::Zombie(Player *player,Tokens *tokens)
{
	int days = 365;
	if(tokens->Size() > 2)
		days = std::atoi(tokens->Get(2).c_str());
	std::pair<int,int>	info = Game::player_index->NumberOutOfDate(days);
	std::ostringstream	buffer;
	buffer << "Number of zombie accounts over " << days << " days old is ";
	buffer << info.first << " of " << info.second << "\n";
	player->Send(buffer);
}

