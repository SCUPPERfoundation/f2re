/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef COMMODITYEXCHANGE_H
#define COMMODITYEXCHANGE_H

#include <fstream>
#include <map>
#include <sstream>
#include <string>

class CommodExchGraphs;
class Commodity;
class	CommodityExchItem;
class	FedMap;
class	Happenings;
class	Infrastructure;
class Player;
class	Tokens;

typedef	std::map<std::string,CommodityExchItem *,std::less<std::string> >	CommodIndex;

class	CommodityExchange
{
public:
	static const std::string	commod_names[];

private:
	static CommodExchGraphs	*graphs;
	static Happenings			*happenings;

	FedMap		*home_map;
	int			type;					// AGRI/BIO/TECH etc
	int			max_deficit;		// Don't let stock go below this level below min_
	int			max_surplus;
	int			cycles;				// accumulated cycles since last update
	CommodIndex::iterator display_iter;	// which commodity to display next
	CommodIndex	commod_index;		// commodities bought & sold here indexed by lower case name

	static CommodExchGraphs *LoadGraphs();

	CommodityExchItem	*Find(const std::string& commodity);
	void	CheckHappenings();

public:
	static int	GraphCalculateCost(int graph_type,int base_value,int stock,int max_stock);

	static void	CancelEventsReports(const std::string&	who_to);
	static void	DisplayEvents(Player *player,Tokens *tokens,const std::string& line);
	static void	ReportEvents(const std::string&	who_to);

	CommodityExchange(FedMap *fed_map,int exch_type);
	CommodityExchange(FedMap *fed_map,int exch_type,int deficit);
	~CommodityExchange()	{	}

	const std::string&	Riot();

	long	Buy(const std::string& commodity);
	long	Sell(const std::string& commodity);
	long	YardPurchase(const std::string& commodity,int amount,std::ostringstream& buffer,int action);

	int	GetProductivityBonus(CommodityExchItem *item);
	int	Value(const std::string& commodity);

	bool	AddCategoryConsumptionPoints(int econ_level,int quantity,bool permanent);
	bool	AddConsumptionPoint(Player *player,const std::string& commodity_name,int quantity);
	bool	AddProduction(Player *player,const std::string& commodity_name,int quantity);
	bool	AddProductionPoint(Player *player,const std::string& commodity_name,int quantity);
	bool	MaxStock(int level,const std::string commod_name);
	bool	MinStock(int level,const std::string commod_name);
	bool	SetSpread(int amount,const std::string commod_name);

	void	AddCommodity(CommodityExchItem *item);
	void	AdjustConsumerConsumption(int change);
	void	Buy(Player *player,const std::string& commodity);
	void	ChangeProduction(const Commodity *commodity,int amount);
	void	CheckCartelPrices(Player *player,const Commodity *commodity,
														const std::string& star_name,
														const std::string& planet_name,
														bool send_intro = true);
	void	CheckPrices(Player *player,const std::string& commodity,bool send_intro = true);
	void	CheckPrices(Player *player,const Commodity *commodity,bool send_intro = true);
	void	CheckGroupPrices(Player *player,int commod_grp);
	void	Display(Player *player,const std::string& commod_grp);
	void	DisplayProduction(Player *player,const std::string& commod_grp);
	void	Dump();
	void	MaxStock(int level);
	void	MinStock(int level);
	void	PremiumPriceCheck(Player *player,const Commodity *commodity,const std::string& name,
																						const std::string& star_name);
	void	PremiumPriceCheck(const Commodity *commodity,const std::string& name,
										const std::string& star_name,std::ostringstream& buffer,int which);
	void	RemotePriceCheck(Player *player,const Commodity *commodity,const std::string& name,
																						const std::string& star_name);
	void	Sell(Player *player,const std::string& commodity);
	void	SetSpread(int amount);
	void	SetExchangeEfficiency(const Infrastructure *infra);
	void	Update(int cycles, bool are_players);
	void	Write(std::ofstream&	file);
};

#endif
