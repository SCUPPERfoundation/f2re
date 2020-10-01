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

#include "tokens.h"

#include <iostream>

#include <cctype>
#include <cstdlib>

#include "company.h"
#include "misc.h"

const int Tokens::MAX_TOKENS;
const int Tokens::NOT_FOUND = -1;
const std::string	Tokens::error("Index out of bounds!");

const std::string& Tokens::operator[](unsigned index) const
{
	if(index >= token_array.size())
		return(error);

	return(token_array[index].second);
}


void	Tokens::Dump()
{
	std::cout << "Contents of Tokens::token_array:" <<std::endl;
	for(TokenArray::iterator iter = token_array.begin();iter != token_array.end();++iter)
		std::cout << "  " << iter->second << std::endl;
	std::cout << std::endl;
}

int	Tokens::FindIndex(const std::string& the_word)
{
	int index = 0;
	for(TokenArray::iterator iter = token_array.begin();iter != token_array.end();++iter,++index)
	{
		if(iter->second == the_word)
			return(index);
	}
	return(-1);
}

const std::string& Tokens::Get(unsigned index) const
{
	if(index >= token_array.size())
		return(error);

	return(token_array[index].second);
}

const TokenRec& Tokens::GetRec(unsigned index) const
{
	static const TokenRec		error_token(-1,error);
	if(index >= token_array.size())
		return(error_token);

	return(token_array[index]);
}

const std::string&	Tokens::GetRestOfLine(const std::string& line,unsigned start,int format) const
{
	static std::string	rest_of_line;
	int	index =  GetStart(start);
	if((index == NOT_FOUND) || (static_cast<unsigned>(index) >= line.length()))
		return(error);
	else
	{
		rest_of_line = line.substr(index);
		if(format == LOWER_CASE)
		{
			unsigned len = rest_of_line.length();
			for(unsigned count = 0;count < len;count++)
				rest_of_line[count] = std::tolower(rest_of_line[count]);
		}
		if(format == PLANET)
			NormalisePlanetTitle(rest_of_line);
		if(format == COMPANY)
			NormaliseCompany(rest_of_line);
		return(rest_of_line);
	}
}

int	Tokens::GetSignedNumber(unsigned index,const std::string& line) const
{
	int number = std::atoi(Get(index).c_str());
	int to_check = token_array[index].first -1;
	if(line[to_check] == '-')
		return(-number);
	else
		return(number);
}

int	Tokens::GetStart(unsigned index) const
{
	if(index >= token_array.size())
		return(NOT_FOUND);

	return(token_array[index].first);
}

void	Tokens::NormaliseCompany(std::string& the_name)
{
	int	len = the_name.length();
	for(int count = 0;count < len;count++)
	{
		if((count == 0) || (the_name[count - 1] == ' '))
			the_name[count] = std::toupper(the_name[count]);
		else
			the_name[count] = std::tolower(the_name[count]);
	}
}

void	Tokens::Tokenize(const std::string line,unsigned max_tokens)
{
	std::string	text(line);
	text += " ";

	token_array.clear();
	int len = text.length();
	int next_start = 0;
	for(int count = 0;count < len;count++)
	{
		if((ispunct(text[count]) != 0) || (isspace(text[count]) != 0))
		{
			if(next_start == count)
				next_start++;
			else
			{
				std::pair<int,std::string> rec(next_start,text.substr(next_start,count - next_start));
				token_array.push_back(rec);
				if(token_array.size() >= max_tokens)
					break;
				next_start = count + 1;
			}
		}
	}

	for(TokenArray::iterator iter= token_array.begin();iter != token_array.end();iter++)
	{
		int	len = iter->second.length();
		for(int count = 0;count < len;count++)
			iter->second[count] = tolower(iter->second[count]);
	}
}

void	Tokens::UpdateTokenize(const std::string line,unsigned max_tokens)
{
	std::string	text(line);
	text += " ";

	token_array.clear();
	int len = text.length();
	int next_start = 0;
	for(int count = 0;count < len;count++)
	{
		if(isspace(text[count]) != 0)
		{
			if(next_start == count)
				next_start++;
			else
			{
				std::pair<int,std::string> rec(next_start,text.substr(next_start,count - next_start));
				token_array.push_back(rec);
				if(token_array.size() >= max_tokens)
					break;
				next_start = count + 1;
			}
		}
	}

	for(TokenArray::iterator iter = token_array.begin();iter != token_array.end();iter++)
	{
		int	len = iter->second.length();
		for(int count = 0;count < len;count++)
			iter->second[count] = tolower(iter->second[count]);
	}
}

