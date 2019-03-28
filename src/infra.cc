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

#include "infra.h"

#include <sstream>

#include <cstdlib>

#include "build_agri.h"
#include "build_airlane.h"
#include "build_airport.h"
#include "build_anti_agathics.h"
#include "build_antigrav.h"
#include "build_atmos.h"
#include "build_base.h"
#include "build_biodiv.h"
#include "build_biolab.h"
#include "build_canal.h"
#include "build_clinic.h"
#include "build_coal.h"
#include "build_defence.h"
#include "build_dole.h"
#include "build_family.h"
#include "build_fibre.h"
#include "build_floating.h"
#include "build_fusion.h"
#include "build_genetic.h"
#include "build_helio.h"
#include "build_hospital.h"
#include	"build_housing.h"
#include "build_informer.h"
#include "build_insulation.h"
#include "build_insurance.h"
#include "build_leisure.h"
#include "build_metastudio.h"
#include "build_mining.h"
#include "build_oil.h"
#include "output_filter.h"
#include "build_pension.h"
#include "build_phone.h"
#include "build_police.h"
#include "build_pollution.h"
#include "build_port.h"
#include "build_radio.h"
#include "build_railway.h"
#include "build_research.h"
#include "build_riot_police.h"
#include "build_satellite.h"
#include "build_school.h"
#include "build_solar.h"
#include "build_surveillance.h"
#include "build_tachyon.h"
#include "build_tech.h"
#include "build_telegraph.h"
#include "build_terraform.h"
#include "build_tquark.h"
#include "build_urban.h"
#include "build_weather.h"

#include "business.h"
#include "cartel.h"
#include "commodity_exchange.h"
#include "company.h"
#include "comp_register.h"
#include "depot.h"
#include "disaffection.h"
#include "efficiency.h"
#include "factory.h"
#include "fedmap.h"
#include "galaxy.h"
#include "mail.h"
#include "misc.h"
#include "player.h"
#include "population.h"
#include "riots.h"
#include "review.h"
#include "star.h"
#include "syndicate.h"
#include "warehouse.h"
#include "xml_parser.h"


const std::string	 Infrastructure::econ_names[] =
{
	"None", "Agricultural", "Resource", "Industrial",
	"Technical", "Biological", "Leisure", ""
};

// We can put the definition here, because the function is private to the class
template <typename T>
inline bool	Infrastructure::IncEnhancement(Player *player,Tokens *tokens,const std::string& name)
{
	switch(AddEnhancement(player,tokens,name))
	{
		case ENH_ADDED:	return(true);
		case ENH_FAILED:	return(false);
	}

	T *build = new T(home,player,tokens);
	return(BuildEnhancement(player,build));
}


Infrastructure::Infrastructure(FedMap *fed_map,const char **attrib)
{
	home = fed_map;

	const std::string	*owner_str = XMLParser::FindAttrib(attrib,"owner");
	if(owner_str != 0)
		owner_name = *owner_str;

	const std::string	*econ_str = XMLParser::FindAttrib(attrib,"economy");
	if(econ_str != 0)
		economy = Infrastructure::FindEconomyType(*econ_str);
	else
		economy = 0;

	treasury = XMLParser::FindLongAttrib(attrib,"treasury",0L);

	if((treasury > 1000000000L) || (treasury < -1500000000L))
		treasury = 1000000000L;

	workers = total_workers = XMLParser::FindNumAttrib(attrib,"workers",1000);
	casualties = XMLParser::FindNumAttrib(attrib,"casualties",0);
	if(casualties > 0)
		casualties--;
	yard_markup = XMLParser::FindNumAttrib(attrib,"yard-markup",0);
	disaffection = 0;
	fleet_size = 0;
	promote = XMLParser::FindNumAttrib(attrib,"promote",-1);

	if(home->Title().find(" Space") != std::string::npos)
	{
		const std::string	*closed_str = XMLParser::FindAttrib(attrib,"closed");
		if((closed_str != 0) && (*closed_str == "true"))
			status.set(CLOSED);
	}

	const std::string	*reg_str = XMLParser::FindAttrib(attrib,"registry");
	if((reg_str != 0) && (*reg_str == "true"))
		flags.set(REGISTRY);
	slithy_xform = XMLParser::FindNumAttrib(attrib,"slithy-xform",1) - 1;

	int base_pop = XMLParser::FindNumAttrib(attrib,"base",1000);
	population = new Population(this,base_pop);

	discontent = new Disaffection(this);
	efficiency = new Efficiency(this);

 	riots = 0;
}

Infrastructure::Infrastructure(FedMap *fed_map,const std::string& name,
										long cash,int yard,int econ,bool closed,bool reg)
{
	home = fed_map;
	owner_name = name;
	workers = total_workers = 1000;
	economy = econ;
	yard_markup = yard;
	treasury = cash;

	fleet_size = 0;
	if(closed)	status.set(CLOSED);
	if(reg)		flags.set(REGISTRY);

	discontent = new Disaffection(this);
	efficiency = new Efficiency(this);
	population = new Population(this);
	riots = 0;
	slithy_xform = 0;
}

Infrastructure::~Infrastructure()
{
	for(WarehouseList::iterator iter = warehouse_list.begin();iter != warehouse_list.end();iter++)
		delete iter->second;
	delete discontent;
	delete efficiency;
	delete population;
	delete riots;

/****************** delete built enhancements here ***************/

}


void	Infrastructure::AddDepot(Depot *depot,const std::string& name)
{
	depot_list[name] = depot;
}

int	Infrastructure::AddEnhancement(Player *player,Tokens *tokens,const std::string& name)
{
std::ostringstream	buffer;
	for(EnhanceList::iterator iter = enhancements.begin();iter != enhancements.end();iter++)
	{
		if((*iter)->Name() == name)
		{
			if((*iter)->Add(player,tokens))
			{
				static const std::string	cash("Your treasury has paid out a total of 10,000,000ig to contractors.\n");
				treasury -= 10000000L;
				player->Send(cash);
				home->SaveCommodityExchange();
				home->SaveInfrastructure();
				return(ENH_ADDED);
			}
			else
				return(ENH_FAILED);
		}
	}
	return(ENH_NONE);
}

void	Infrastructure::AddFactory(Factory *factory,bool to_notify)
{
	factories.push_back(factory);
	if(to_notify)
	{
		const PlayerList& pl_list = home->PlayersOnMap();
		if(pl_list.empty())
			return;

		AttribList	attribs;
		std::pair<std::string,std::string> attrib(std::make_pair("output",factory->Output()));
		attribs.push_back(attrib);

		for(PlayerList::const_iterator iter = pl_list.begin();iter != pl_list.end();++iter)
			(*iter)->Send("",OutputFilter::ADD_FACTORY,attribs);
	}
	AddLabour(-factory->LabourHired());
}

