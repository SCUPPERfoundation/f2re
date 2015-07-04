/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2015
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef LOCREC_H
#define LOCREC_H

#include <string>

class FedMap;

struct LocRec
{
	std::string	star_name;
	std::string	map_name;
	int			loc_no;
	FedMap		*fed_map;
};

#endif
