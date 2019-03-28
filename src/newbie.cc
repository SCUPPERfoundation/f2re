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

#include "newbie.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include <cctype>
#include <cstdlib>
#include <cstring>

#include <unistd.h>

#include "cmd_parser.h"
#include "forbidden.h"
#include "login.h"
#include "misc.h"
#include "player.h"
#include "player_index.h"


void	Newbie::DisplayAccept(Player *player)
{
	std::ostringstream	buffer("");
	buffer << std::endl << "Name: " << player->name << std::endl;
	buffer << "  Race:   " << player->race << std::endl;
	buffer << "  Gender: " << Player::gender_str1[player->gender] << std::endl;
	buffer << "  Stats:" << std::endl;
	buffer << "    Strength:     " << std::setw(2) << player->strength[Player::MAXIMUM] << '/';
	buffer << std::setw(2) << player->strength[Player::CURRENT] << std::endl;
	buffer << "    Stamina:      " << std::setw(2) << player->stamina[Player::MAXIMUM] << '/';
	buffer << std::setw(2) << player->stamina[Player::CURRENT] << std::endl;
	buffer << "    Dexterity:    " << std::setw(2) << player->dexterity[Player::MAXIMUM] << '/';
	buffer << std::setw(2) << player->dexterity[Player::CURRENT] << std::endl;
	buffer << "    Intelligence: " << std::setw(2) << player->intelligence[Player::MAXIMUM] << '/';
	buffer << std::setw(2) << player->intelligence[Player::CURRENT] << std::endl;
	write(player->sd,buffer.str().c_str(),buffer.str().length());
	buffer.str("");
	buffer << std::endl << "Do you want to play using this character? [yes/no]:\n";
	write(player->sd,buffer.str().c_str(),buffer.str().length());
}

Player	*Newbie::Find(int sd)
{
	DescIndex::iterator	iter = desc_index.find(sd);
	if(iter != desc_index.end())
		return(iter->second);
	else
		return(0);
}

bool	Newbie::IsANewbie(int sd)
{
	return(Find(sd) != 0);
}

void	Newbie::LostLine(int sd)
{
	DescIndex::iterator iter = desc_index.find(sd);
	if(iter != desc_index.end())
	{
		Player	*rec = iter->second;
		desc_index.erase(iter);
		delete rec;
	}
}

int	Newbie::NewPlayer(LoginRec *rec)
{
	static const std::string	reply_to("fi@ibgames.com");
	static const std::string	subject("\"Welcome to Federation 2\"");
	static const std::string	filename("welcome.dat");

	std::string	start("\n\nWelcome to Federation II dataspace. Please provide a name for your \
character. (Minimum 3, maximum 15 letters):\n");

	Player	*player = new Player(rec);
	desc_index[rec->sd] = player;
	write(rec->sd,start.c_str(),start.length());
	return(Player::START);
}

bool	Newbie::ProcessAccept(int sd,std::string& text,Player *player)
{
	std::string	start("\n\nPlease provide a name for your character. (Mimimum 3, maximum 15 letters):\n");
	std::string name_in_use("\n\nI'm sorry that name is not available. Please try again with a different name.\n");

	std::string	line;
	InputBuffer(player->input_buffer,text,line);
	int len = line.length();
	if(len > 0)
	{
		player->status_flags.reset();
		if((line[0] == 'y') || (line[0] == 'Y'))
		{
			Player *avatar = Game::player_index->FindName(player->name);
			bool is_in_vocab = Game::parser->IsInVocab(player->name);
			bool	is_forbidden = Game::forbidden->IsForbidden(player->name);
			if((avatar != 0) || is_in_vocab || is_forbidden)
			{
				write(sd,name_in_use.c_str(),name_in_use.length());
				write(sd,start.c_str(),start.length());
				player->status_flags.set(Player::START);
			}
			else
			{
				DescIndex::iterator iter = desc_index.find(player->sd);
				if(iter != desc_index.end())
					desc_index.erase(iter);
				Game::player_index->NewPlayer(player);
				std::ostringstream	buffer;
				buffer << "Groundhog " << player->name << " has started playing.";
				WriteLog(buffer);
			}
		}
		else
		{
			write(sd,start.c_str(),start.length());
			player->status_flags.set(Player::START);
		}
	}
	return(true);
}

