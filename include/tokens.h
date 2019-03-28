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

#ifndef TOKENS_H
#define TOKENS_H

#include <string>
#include <utility>
#include <vector>

typedef	std::pair<int,std::string>	TokenRec;
typedef	std::vector<TokenRec>		TokenArray;

class	Tokens			// splits a string into tokens
{
public:
	static const int	MAX_TOKENS = 12;		// may need to up this default in due course
	static const int	NOT_FOUND;
	static const std::string	error;
	enum	{ RAW, LOWER_CASE, PLANET, COMPANY };		// return formats for GetRestOfLine()

private:
	TokenArray	token_array;

public:
	static void	NormaliseCompany(std::string& the_name);

	Tokens()		{ token_array.resize(MAX_TOKENS * 2); }
	~Tokens()	{	}
	const std::string& 	operator[](unsigned index) const;

	const TokenRec& 		GetRec(unsigned index) const;

	const std::string& 	Get(unsigned index) const;
	const std::string&	GetRestOfLine(const std::string& line,unsigned start,int format) const;

	int	FindIndex(const std::string& the_word);	// returns NOT_FOUND if word not found
	int	GetSignedNumber(unsigned index,const std::string& line) const;
	int 	GetStart(unsigned index) const;

	unsigned	Size()		{ return(token_array.size());	}

	void	Dump();
	void	Tokenize(const std::string line,unsigned max_tokens = MAX_TOKENS);
	void	UpdateTokenize(const std::string line,unsigned max_tokens = MAX_TOKENS);
};

#endif



