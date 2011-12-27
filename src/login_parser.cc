/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-6
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "login_parser.h"

#include <sstream>

#include <cstdlib>
#include <cstring>

#include "xml_login.h"

const int	LoginParser::UNKNOWN = 9999;
const char	*LoginParser::elements[] = {	"c-login", "c-newbie", ""	};


LoginParser::LoginParser(XMLLoginRec *login_rec)
{
	rec = login_rec;
}

LoginParser::~LoginParser()
{

}

int	LoginParser::Find(const char *command)
{
	for(int count = 0;elements[count][0] != '\0';count++)
	{
		if(std::strcmp(command,elements[count]) == 0)
			return(count);
	}
	return(UNKNOWN);
}

void	LoginParser::Login(const char **attrib)
{
	rec->api_level = FindNumAttrib(attrib,"level",1);
	const std::string *id = FindAttrib(attrib,"id");
	if(id != 0)
		rec->id = *id;
	const std::string *pwd = FindAttrib(attrib,"pwd");
	if(pwd != 0)
		rec->password = *pwd;

	rec->status = XMLLoginRec::RETURNING;
}

void	LoginParser::Newbie(const char **attrib)
{
	Login(attrib);

	const std::string *email = FindAttrib(attrib,"e-mail");
	if(email != 0)
		rec->email = *email;
	const std::string *name = FindAttrib(attrib,"name");
	if(name != 0)
		rec->name = *name;
	const std::string *species = FindAttrib(attrib,"species");
	if(species != 0)
		rec->species = *species;

	rec->strength = FindNumAttrib(attrib,"str",20);
	rec->stamina = FindNumAttrib(attrib,"sta",20);
	rec->dexterity = FindNumAttrib(attrib,"dex",20);
	rec->intelligence = FindNumAttrib(attrib,"int",20);

	const std::string *gender = FindAttrib(attrib,"gender");
	if(gender != 0)
		rec->gender = (*gender)[0];

	rec->status = XMLLoginRec::NEWBIE;
}

void	LoginParser::StartElement(const char *element,const char **attrib)
{
	switch(Find(element))
	{
		case 0:	Login(attrib);		break;
		case 1:	Newbie(attrib);	break;
	}
}