void	Infrastructure::AddLabour(int num_workers)
{
	workers += num_workers;
	std::ostringstream	buffer;
	buffer << workers;

	AttribList attribs;
	std::pair<std::string,std::string> attrib(std::make_pair("workers",buffer.str()));
	attribs.push_back(attrib);

	const PlayerList&	pl_list = home->PlayersOnMap();
	for(PlayerList::const_iterator iter = pl_list.begin();iter != pl_list.end();++iter)
		(*iter)->Send("",OutputFilter::UPDATE_WORKERS,attribs);
}

void	Infrastructure::AddWarehouse(Warehouse *warehouse,const std::string& name)
{
	Player *player = Game::player_index->FindName(name);
	if(player != 0)
	{
		std::ostringstream	buffer;
		buffer << home->HomeStar() << "." << home->Title();
		player->AddWarehouse(buffer.str());
		warehouse_list[name] = warehouse;
	}
}

int	Infrastructure::AssignWorkers(int num_workers)
{
	if(num_workers > workers)
		num_workers = workers;
	int	current_population = population->GetBasePopulation();
	if((current_population - num_workers) < 100)
		num_workers = current_population - 100;
	if(num_workers <= 0)
		return(0);

	population->SetBasePopulation(current_population - num_workers);
	total_workers -= num_workers;
	workers -= num_workers;
	return(num_workers);
}

int	Infrastructure::BasePopulation()
{
	return(population->GetBasePopulation());
}

bool	Infrastructure::BuildEnhancement(Player *player,Enhancement *build)
{
	if(!build->IsOK())
	{
		player->Send("I'm sorry, I don't seem to be able to build that at the moment!\n");
		delete build;
		return(false);
	}
	else
	{
		AddEnhancement(build);
		static const std::string	cash("Your treasury has paid out a total of 10,000,000ig to contractors.\n");
		treasury -= 10000000L;
		player->Send(cash);
		home->SaveCommodityExchange();
		home->SaveInfrastructure();
		return(true);
	}
}

void	Infrastructure::BuildRiot()
{
	Player	*player = Game::player_index->FindName(owner_name);
	if(riots == 0)
		riots = new Riots(home);

	int num_builds = enhancements.size();
	if(num_builds > 0)
	{
		int which = rand() % num_builds;
		EnhanceList::iterator	iter;
		int	count;
		for(count = 0,iter = enhancements.begin();iter != enhancements.end();iter++,count++)
		{
			if(count == which)
				break;
		}
		if(iter != enhancements.end())
		{
			bool do_delete = (*iter)->Riot();
			Enhancement	*build = *iter;
			riots->ReportBuildDamage(player,build->Name());
			std::ostringstream	buffer;
			buffer << home->Title() << " has lost a " << build->Name() << " to rioting!";
			WriteLog(buffer);

			if(do_delete)
			{
				enhancements.erase(iter);
				delete build;
			}
		}
	}

	ExchangeRiot();
}

int	Infrastructure::CalculateDisaffection()
{
	if(home->CartelName() == home->HomeStar())
	{
		Cartel	*cartel = Game::syndicate->Find(home->CartelName());
		if((cartel != 0) && cartel->IsBuildingACity())
		{
			disaffection = 0;
			return 0;
		}
	}
		for(EnhanceList::iterator iter = enhancements.begin();iter != enhancements.end();iter++)
			(*iter)->UpdateDisaffection(discontent);
	return(discontent->Update());
}

void	Infrastructure::CalculateEfficiency()
{
	for(EnhanceList::iterator iter = enhancements.begin();iter != enhancements.end();iter++)
		(*iter)->UpdateEfficiency(efficiency);
	efficiency->Update();
	home->SetExchangeEfficiency();
}

void	Infrastructure::CalculatePopulationAndWorkers()
{
	for(EnhanceList::iterator iter = enhancements.begin();iter != enhancements.end();iter++)
		(*iter)->UpdatePopulation(population);
	for(FactoryList::iterator iter = factories.begin();iter != factories.end();iter++)
		population->FactoryWorkersEmployed((*iter)->LabourHired());

	total_workers = population->UpdatePopulation();
	workers = population->UpdateWorkers();
}

bool	Infrastructure::CheckForRioting()
{
	if(disaffection > (rand() % 100))
		return(true);
	else
		return(false);
}

void	Infrastructure::CheckTreasuryOverflow(std::string& planet_title)
{
	if(treasury > 2000000000L)	// There's more than likely been an overflow!
	{
		treasury = 2000000000L;
		std::ostringstream	buffer;
		buffer << "Treasury overflow fixed on planet " << planet_title;
		WriteLog(buffer);
	}
}

void	Infrastructure::Close(Player *player)
{
	const static std::string	error("Only owners of planets can close them to visitors!\n");

	if((player->Name() == owner_name) || player->IsManager())
	{
		status.set(CLOSED);
		std::ostringstream	buffer;
		buffer << home->Title() << " is now closed to visitors.\n";
		player->Send(buffer);
		Game::review->Post(buffer);
	}
	else
		player->Send(error);
}

void	Infrastructure::Consolidate(Company *company)
{
	static const std::string	error("I can't find a depot belonging to your company.\n");

	Depot	*depot = FindDepot(company->Name());
	if(depot == 0)
		company->CEO()->Send(error);
	else
		depot->Consolidate(company->CEO());
}

void	Infrastructure::Consolidate(Player *player)
{
	static const std::string	error("I can't find a warehouse belonging to you.\n");

	Warehouse	*warehouse = FindWarehouse(player->Name());
	if(warehouse == 0)
		player->Send(error);
	else
		warehouse->Consolidate(player);
}

void	Infrastructure::Consolidate(Business *business)
{
	static const std::string	error("I can't find a depot belonging to your business.\n");

	Depot	*depot = FindDepot(business->Name());
	if(depot == 0)
		business->CEO()->Send(error);
	else
		depot->Consolidate(business->CEO());
}

bool	Infrastructure::DeleteDepot(const std::string& co_name)
{
	DepotList::iterator iter = depot_list.find(co_name);
	if(iter != depot_list.end())
	{
		Depot	*depot = iter->second;
		depot_list.erase(iter);
		delete depot;
		return(true);
	}
	else
		return(false);
}

