/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef COMMODITIES_H
#define COMMODITIES_H

#include <bitset>
#include <map>
#include <string>


class Commodity;

typedef std::map<std::string,Commodity *,std::less<std::string> >	CommodityIndex;

class	Commodities
{
public:
	static const std::string	defence_names[];
	static const std::string	consumer_names[];
	static const int	MAX_INPUTS = 6;
	static const int	UNKNOWN_TYPE;

	enum	{ AGRI, RES, IND, TECH, BIO, LEIS, MAX_TYPES };
	enum
	{
		C_AGRI, 		C_RES,		C_IND, 		C_TECH, 		C_BIO,	 	C_LEIS,		C_BULK,
		C_CONS,		C_DEFENCE,	C_ENERGY,	C_SEA,		C_ALL,		MAX_C_TYPES
	};

private:
	CommodityIndex		commodity_index;

	void	Write();

public:
	static std::string&	Normalise(const std::string& the_name);

	static int	Group2Type(const std::string& the_name);
	static bool	IsConsumerType(const std::string& name);
	static bool	IsDefenceType(const std::string& name);
	static void	Translate(std::string& name);

	Commodities()	{	}
	~Commodities();

	const std::string&	GetRandomCommodity();
	const std::string&	Name(const std::string& the_name) const;

	const Commodity	*Find(const std::string& name);

	int	Commod2Type(const std::string& the_name);
	int	Cost(const std::string& the_name);
	int	Graph(const std::string& the_name);
	int	MaxCommod()										{ return(commodity_index.size());	}

	bool	IsCommodity(const std::string name);
	bool	IsType(const std::string name,int type);

	void	AddCommodity(Commodity *commodity);
	void	Dump();
	void	SwitchRandomGraph();
};

struct InputRec
{
	std::string	name;										// input commodity name
	int	quantity;										// input quantity
};

struct Commodity
{
	std::string	name;										// real name of the commodity
	int	type;												// eg AGRI, TECH, etc.
	int	cost;												// base cost/ton
	int	graph;											// exchange supply/demand graph
	std::bitset<Commodities::MAX_C_TYPES>	type_flags;	// eg AGRI, BULK, etc

	//	Inputs for production of this commodity
	int	labour;											// labour need for full output
	InputRec	inputs[Commodities::MAX_INPUTS];		// type and quantity of input needed
};

#endif
