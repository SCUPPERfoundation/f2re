/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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