bool	Infrastructure::DeleteFactory(Factory *factory)
{
	for(FactoryList::iterator iter = factories.begin();iter != factories.end();iter++)
	{
		if((*iter) == factory)
		{
			factories.erase(iter);
			AddLabour(factory->LabourHired());

			const PlayerList& pl_list = home->PlayersOnMap();
			if(!pl_list.empty())
			{
				AttribList	attribs;
				std::pair<std::string,std::string> attrib(std::make_pair("output",factory->Output()));
				attribs.push_back(attrib);

				for(PlayerList::const_iterator iter = pl_list.begin();iter != pl_list.end();++iter)
					(*iter)->Send("",OutputFilter::REMOVE_FACTORY,attribs);
			}

			delete factory;
			return(true);
		}
	}
	return(false);
}

void	Infrastructure::Demolish(Player *player,const std::string&  building)
{
	static const std::string	no_demolish("I can't find one of those on this planet!\n");

	for(EnhanceList::iterator iter = enhancements.begin();iter != enhancements.end();++iter)
	{
		if((*iter)->Name() == building)
		{
			if((*iter)->Demolish(player))
			{
				int refund = 3000000 + (std::rand() % 4000000);
				treasury += refund;

				if((*iter)->TotalBuilds() == 0)
				{
					Enhancement	*temp = *iter;
					enhancements.erase(iter);
					delete temp;
				}

				std::ostringstream	buffer;
				buffer << "You demolish the enhancement and realize a total of " << refund;
				buffer << "ig from the sale of salvaged components and assets.\n";
				player->Send(buffer);
			}
			return;
		}
	}
	player->Send(no_demolish);
}

void	Infrastructure::DepotRiot()
{
	if(riots == 0)
		riots = new Riots(home);

	Player	*founder = Game::player_index->FindName(Owner());
	if(founder == 0)
		return;

	int	num_depots = depot_list.size();
	if(num_depots > 0)
	{
		int which = rand() % num_depots;
		DepotList::iterator	iter;
		int count;
		for(count = 0,iter = depot_list.begin();iter != depot_list.end();iter++,count++)
		{
			if(count == which)
				break;
		}

		if(iter != depot_list.end())
		{
			Company	*company = Game::company_register->Find(iter->second->Owner());
			if(company != 0)
			{
				Player	*depot_owner = Game::company_register->Owner(iter->second->Owner());
				if(depot_owner != 0)
				{
					if(company->DestroyDepot(home))
					{
						riots->ReportDepotDamage(founder,depot_owner,company);
						std::ostringstream	buffer;
						buffer << depot_owner->Name() << " has lost a depot on " << home->Title() << " to rioting!";
						WriteLog(buffer);
					}
				}
			}
		}
	}

	ExchangeRiot();
}

void	Infrastructure::Display(Player *player,bool show_fabric)
{
	std::ostringstream	buffer;
	buffer << home->Title() << ", " << home->HomeStar() << " system - Owner " << owner_name;
	if(!IsOpen(0))
		buffer << " - Closed to visitors";
	buffer << "\n  Cartel: " << home->HomeStarPtr()->CartelName() << "\n";
	player->Send(buffer);
	buffer.str("");
	buffer << "  Economy: " << econ_names[economy];
	if(economy != 0)
		buffer << "    Workforce: "<< workers << "/" << total_workers << "\n";
	buffer << "  Shipyard markup: " << yard_markup << "%";
	if(flags.test(REGISTRY))
	{
		if(fleet_size == 1)
			buffer << "    Merchant Fleet: 1 ship";
		else
			buffer << "    Merchant Fleet: " << fleet_size << " ships";
	}
	buffer << "\n  ";
	if(player->Rank() >= Player::MERCHANT)
	{
		DisplayDisaffection(buffer);
		buffer << "\n";
	}
	player->Send(buffer);

	if(player->Name() == owner_name)
	{
		buffer.str("");
		buffer << "  Treasury: ";
		MakeNumberString(treasury,buffer);
		buffer << "ig\n";
		player->Send(buffer);
	}

	if(show_fabric)
	{
		if((depot_list.size() + warehouse_list.size() + factories.size()) > 0)
		{
			player->Send("Commercial Activities:\n");
			DisplayWarehouses(player);
			DisplayDepots(player);
			DisplayFactories(player);
		}
	}
}

void	Infrastructure::DisplayBuilds(Player *player)
{
	std::ostringstream	buffer;
	if(enhancements.size() == 0)
	{
		buffer << home->Title() << " - No infrastructure built yet!\n";
		player->Send(buffer);
		return;
	}
	buffer << home->Title() << " - Infrastructure:\n";
	player->Send(buffer);
	int total = 0;
	for(EnhanceList::iterator iter = enhancements.begin();iter != enhancements.end();iter++)
	{
		(*iter)->Display(player);
		total += (*iter)->TotalBuilds();
	}
	buffer.str("");
	buffer << "   \nTotal infrastructure builds: "<< total << "\n";
	player->Send(buffer);
}

void	Infrastructure::DisplayDepots(Player *player)
{
	std::ostringstream	buffer;
	if(depot_list.size() != 0)
	{
		buffer << "  Depots:\n";
		for(DepotList::iterator iter = depot_list.begin();iter != depot_list.end();iter++)
			buffer << "    " << iter->first << "\n";
	}
	buffer << std::endl;
	player->Send(buffer);
}

void	Infrastructure::DisplayDisaffection(std::ostringstream& buffer)
{
	if(disaffection < 0)
		buffer << "Approval rating: " << -disaffection << "%";
	if(disaffection == 0)
		buffer << "Approval rating: Satisfactory";
	if(disaffection > 0)
		buffer << "Disaffection rating: " << disaffection << "%";
}

void	Infrastructure::DisplayDisaffection(Player *player)
{
	discontent->Display(player);
}

void	Infrastructure::DisplayFactories(Player *player)
{
	std::ostringstream	buffer;
	if(factories.size() != 0)
	{
		buffer << "  Factories:\n";
		for(FactoryList::iterator iter = factories.begin();iter != factories.end();iter++)
			(*iter)->PlanetLineDisplay(buffer);
	}
	buffer << std::endl;
	player->Send(buffer);
}

void	Infrastructure::DisplayPopulation(Player *player)
{
	population->Display(player);
}

void	Infrastructure::DisplayWarehouses(Player *player)
{
	if(warehouse_list.size() != 0)
	{
		std::ostringstream	buffer;
		buffer << "  " << warehouse_list.size() << " warehouses:\n";
		player->Send(buffer);
	}
	else
		player->Send("  No warehouses\n");
}

