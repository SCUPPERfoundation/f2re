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

#include "mail_parser.h"

#include <cstdlib>
#include <cstring>

#include "mail.h"
#include "misc.h"


FedMailParser::~FedMailParser()
{
	//
}


FedMssg	*FedMailParser::CreateMessage(const char **attrib)
{
	FedMssg	*mssg = new FedMssg;
	const std::string *time_txt = FindAttrib(attrib,"time");
	if(time_txt == 0)
	{
		delete mssg;
		return(0);
	}
	else
		mssg->sent = std::atol(time_txt->c_str());

	const std::string *to_txt = FindAttrib(attrib,"to");
	if(to_txt == 0)
	{
		delete mssg;
		return(0);
	}
	else
		mssg->to = *to_txt;
	
	const std::string *from_txt = FindAttrib(attrib,"from");
	if(from_txt == 0)
	{
		delete mssg;
		return(0);
	}
	else
		mssg->from = *from_txt;

	return(mssg);
}

void	FedMailParser::EndElement(const char *element)
{
	if(std::strcmp(element,"fed2-mail") == 0)
	{
		Game::fed_mail = fed_mail;
		fed_mail = 0;
	}
	else
	{
		if(std::strcmp(element,"message") == 0)
		{
			if((current != 0) && (fed_mail != 0))
			{
				current->body = buffer; 
				fed_mail->Add(current);
				current = 0;
			}
		}
	}
}

void	FedMailParser::StartElement(const char *element,const char **attrib)
{
	if(std::strcmp(element,"fed2-mail") == 0)
		fed_mail = new FedMail;
	else
	{
		if(std::strcmp(element,"message") == 0)
		{
			current = CreateMessage(attrib);
			buffer = "";
		}
	}
}

void	FedMailParser::TextData(const char *text,int textlen)
{
	char buff[MAX_LINE];
	std::strncpy(buff,text,textlen);
	buff[textlen] = '\0';
	buffer += buff;
}

