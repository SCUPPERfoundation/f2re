/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef MAILPARSER_H
#define MAILPARSER_H

#include "xml_parser.h"

class FedMail;
class FedMssg;

class	FedMailParser : public XMLParser
{
private:
	FedMail	*fed_mail;
	FedMssg	*current;
	std::string	buffer;

	FedMssg	*CreateMessage(const char **attrib);

	void	EndElement(const char *element);
	void	StartElement(const char *element,const char **attrib);
	void	TextData(const char *text,int textlen);

public:
	FedMailParser() : fed_mail(0), current(0)	{	}
	virtual ~FedMailParser();
};

#endif