int	Infrastructure::EfficiencyBonus(int type) const
{
	switch(type)
	{
		case Commodities::C_AGRI:		return(efficiency->AgriEfficiency());
		case Commodities::C_RES:		return(efficiency->ResourceEfficiency());
		case Commodities::C_IND:		return(efficiency->IndEfficiency());
		case Commodities::C_TECH:		return(efficiency->TechEfficiency());
		case Commodities::C_BIO:		return(efficiency->BioEfficiency());
		case Commodities::C_LEIS:		return(efficiency->LeisureEfficiency());

		case Commodities::C_BULK:		return(efficiency->BulkEfficiency());
		case Commodities::C_CONS:		return(efficiency->ConsumerEfficiency());
		case Commodities::C_DEFENCE:	return(efficiency->DefenceEfficiency());
		case Commodities::C_ENERGY:	return(efficiency->EnergyEfficiency());
		case Commodities::C_SEA:		return(efficiency->SeaEfficiency());

		case Commodities::C_ALL:		return(efficiency->AllEfficiency());
	}
	return(0);
}

void	Infrastructure::EmbezzleRiot()
{
	Player	*player = Game::player_index->FindName(owner_name);
	if(riots == 0)
		riots = new Riots(home);

	if(treasury > 0L)
	{
		long old_treasury = treasury;
		int percentage = 70 - (rand() % 31);
		treasury = (treasury/100) * percentage;
		riots->ReportEmbezzleDamage(player,old_treasury - treasury);

		std::ostringstream	buffer;
		buffer << "Hackers reduce " << home->Title() << "'s treasury from " << old_treasury << "ig to ";
		buffer << treasury << " (a " << (100 - percentage) << "% reduction)";
		WriteLog(buffer);
	}

	ExchangeRiot();
}

void	Infrastructure::ExchangeRiot()
{
	Player	*founder = Game::player_index->FindName(Owner());
	const std::string& commod = home->ExchangeRiot();
	if(commod != "")
	{
		riots->ReportExchangeDamage(founder,commod);
		std::ostringstream	buffer;
		buffer << home->Title() << " has lost all its " << commod << " stock to rioting!";
		WriteLog(buffer);
	}
}

void	Infrastructure::FactoryRiot()
{
	if(riots == 0)
		riots = new Riots(home);

	Player	*founder = Game::player_index->FindName(Owner());
	int	num_factories = factories.size();
	if(num_factories > 0)
	{
		int which = rand() % num_factories;
		FactoryList::iterator	iter;
		int count;
		for(count = 0,iter = factories.begin();iter != factories.end();iter++,count++)
		{
			if(count == which)
				break;
		}
		if(iter != factories.end())
		{
			Company	*company = Game::company_register->Find((*iter)->Owner());
			if(company == 0)
				return;
			Player	*factory_owner = Game::company_register->Owner((*iter)->Owner());
			if(factory_owner == 0)
				return;
			std::string	commod_name = (*iter)->Output();
			if(company->DestroyFactory(home,*iter))
			{
				riots->ReportFactoryDamage(founder,factory_owner,company,commod_name);
				std::ostringstream	buffer;
				buffer << factory_owner->Name() << " has lost a " << commod_name;
				buffer << " factory on " << home->Title() << " to rioting!";
				WriteLog(buffer);
			}
		}
	}

	ExchangeRiot();
}

Depot	*Infrastructure::FindDepot(const std::string& name)
{
	DepotList::iterator iter = depot_list.find(name);
	if(iter != depot_list.end())
		return(iter->second);
	else
		return(0);
}

int	Infrastructure::FindEconomyType(const std::string& name)
{
	for(int count = 0;econ_names[count] != "";count++)
	{
		if(name == econ_names[count])
			return(count);
	}
	return(NO_ECON);
}

Enhancement	*Infrastructure::FindEnhancement(const std::string& name)
{
	for(EnhanceList::iterator	iter = enhancements.begin();iter != enhancements.end();iter++)
	{
		if((*iter)->Name() == name)
			return(*iter);
	}
	return(0);
}

Factory	*Infrastructure::FindFactory(const std::string& co_name,int f_num)
{
	for(FactoryList::iterator iter = factories.begin();iter != factories.end();iter++)
	{
		if(((*iter)->Number() == f_num) && ((*iter)->Owner() == co_name))
			return(*iter);
	}
	return(0);
}

Warehouse	*Infrastructure::FindWarehouse(const std::string& name)
{
	WarehouseList::iterator iter = warehouse_list.find(name);
	if(iter != warehouse_list.end())
		return(iter->second);
	else
		return(0);
}

void	Infrastructure::FusionExplosion()
{
	EnhanceList::iterator	iter;
	for(iter = enhancements.begin();iter != enhancements.end();iter++)
	{
		if(((*iter)->Name() == "Fusion") && ((std::rand() % 120) == 0))
		{
			bool do_delete = (*iter)->Riot();
			Enhancement	*build = *iter;
			int new_casualties = 50 + (std::rand() % 100);

			std::ostringstream	buffer;
			buffer << "Reports are coming of a fusion generator explosion on " << home->Title();
			WriteLog(buffer);
			buffer << "\n";
			Game::review->Post(buffer);
			buffer.str("");

			buffer << "   Total Workers: " << total_workers;
			buffer << "   Workers: " << workers;
			buffer << "   Casualties: " << new_casualties;
			WriteLog(buffer);
			buffer.str("");

			if((total_workers > (new_casualties + 10)) && (workers > (new_casualties + 10)))
			{
				casualties += new_casualties;
				total_workers -= new_casualties;
				AddLabour(-new_casualties);
			}

			FedMssg	*mssg = new FedMssg;
			mssg->sent = std::time(0);
			mssg->to = owner_name;
			buffer.str("");
			buffer << "Head of Internal Security, " << home->Title();
			mssg->from = buffer.str();
			buffer.str("");
			buffer << "One of your fusion generators has exploded. ";
			buffer << "The casualty count has reached " << new_casualties << "\n";
			mssg->body = buffer.str();
			Game::fed_mail->Add(mssg);

			if(do_delete)
			{
				enhancements.erase(iter);
				delete build;
			}
			return;
		}
	}
}

bool	Infrastructure::HasAirportUpgrade()
{
	Enhancement	*airport = FindEnhancement("Airport");
	if((airport != 0) && (airport->Get() != 0))
		return(true);
	else
		return(false);
}

