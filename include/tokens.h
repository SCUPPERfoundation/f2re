/*----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-9
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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



