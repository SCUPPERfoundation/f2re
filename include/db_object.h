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