void	Infrastructure::Immigrants(int num_workers)
{
	population->SetBasePopulation(population->GetBasePopulation() + num_workers);
	total_workers += num_workers;
	workers += num_workers;
}

bool	Infrastructure::IncBuild(Player *player,int build_type,Tokens *tokens)
{
	static const std::string	no_cash("Your treasury doesn't have the 10,000,000ig it would cost!\n");
	if(treasury < 10000000L)
	{
		player->Send(no_cash);
		return(false);
	}
	switch(build_type)
	{
		case	Enhancement::HELIOGRAPH:	return(IncEnhancement<Helio>(player,tokens,"Heliograph"));
		case	Enhancement::BASE:			return(IncEnhancement<Base>(player,tokens,"Base"));
		case	Enhancement::CLINIC:			return(IncEnhancement<Clinic>(player,tokens,"Clinic"));
		case	Enhancement::HOSPITAL:		return(IncEnhancement<Hospital>(player,tokens,"Hospital"));
		case	Enhancement::POLICE:			return(IncEnhancement<Police>(player,tokens,"Police"));
		case	Enhancement::FAMILY:			return(IncEnhancement<Family>(player,tokens,"Family"));
		case	Enhancement::CANAL:			return(IncEnhancement<Canal>(player,tokens,"Canal"));
		case	Enhancement::SCHOOL:			return(IncEnhancement<School>(player,tokens,"School"));
		case	Enhancement::AGRI_COLLEGE:	return(IncEnhancement<AgriCollege>(player,tokens,"AgriCollege"));
		case	Enhancement::ATMOS:			return(IncEnhancement<AtmosControl>(player,tokens,"Atmos"));
		case	Enhancement::POLLUTION:		return(IncEnhancement<Pollution>(player,tokens,"Pollution"));
		case	Enhancement::BIODIVERSITY:	return(IncEnhancement<BioDiversity>(player,tokens,"Biodiversity"));
		case	Enhancement::DOLE:			return(IncEnhancement<Dole>(player,tokens,"Dole"));
		case	Enhancement::TELEGRAPH:		return(IncEnhancement<Telegraph>(player,tokens,"Telegraph"));
		case	Enhancement::COAL:			return(IncEnhancement<Coal>(player,tokens,"Coal"));
		case	Enhancement::PENSION:		return(IncEnhancement<Pension>(player,tokens,"Pension"));
		case	Enhancement::RAILWAY:		return(IncEnhancement<Railway>(player,tokens,"Railway"));
		case	Enhancement::HOUSING:		return(IncEnhancement<Housing>(player,tokens,"Housing"));
		case	Enhancement::RIOT_POLICE:	return(IncEnhancement<RiotPolice>(player,tokens,"Riotpolice"));
		case	Enhancement::MINING_SCH:	return(IncEnhancement<MiningSchool>(player,tokens,"Mining"));
		case	Enhancement::INSULATION:	return(IncEnhancement<Insulation>(player,tokens,"Insulation"));
		case	Enhancement::SATELLITE:		return(IncEnhancement<Satellite>(player,tokens,"Satellite"));
		case	Enhancement::PORT:			return(IncEnhancement<Port>(player,tokens,"Port"));
		case	Enhancement::RADIO:			return(IncEnhancement<Radio>(player,tokens,"Radio"));
		case	Enhancement::TECH:			return(IncEnhancement<TechInst>(player,tokens,"Tech"));
		case	Enhancement::OIL:				return(IncEnhancement<Oil>(player,tokens,"Oil"));
		case	Enhancement::ANTIGRAV:		return(IncEnhancement<AntiGrav>(player,tokens,"Antigrav"));
		case	Enhancement::PHONE:			return(IncEnhancement<Phone>(player,tokens,"Phone"));
		case	Enhancement::AIRLANE:		return(IncEnhancement<AirLane>(player,tokens,"Airlane"));
		case	Enhancement::FIBRE:			return(IncEnhancement<FibreOptics>(player,tokens,"Fibre"));
		case	Enhancement::RESEARCH:		return(IncEnhancement<ResearchInst>(player,tokens,"Research"));
		case	Enhancement::DEFENCE:		return(IncEnhancement<DefenceVessel>(player,tokens,"Defence"));
		case	Enhancement::WEATHER:		return(IncEnhancement<Weather>(player,tokens,"Weather"));
		case	Enhancement::URBAN:			return(IncEnhancement<Urban>(player,tokens,"Urban"));
		case	Enhancement::SURVEILLANCE:	return(IncEnhancement<Surveillance>(player,tokens,"Surveillance"));
		case	Enhancement::FUSION:			return(IncEnhancement<FusionPower>(player,tokens,"Fusion"));
		case	Enhancement::INSURANCE:		return(IncEnhancement<Insurance>(player,tokens,"Insurance"));
		case	Enhancement::FLOATING:		return(IncEnhancement<FloatingCity>(player,tokens,"Floating"));
		case	Enhancement::BIOLAB:			return(IncEnhancement<BioLab>(player,tokens,"Biolab"));
		case	Enhancement::TERRAFORM:		return(IncEnhancement<Terraform>(player,tokens,"Terraform"));
		case	Enhancement::GENETIC:		return(IncEnhancement<Genetic>(player,tokens,"Genetic"));
		case	Enhancement::TACHYON:		return(IncEnhancement<Tachyon>(player,tokens,"Tachyon"));
		case	Enhancement::TQUARK:			return(IncEnhancement<TQuark>(player,tokens,"Tquark"));
		case	Enhancement::METASTUDIO:	return(IncEnhancement<MetaStudio>(player,tokens,"Metastudio"));
		case	Enhancement::SOLAR:			return(IncEnhancement<Solar>(player,tokens,"Solar"));
		case	Enhancement::INFORMER:		return(IncEnhancement<Informer>(player,tokens,"Informer"));
		case	Enhancement::LEISURE:		return(IncEnhancement<Leisure>(player,tokens,"Leisure"));
		case	Enhancement::AIRPORT:		return(IncEnhancement<Airport>(player,tokens,"Airport"));
		case	Enhancement::ANTIAGATHICS:	return(IncEnhancement<AntiAgathics>(player,tokens,"Longevity"));
	}
	player->Send("I don't understand what it is you are trying to build!\n");
	return(false);
}

