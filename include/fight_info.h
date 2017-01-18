/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef FIGHTINFO_H
#define FIGHTINFO_H

// NOTE: If you change this, do -NOT- add any Fed includes

struct FightInfoIn
{
	int	shields;
	int	engines;
	int	sensors;
	int	jammers;
	int 	lasers;
	int 	twin_lasers;
	int 	quad_lasers;
	int 	missile_rack;
	bool 	defence_laser;
};

struct FightInfoOut
{
	bool 	has_damage;
	int 	computer_damage;
	int	sensor_damage;
	int	jammer_damage;
	int 	missile_rack_damage;
	int 	laser_damage;
	int 	twin_laser_damage;
	int 	quad_laser_damage;
	int 	shield_damage;
	int 	hull_damage;
	int 	engine_damage;
};

#endif
