/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef DBOBJECT_H
#define DBOBJECT_H

#include <sstream>
#include <string>

#include <sqlite3.h>

class FedObject;
class Object;
class	ObjContainer;
class Player;

class	DBObject
{
private:
	static const int	NUM_COLS;	// Number of columns in the 'objects' table
	sqlite3		*db;					// The Sqlite 'objects.db' database pointer

	bool	CreateDBAndTable();

	void	AddStoredObjectToContainer(Player *player,ObjContainer *container,
					const std::string& obj_template,const std::string& home_star,
					const std::string& home_map,int size,int visibility,int where);
	void	FillContainer(Player *player,ObjContainer *container,char **table,int rows);
	void	SqliteError(char *sql_err,std::ostringstream& sql_cmd);

public:
	DBObject();
	~DBObject();

	bool	DeleteStoredObjects(const std::string& name,int which);

	void	FetchObjects(Player *player,ObjContainer *container,int which);
	void	StoreObjectToDB(const std::string& name,const Object *object);
};

#endif