void	Infrastructure::InformerRingBust()
{
	Enhancement	*informer = FindEnhancement("Informer");
	if((informer != 0) && ((std::rand() % 130) == 0))
	{
		std::ostringstream	buffer;
		buffer << "The existance of a government informer ring on " << home->Title() << " has ";
		buffer << "become public knowledge causing grave disaffection among the inhabitants!\n";
		Game::review->Post(buffer);
		buffer.str("");
		buffer << "An informer network has been outed on " << home->Title() << "!";
		WriteLog(buffer);

		if(disaffection < 50)
			disaffection = 50;
		ProcessDisaffection();
	}
}

bool	Infrastructure::IsOpen(Player *player)
{
	if(player == 0)
		return(!status.test(CLOSED));

	if(status.test(CLOSED))
	{
		if((player->Name() == owner_name) || player->IsManager())
			return(true);
		else
			return(false);
	}
	else
		return(true);
}

const std::string&	Infrastructure::List(Player *player,const std::string& star_name)
{
	static std::string	text;
	std::ostringstream	buffer;

	buffer << home->Title() << "(" << econ_names[economy][0] << ") ";
	text = buffer.str();
	return(text);
}

Warehouse	*Infrastructure::NewWarehouse(Player *player)
{
	Warehouse *ware = new Warehouse(home,player->Name());
	AddWarehouse(ware,player->Name());
	return(ware);
}

void	Infrastructure::Open(Player *player)
{
	const static std::string	error("Only owners of planets can open them to visitors!\n");

	if((player->Name() == owner_name) || player->IsManager())
	{
		status.reset(CLOSED);
		std::ostringstream	buffer;
		buffer << home->Title() << " is now open to visitors.\n";
		player->Send(buffer);
		Game::review->Post(buffer);
	}
	else
		player->Send(error);
}

void	Infrastructure::Output(Player *player)
{
	static const std::string	error("You don't own this planet!\n");
	static const std::string	none("There aren't any factories on this planet!\n");

	if(player->Name() !=  owner_name)
	{
		player->Send(error);
		return;
	}

	if(factories.size() > 0)
	{
		player->Send("Factory output survey:\n");
		for(FactoryList::iterator iter = factories.begin();iter != factories.end();iter++)
			(*iter)->Output(player);
	}
	else
		player->Send(none);
}

void	Infrastructure::PersonalRiot()
{
	Player	*player = Game::player_index->FindName(owner_name);
	if(riots == 0)
		riots = new Riots(home);

	if (player != 0)
	{
		int percentage = 70 - (rand() % 31);
		long loss = player->PersonalRiot(percentage);
		if(loss != 0L)
		{
			riots->ReportPersonalDamage(player,loss);
			std::ostringstream	buffer;
			buffer << "Hackers operating from riot torn " << home->Title() << " have stolen " << loss << "ig from ";
			buffer << player->Name() << "'s personal bank account (a "<< (100 - percentage) << "% reduction)";
			WriteLog(buffer);
		}
	}

	ExchangeRiot();
}

void	Infrastructure::PODisplay(Player *player)
{
	const std::string	not_owner("You don't own this planet!\n");
	const std::string	no_factories("No one has built any factories on this planet!\n");

	if(player->Name() != owner_name)
	{
		player->Send(not_owner);
		return;
	}
	if(factories.size() == 0)
	{
		player->Send(no_factories);
		return;
	}

	std::ostringstream	buffer;
	buffer << "Factories located on " << home->Title() << ":\n";
	player->Send(buffer);
	for(FactoryList::iterator iter = factories.begin();iter != factories.end();iter++)
		(*iter)->PODisplay(player);
}

void	Infrastructure::ProcessDisaffection()
{
	std::ostringstream	buffer;
	buffer << home->Title() << ": Disaffection is " << disaffection << "%";
	WriteLog(buffer);

	if(CheckForRioting())
	{
		int	riot_type =  rand() % 6;
		switch(riot_type)
		{
			case Riots::BUILDS:		BuildRiot();		break;
			case Riots::WAREHOUSES:	WarehouseRiot();	break;
			case Riots::DEPOTS:		DepotRiot();		break;
			case Riots::FACTORIES:	FactoryRiot();		break;
			case Riots::EMBEZZLE:	EmbezzleRiot();	break;
			case Riots::PERSONAL:	PersonalRiot();	break;
		}
	}
}

void	Infrastructure::ProcessInfrastructure(CommodityExchange *commodity_exchange)
{
	CalculatePopulationAndWorkers();
	CalculateEfficiency();

	if((economy > AGRICULTURAL) && (economy != BIOLOGICAL))
		disaffection = CalculateDisaffection();

	if(disaffection > 0)
		ProcessDisaffection();

	ProcessWorkForce(commodity_exchange);
}

void	Infrastructure::ProcessWorkForce(CommodityExchange *commodity_exchange)
{
	int consumer_change = (total_workers - 1000)/100;
	if(consumer_change != 0)
		commodity_exchange->AdjustConsumerConsumption(consumer_change);
}

void	Infrastructure::Promote(Player *player)
{
	if((promote >= 0) && (player != 0))
	{
		player->Send("Your planet is already scheduled for promotion!\n");
		return;
	}

	int	promo = -1;
	switch(economy)
	{
		case AGRICULTURAL:	promo = 30;		break;
		case RESOURCE:			promo = 70;		break;
		case INDUSTRIAL:		promo = 125;	break;
	}

	if((promo < 0) && (player != 0))
	{
		player->Send("Promotion is not available at this level.\n");
		return;
	}

	if(TotalBuilds() >= promo)
	{
		promote = economy + 1;
		if(economy == LEISURE)
			promote = 0;
		if(player != 0)
			player->Send("Your planet will promote at the next reset.\n");
	}
	else
	{
		if(player != 0)
		{
			player->Send("Your planet is not yet ready for promotion!\n");
			std::ostringstream	buffer;
			buffer << "You have " << TotalBuilds() << " builds...\n";
			player->Send(buffer);
		}
	}
}

void	Infrastructure::PromotePlanetOwners()
{
	if(promote != -1)
	{
		for(EnhanceList::iterator iter = enhancements.begin();iter != enhancements.end();iter++)
			(*iter)->LevelUpdate();
		Player	*player = Game::player_index->FindName(owner_name);
		if(player != 0)
		{
			if(economy == LEISURE)
				player->Gengineer2Magnate();
			else
				player->Promote();
		}
		promote = -1;
		home->SaveCommodityExchange();
		home->SaveInfrastructure();
	}
}

