/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef COMMODEXCHGRAPHS_H
#define COMMODEXCHGRAPHS_H

class CommodGraphRec;

class	CommodExchGraphs
{
public:
	static const int	MAX_POINTS = 11;
	static const int	MAX_GRAPHS = 8;

private:
	CommodGraphRec	*graph_array[MAX_GRAPHS];
	
public:
	CommodExchGraphs();
	~CommodExchGraphs();

	int	CalculateCost(int type,int base_cost,int stock,int max_stock);
	bool	AddRecord(CommodGraphRec *rec,int type);
};


struct CommodGraphRec
{
	int	points_array[CommodExchGraphs::MAX_POINTS];
};

#endif
