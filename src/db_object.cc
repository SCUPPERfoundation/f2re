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

#include "db_object.h"

#include <iostream>

#include <cstdlib>

#include <unistd.h>

#include "galaxy.h"
#include "misc.h"
#include "object.h"
#include "obj_container.h"
#include "player.h"
#include "ship.h"

const int	DBObject::NUM_COLS = 7;

DBObject::DBObject()
{
	if(access("data/objects.db",F_OK != 0))
	{
		if(!CreateDBAndTable())
			WriteLog("Unable to create objects database 'data/objects.db'");
		else
		{
			int	ret_val = sqlite3_open("data/objects.db",&db);
			if(ret_val != SQLITE_OK)
			{
				std::ostringstream	buffer;
				buffer << "Unable to open object database: 'data/objects.db': ";
				buffer << sqlite3_errmsg(db);
				WriteLog(buffer);
				sqlite3_close(db);
				db = 0;
			}
		}
	}
	if(sqlite3_open("data/objects.db",&db) != SQLITE_OK)
	{
		std::ostringstream	buffer;
		buffer << "Unable to create objects database: 'data/objects.db': ";
		buffer << sqlite3_errmsg(db);
		WriteLog(buffer);

		sqlite3_close(db);
		db = 0;
	}
}

DBObject::~DBObject()
{
	if(db != 0)
		sqlite3_close(db);
}


void	DBObject::AddStoredObjectToContainer(Player *player,ObjContainer *container,
							const std::string& obj_template,	const std::string& home_star,
							const std::string& home_map,int size,int visibility,int where)
{
	FedObject	*template_object = Game::galaxy->FetchObject(home_star,home_map,obj_template);
	if(template_object != 0)
	{
		FedObject	*real_object = template_object->Create(0,where,player,size,visibility,true);
		{
			if(real_object != 0)
				container->AddObject(real_object);
		}
	}
}

bool	DBObject::CreateDBAndTable()
{
	WriteLog("Creating new objects database");
	int	ret_val = sqlite3_open("data/objects.db",&db);
	if(ret_val != SQLITE_OK)
	{
		std::ostringstream	buffer;
		buffer << "Unable to create objects database: 'data/objects.db': ";
		buffer << sqlite3_errmsg(db);
		WriteLog(buffer);

		sqlite3_close(db);
		db = 0;
		return(false);
	}

	char		*sql_err;
	std::string sql = "CREATE TABLE objects (template_id TEXT NOT NULL,\
							owner TEXT NOT NULL,\
							home_star_name TEXT NOT NULL,\
                     home_map_title TEXT NOT NULL,\
							size INTEGER NOT NULL DEFAULT 1,\
							visibility INTEGER NOT NULL DEFAULT 100,\
							location INTEGER NOT NULL DEFAULT -1)";
	ret_val = sqlite3_exec(db,sql.c_str(),NULL,NULL,&sql_err);
	if(ret_val != SQLITE_OK)
	{
		std::ostringstream	buffer;
		buffer << sql;
		SqliteError(sql_err,buffer);
		sqlite3_close(db);
		db = 0;
		return(false);
	}

	sql = "CREATE INDEX obj_index on objects(owner)";
	ret_val = sqlite3_exec(db,sql.c_str(),NULL,NULL,&sql_err);
	if(ret_val != SQLITE_OK)
	{
		std::ostringstream	buffer;
		buffer << sql;
		SqliteError(sql_err,buffer);
		sqlite3_close(db);
		db = 0;
		return(false);
	}

	return(true);
}

bool	DBObject::DeleteStoredObjects(const std::string& name,int which)
{
	std::ostringstream	sql;
	char	*sql_err;

	if(which == FedObject::ALL_IN_DATABASE)
		sql << "DELETE FROM objects WHERE owner='" << name << "';";
	else
	{
		sql << "DELETE FROM objects WHERE (owner='" << name;
		sql << "') AND (location=" << which << ");";
	}
	int ret_val = sqlite3_exec(db,sql.str().c_str(),NULL,NULL,&sql_err);
	if(ret_val != SQLITE_OK)
	{
		SqliteError(sql_err,sql);
		return(false);
	}
	else
	{
		return(true);
	}
}

void	DBObject::FetchObjects(Player *player,ObjContainer *container,int which)
{
	if(container == 0)
		return;

	std::ostringstream	sql;
	char	**table;
	char	*sql_err;
	int	num_cols, num_rows;

	sql << "SELECT * FROM objects WHERE (owner='" << player->Name();
	sql << "') AND (location=" << which << ");";

	int	ret_val = sqlite3_get_table(db,sql.str().c_str(),&table,&num_rows,&num_cols,&sql_err);

	if(ret_val != SQLITE_OK)
		SqliteError(sql_err,sql);
	else
	{
		FillContainer(player,container,table,num_rows);
		sqlite3_free_table(table);
	}
}

// table must be freed by the calling routine using sqlite3_free_table()
void	DBObject::FillContainer(Player *player,ObjContainer *container,char **table,int rows)
{
	if(container == 0)
		return;

	for(int count = 0;count < rows;++count)
	{
		AddStoredObjectToContainer(player,container,				// pointers to the player & container
				table[(count + 1) * NUM_COLS + 0],					// template id
				table[(count + 1) * NUM_COLS + 2],					// home star name
				table[(count + 1) * NUM_COLS + 3],					// home map title
				std::atoi(table[(count + 1) * NUM_COLS + 4]),	// size
				std::atoi(table[(count + 1) * NUM_COLS + 5]),	// visibility
				std::atoi(table[(count + 1) * NUM_COLS + 6]));	//	location
	}
}

void	DBObject::SqliteError(char *sql_err,std::ostringstream& sql_cmd)
{
	if(sql_err != NULL)
	{
		std::ostringstream	buffer;
		buffer << "SQL error: " << sql_err;
		WriteLog(buffer);
		WriteLog(sql_cmd);
		sqlite3_free(sql_err);
	}
}

void	DBObject::StoreObjectToDB(const std::string& name,const Object *object)
{
	if((object != 0) && (object->IsStorable()))
	{
		std::ostringstream	sql;
		char	*sql_err;

		sql << "INSERT INTO objects VALUES('" << object->ObjTemplate() << "','";
		sql << name << "','" << object->HomeLocRec()->star_name << "','";
		sql << object->HomeLocRec()->map_name << "'," << object->Size() << ",";
		sql << object->Visibility() << "," << object->Where() << ")";
		if(sqlite3_exec(db,sql.str().c_str(),NULL,NULL,&sql_err) != SQLITE_OK)
			SqliteError(sql_err,sql);
	}
}