void	Infrastructure::Promote2Leisure(Player *player)
{
	if(promote >= 0)
	{
		player->Send("Your planet is already scheduled for promotion!\n");
		return;
	}

	if(TotalBuilds() >= 265)
	{
		promote = LEISURE;
		player->Send("Your planet will promote at the next reset.\n");
		return;
	}

	std::ostringstream	buffer;
	buffer << "Your planet is not yet ready for promotion! ";
	buffer << "You only have " << TotalBuilds() << " builds...\n";
	player->Send(buffer);
}

void	Infrastructure::ReleaseAssets(const std::string& ask_whom,const std::string& from_whom)
{
	for(EnhanceList::iterator iter = enhancements.begin();iter != enhancements.end();iter++)
	{
		if((*iter)->Name() == ask_whom)
			return((*iter)->ReleaseAssets(from_whom));
	}
}

bool	Infrastructure::RemoveWarehouse(Player *player)
{
	WarehouseList::iterator iter = warehouse_list.find(player->Name());
	if(iter != warehouse_list.end())
	{
		Warehouse	*ware = iter->second;
		warehouse_list.erase(iter);
		delete ware;
		return(true);
	}
	else
		return(false);
}

void	Infrastructure::Report()
{
	std::ostringstream	buffer;
	buffer << home->Title() << ": " << treasury/1000000 << "Mig, ";
	int	num_builds = 0;
	for(EnhanceList::iterator iter = enhancements.begin();iter != enhancements.end();iter++)
		num_builds += (*iter)->TotalBuilds();
	buffer << num_builds << " builds";
	WriteLog(buffer);
}

bool	Infrastructure::RequestResources(Player *player,const std::string& donor,const std::string& recipient,int quantity)
{
	static const std::string	error("You don't seem to have the necessary prerequisites for your build!\n");

	for(EnhanceList::iterator iter = enhancements.begin();iter != enhancements.end();iter++)
	{
		if((*iter)->Name() == donor)
			return((*iter)->RequestResources(player,recipient,quantity));
	}
	player->Send(error);
	return(false);
}

void	Infrastructure::SendXMLBuildInfo(Player *player)
{
	int total = 0;
	for(EnhanceList::iterator iter = enhancements.begin();iter != enhancements.end();iter++)
	{
		total += (*iter)->TotalBuilds();
		(*iter)->XMLDisplay(player);
	}
	if(total > 0)
	{
		std::ostringstream	buffer;
		buffer << "Total Infrastructure Builds: " << total;
		AttribList attribs;
		attribs.push_back(std::make_pair("info",buffer.str()));
		player->Send("",OutputFilter::BUILD_PLANET_INFO,attribs);
	}
}

void	Infrastructure::SendXMLPlanetInfo(Player *player)
{
	std::ostringstream	buffer;
	AttribList attribs;
	attribs.push_back(std::make_pair("name",home->Title()));
	buffer << home->HomeStarPtr()->CartelName() << " Cartel, " << home->HomeStar() <<"' ";
	attribs.push_back(std::make_pair("system",buffer.str()));
	attribs.push_back(std::make_pair("owner",owner_name));
	attribs.push_back(std::make_pair("economy",econ_names[economy]));
	buffer.str("");
	buffer << total_workers;
	attribs.push_back(std::make_pair("total-wf",buffer.str()));
	buffer.str("");
	buffer << workers;
	attribs.push_back(std::make_pair("avail-wf",buffer.str()));
	buffer.str("");
	buffer << yard_markup;
	attribs.push_back(std::make_pair("yard",buffer.str()));
	if(player->Rank() >= Player::MERCHANT)
	{
		buffer.str("");
		buffer << disaffection;
		attribs.push_back(std::make_pair("disaffection",buffer.str()));
	}
	if(flags.test(REGISTRY))
	{
		buffer.str("");
		buffer << fleet_size;
		attribs.push_back(std::make_pair("fleet",buffer.str()));
	}
	if(player->Name() == owner_name)
	{
		buffer.str("");
		buffer << treasury;
		attribs.push_back(std::make_pair("treasury",buffer.str()));
	}
	player->Send("",OutputFilter::GEN_PLANET_INFO,attribs);

	for(WarehouseList::iterator iter = warehouse_list.begin();iter != warehouse_list.end();iter++)
	{
		AttribList attribs;
		attribs.push_back(std::make_pair("name",iter->first));
		player->Send("",OutputFilter::WARE_PLANET_INFO,attribs);
	}

	for(DepotList::iterator iter = depot_list.begin();iter != depot_list.end();iter++)
	{
		AttribList attribs;
		attribs.push_back(std::make_pair("name",iter->first));
		player->Send("",OutputFilter::DEPOT_PLANET_INFO,attribs);
	}

	for(FactoryList::iterator iter = factories.begin();iter != factories.end();iter++)
		(*iter)->PlanetXMLLineDisplay(player);

	SendXMLBuildInfo(player);
}

bool	Infrastructure::SetRegistry(Player *player)
{
	static const std::string	error("You are already a ship registrar for this planet!\n");
	static const std::string	ok("Congratulations, you are now the ship registrar for this planet.\n");

	if(flags.test(REGISTRY))
	{
		player->Send(error);
		return(false);
	}
	else
	{
		flags.set(REGISTRY);
		player->Send(ok);
		return(true);
	}
}

void	Infrastructure::SetYardMarkup(Player *player,int amount)
{
	static const std::string	error("You're not the owner of this planet!\n");

	if(player->Name() != owner_name)
		player->Send(error);
	else
	{
		if(amount > 10)
			amount = 10;
		if(amount < -10)
			amount = -10;
		yard_markup = amount;
		std::ostringstream	buffer;
		buffer << "Spaceship purchase prices for this planet are now ";
		if(yard_markup == 0)
			buffer << "set to ";
		else
		{
			buffer << std::abs(yard_markup) << "% ";
			if(yard_markup > 0)
				buffer << "above ";
			else
				buffer << "below ";
		}
		buffer << "the standard rate.\n";
		player->Send(buffer);
	}
}

bool	Infrastructure::SlithyXform(Player *player)
{
	if(slithy_xform > 0)
	{
		std::ostringstream buffer;
		buffer << "You can't use slithies to add to the treasury on " << home->Title();
		buffer << " for another " << slithy_xform << " days!\n";
		player->Send(buffer);
		return(false);
	}
	else
	{
		treasury += 10000000L;
		slithy_xform = 14;
		return(true);
	}
}

