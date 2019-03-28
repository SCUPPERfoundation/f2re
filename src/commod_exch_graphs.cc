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

#include "commod_exch_graphs.h"

#include <iostream>

const int	CommodExchGraphs::MAX_POINTS;
const int	CommodExchGraphs::MAX_GRAPHS;

CommodExchGraphs::CommodExchGraphs()
{
	for(int count = 0;count < MAX_GRAPHS;graph_array[count++] = 0)
		;
}

CommodExchGraphs::~CommodExchGraphs()
{
	for(int count = 0;count < MAX_GRAPHS;count++)
		delete graph_array[count];
}


bool	CommodExchGraphs::AddRecord(CommodGraphRec *rec,int type)
{
	if((type <= 0) || (type >= MAX_GRAPHS))	// we don't use index zero
		return(false);
	else
		graph_array[type] = rec;
	return(true);
}

// Using integer arithmetic, so the brackets matter
int	CommodExchGraphs::CalculateCost(int type,int base_cost,int stock,int max_stock)
{
	// enforce constraints...
	if(max_stock > 10000)
		max_stock = 10000;
	if((stock > 10000) || (stock > max_stock))
		stock = max_stock;
	if((stock < 0) || (type < 1) || (type >= MAX_GRAPHS) || (graph_array[type] == 0))
		return(base_cost);

	int scaled_quantity = (stock * 1000)/max_stock;
	int lower_index = scaled_quantity/100;
	int lower_bound = graph_array[type]->points_array[lower_index];
	int delta = scaled_quantity % 100;

	if(delta == 0)	
		return((base_cost * (100 + lower_bound))/100);
	else
	{
		int upper_bound = graph_array[type]->points_array[lower_index + 1];
		int percent = lower_bound + ((upper_bound - lower_bound) * delta)/100;
		return((base_cost * (100 + percent))/100);
	}
}