bool	Newbie::ProcessDexterity(int sd,std::string& text,Player *player)
{
	std::string	line;
	InputBuffer(player->input_buffer,text,line);
	int len = line.length();
	if(len > 0)
	{
		int	remainder = 140 - player->strength[Player::CURRENT] - player->stamina[Player::CURRENT];
		int 	max_dex = remainder - 20;
		if(max_dex > 70)	max_dex = 70;
		int 	dex = std::atoi(line.c_str());
		if(dex > max_dex)	dex = max_dex;
		if(dex < 20)		dex = 20;
		player->dexterity[Player::MAXIMUM] = player->dexterity[Player::CURRENT] = dex;
		int	intel = remainder - dex;
		if(intel > 70)		intel = 70;
		player->intelligence[Player::MAXIMUM] = player->intelligence[Player::CURRENT] = intel;
		DisplayAccept(player);
		player->status_flags.reset();
		player->status_flags.set(Player::ACCEPT);
	}
	return(true);
}

bool	Newbie::ProcessGender(int sd,std::string& text,Player *player)
{
	std::string	stats("\n\nYour character has four stats - strength, stamina, dexterity and \
intelligence. You have 140 stat points to distribute between the four \
stats. You can either opt to distribute them equally between the stats \
(35 points to each) or you can choose how to distribute them yourself.\n\
Distribute the stats equally? [yes/no]:\n");

	std::string	line;
	InputBuffer(player->input_buffer,text,line);
	int len = line.length();
	if(len > 0)
	{
		if((line[0] == 'm') || (line[0] == 'M'))
			player->gender = Player::MALE;
		else
		{
			if((line[0] == 'n') || (line[0] == 'N'))
				player->gender = Player::NEUTER;
			else
				player->gender = Player::FEMALE;
		}
		write(sd,stats.c_str(),stats.length());
		player->status_flags.reset();
		player->status_flags.set(Player::STATS);
	}
	return(true);
}

bool	Newbie::ProcessInput(int sd,std::string& text)
{
	Player	*player = Find(sd);
	if(player == 0)
		return(true);

	if(player->status_flags.test(Player::START))		return(ProcessName(sd,text,player));
	if(player->status_flags.test(Player::RACE))		return(ProcessRace(sd,text,player));
	if(player->status_flags.test(Player::GENDER))	return(ProcessGender(sd,text,player));
	if(player->status_flags.test(Player::STATS))		return(ProcessStats(sd,text,player));
	if(player->status_flags.test(Player::STR))		return(ProcessStrength(sd,text,player));
	if(player->status_flags.test(Player::STA))		return(ProcessStamina(sd,text,player));
	if(player->status_flags.test(Player::DEX))		return(ProcessDexterity(sd,text,player));
	if(player->status_flags.test(Player::ACCEPT))	return(ProcessAccept(sd,text,player));

	return(true);
}

bool	Newbie::ProcessName(int sd,std::string& text,Player *player)
{
	std::string	race("\n\nPlease choose the race of your character. For example, human, cyborg, \
vulcan, droid, grue, or any other type that you care to invent. (Minimum 4, maximum 15 letters):\n");
	std::string	wrong("\n\nThe name for your character must be not more than 15 letters and less \
than three. Please enter a name:\n");

	std::string	line;
	InputBuffer(player->input_buffer,text,line);
	int len = line.length();
	if(len > 0)
	{
		if((len >= 3) && (len <= 15))
		{
			player->name = line;
			int len = player->name.length();
			for(int count = 0;count < len;count++)
			{
				if((!std::isalpha(player->name[count])) || (player->name[count] == '-'))
					player->name[count] = ' ';
			}
			std::string::iterator iter = std::remove(player->name.begin(),player->name.end(),' ');
			player->name.erase(iter,player->name.end());
			len = player->name.length();
			if(len < 3)
			{
				write(sd,wrong.c_str(),wrong.length());
				return(true);
			}

			player->name[0] = std::toupper(player->name[0]);
			for(int count = 1;count < len;count++)	// can't get std::transform() working properly
				player->name[count] = std::tolower(player->name[count]);
			std::ostringstream	buffer("");
			buffer << player->name << " is dressed in a nondescript grey coverall, like most of the rest of the population.";
			player->desc =  buffer.str();
			write(sd,race.c_str(),race.length());
			player->status_flags.reset();
			player->status_flags.set(Player::RACE);
		}
		else
			write(sd,wrong.c_str(),wrong.length());
	}
	return(true);
}