bool	Infrastructure::SupplyWorkers(int num,int wages,Factory *factory)
{
	if(workers < num)
		return(false);

	int	remaining_workers = workers;
	for(FactoryList::iterator iter = factories.begin();iter != factories.end();iter++)
	{
		if((*iter) != factory)
			remaining_workers -= (*iter)->IsHiring(wages);
	}

	if(remaining_workers >= num)
	{
		AddLabour(-num);
		return(true);
	}
	else
		return(false);
}

int	Infrastructure::TotalBuilds()
{
	int total_builds = 0;
	for(EnhanceList::iterator	iter = enhancements.begin();iter != enhancements.end();iter++)
		total_builds += (*iter)->TotalBuilds();
	return(total_builds);
}

bool	Infrastructure::UpgradeAirport(Player *player)
{
	static const std::string	no_airport("You don't actually seem to have any airports \
on this planet...\n");
	static const std::string	not_enuff("You need ten airports on the planet before \
you can upgrade...\n");
	static const std::string	ok("The machines move in and the airports on this planet \
are given the additional capacity to handle interplanetary migrations.\n");

	Enhancement	*airport = FindEnhancement("Airport");
	if(airport == 0)
	{
		player->Send(no_airport);
		return(false);
	}
	if(airport->Set() == 0)		// generic set - used to set trans global flag here
	{
		player->Send(not_enuff);
		return(false);
	}

	player->Send(ok);
	return(true);
}

void	Infrastructure::WarehouseRiot()
{
	if(riots == 0)
		riots = new Riots(home);
	int num_wares = warehouse_list.size();
	if(num_wares > 0)
	{
		int which = rand() % num_wares;
		WarehouseList::iterator	iter;
		int	count;
		for(count = 0,iter = warehouse_list.begin();iter != warehouse_list.end();iter++,count++)
		{
			if(count == which)
				break;
		}
		if(iter != warehouse_list.end())
		{
			Player	*player = Game::player_index->FindName(iter->second->Owner());
			if(player != 0)
			{
				player->RemoveWarehouse(home);
				RemoveWarehouse(player);
				riots->ReportWarehouseDamage(player);
				std::ostringstream	buffer;
				buffer << player->Name() << " has lost a warehouse on " << home->Title() << " to rioting!";
				WriteLog(buffer);
			}
		}
	}

	ExchangeRiot();
}

void	Infrastructure::Weathermen()
{
	EnhanceList::iterator	iter;
	for(iter = enhancements.begin();iter != enhancements.end();iter++)
	{
		if(((*iter)->Name() == "Weather") && ((std::rand() % 60) < 2))
		{
			bool do_delete = (*iter)->Riot();
			Enhancement	*build = *iter;

			std::ostringstream	buffer;
			buffer << "The Weathermen Cult has destroyed a Weather Control Station on " << home->Title();
			WriteLog(buffer);
			buffer << "\n";
			Game::review->Post(buffer);

			FedMssg	*mssg = new FedMssg;
			mssg->sent = std::time(0);
			mssg->to = owner_name;
			buffer.str("");
			buffer << "Head of Internal Security, " << home->Title();
			mssg->from = buffer.str();
			mssg->body = "The Weathermen Cult has destroyed a Weather Control Station\n";
			Game::fed_mail->Add(mssg);

			if(do_delete)
			{
				enhancements.erase(iter);
				delete build;
			}

			return;
		}
	}
}

void	Infrastructure::Write(std::ofstream& file)
{
	file << "<?xml version=\"1.0\"?>\n";
	file << "<infrastructure owner='" << owner_name << "' economy='";
	file << econ_names[economy] << "' treasury='" << treasury << "' yard-markup='";
	file << yard_markup << "' workers='" << total_workers << "' base='";
	file << population->GetBasePopulation() << "'";
	if(casualties > 0)
		file << " casualties='" << casualties << "'";
	if(!IsOpen(0))
		file << " closed='true'";
	if(promote >= 0)
		file << " promote='" << promote << "'";
	if(slithy_xform >= 0)
		file << " slithy-xform='" << slithy_xform << "'";
	file << " registry=" << (flags.test(REGISTRY) ? "'true'" : "'false'") << ">\n";
	for(EnhanceList::iterator iter = enhancements.begin();iter != enhancements.end();iter++)
		(*iter)->Write(file);
	for(WarehouseList::iterator iter = warehouse_list.begin();iter != warehouse_list.end();iter++)
		iter->second->Write(file);
	for(DepotList::iterator iter = depot_list.begin();iter != depot_list.end();iter++)
		iter->second->Write(file);
	for(FactoryList::iterator iter = factories.begin();iter != factories.end();iter++)
		(*iter)->Write(file);
	file << "</infrastructure>" << std::endl;
}

void	Infrastructure::XferFunds(Player *player,int amount,const std::string& to)
{
	static const std::string	err("I can't find a planet with that name!\n");
	static const std::string	not_enuff("Your treasury doesn't have that much money!\n");
	static const std::string	not_owner("You need to own both planets to make transfers between them!\n");
	static const std::string	negative("Positive amounts of money only!\n");

	std::string	to_planet(to);
	NormalisePlanetTitle(to_planet);
	FedMap	*fed_map = Game::galaxy->FindMap(to_planet);

	if(fed_map == 0)					{	player->Send(err);			return;	}
	if((amount *= 1000000) < 0)	{	player->Send(negative);	return;	}
	if( treasury < amount)			{	player->Send(not_enuff);	return;	}
	if(!fed_map->IsOwner(player))	{	player->Send(not_owner);	return;	}

	treasury -= amount;
	long	to_balance = fed_map->ChangeTreasury(amount);
	std::ostringstream	buffer;
	buffer << home->Title() << " treasury: " << treasury << ", ";
	buffer << fed_map->Title() << " treasury: " << to_balance << "\n";
	player->Send(buffer);
}

void	Infrastructure::XMLMapInfo(Player *player)
{
	std::ostringstream	buffer;
	AttribList attribs;
	attribs.push_back(std::make_pair("economy",econ_names[economy]));
	buffer << workers;
	attribs.push_back(std::make_pair("workers",buffer.str()));
	buffer.str("");
	buffer << yard_markup;
	attribs.push_back(std::make_pair("yard",buffer.str()));
	if(flags.test(REGISTRY))
	{
		buffer.str("");
		buffer << fleet_size;
		attribs.push_back(std::make_pair("fleet",buffer.str()));
	}
	player->Send("",OutputFilter::MAP_INFO,attribs);


	for(FactoryList::iterator iter = factories.begin();iter != factories.end();iter++)
		(*iter)->XMLFactoryInfo(player);
	player->Send("",OutputFilter::UPDATE_INFRA);
}