bool	Newbie::ProcessRace(int sd,std::string& text,Player *player)
{
	std::string	gender("\n\nPlease choose the gender of your character. [male/female/neuter]:\n");
	std::string	wrong("\n\nThe race for your character must be not more than 15 letters or less \
than four. Please enter a race:\n");

	std::string	line;
	InputBuffer(player->input_buffer,text,line);
	int len = line.length();
	if(len > 0)
	{
		if((len >= 3) && (len <= 15))
		{
			player->race = line;
			int len = player->race.length();
			for(int count = 0;count < len;count++)
			{
				if(!std::isalnum(player->race[count]))
					player->race[count] = ' ';
			}
			for(int count = 0;count < len;count++)	// can't get std::transform() working properly
				player->race[count] = std::tolower(player->race[count]);
			write(sd,gender.c_str(),gender.length());
			player->status_flags.reset();
			player->status_flags.set(Player::GENDER);
		}
		else
			write(sd,wrong.c_str(),wrong.length());
	}
	return(true);
}

bool	Newbie::ProcessStamina(int sd,std::string& text,Player *player)
{
	std::string	line;
	InputBuffer(player->input_buffer,text,line);
	int len = line.length();
	if(len > 0)
	{
		int	remainder = 140 - player->strength[Player::CURRENT];
		int 	max_sta = remainder - 40;
		if(max_sta > 70)	max_sta = 70;
		int 	sta = std::atoi(line.c_str());
		if(sta > max_sta)	sta = max_sta;
		if(sta < 20)		sta = 20;
		player->stamina[Player::MAXIMUM] = player->stamina[Player::CURRENT] = sta;
		player->status_flags.reset();
		player->status_flags.set(Player::DEX);
		remainder -= sta;
		int 	max_dex = remainder - 20;
		if(max_dex > 70)	max_dex = 70;
		std::ostringstream	buffer("");
		buffer << std::endl << "You have " << remainder << " stat points left. ";
		buffer << "How many points do you want to allocate to dexterity? (Minimum 20, maximum ";
		buffer << max_dex << "):\n";
		write(player->sd,buffer.str().c_str(),buffer.str().length());
	}
	return(true);
}

bool	Newbie::ProcessStats(int sd,std::string& text,Player *player)
{
	std::string	strength("\n\nYou have 140 stat points available. How many points do you want to \
allocate to strength? (Minimum 20, maximum 70):\n");

	std::string	line;
	InputBuffer(player->input_buffer,text,line);
	int len = line.length();
	if(len > 0)
	{
		if((line[0] == 'y') || (line[0] == 'Y'))
		{
			player->strength[Player::MAXIMUM]     = player->strength[Player::CURRENT]     = 35;
			player->stamina[Player::MAXIMUM]      = player->stamina[Player::CURRENT]      = 35;
			player->dexterity[Player::MAXIMUM]    = player->dexterity[Player::CURRENT]    = 35;
			player->intelligence[Player::MAXIMUM] = player->intelligence[Player::CURRENT] = 35;
			DisplayAccept(player);
			player->status_flags.reset();
			player->status_flags.set(Player::ACCEPT);
		}
		else
		{
			write(sd,strength.c_str(),strength.length());
			player->status_flags.reset();
			player->status_flags.set(Player::STR);
		}
	}
	return(true);
}

bool	Newbie::ProcessStrength(int sd,std::string& text,Player *player)
{
	std::string	line;
	InputBuffer(player->input_buffer,text,line);
	int len = line.length();
	if(len > 0)
	{
		int 	str = std::atoi(line.c_str());
		if(str > 70)	str = 70;
		if(str < 20)	str = 20;
		player->strength[Player::MAXIMUM] = player->strength[Player::CURRENT] = str;
		player->status_flags.reset();
		player->status_flags.set(Player::STA);
		int	remainder = 140 - str;
		int 	max_sta = remainder - 40;
		if(max_sta > 70)	max_sta = 70;
		std::ostringstream	buffer("");
		buffer << std::endl << "You have " << remainder << " stat points left. ";
		buffer << "How many points do you want to allocate to stamina? (Minimum 20, maximum ";
		buffer << max_sta << "):\n";
		write(player->sd,buffer.str().c_str(),buffer.str().length());
	}
	return(true);
}

