/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2016
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef DISPLAYCABINET_H
#define DISPLAYCABINET_H

#include	"obj_container.h"
#include	"xml_parser.h"

#include	<cstdlib>
#include	<string>

class	FedObject;

class	DisplayCabinet : public ObjContainer, public XMLParser
{
public:
	static const int	BASE_SIZE = 57;
	static const int	SLITHY_SIZE = 13;

private:
	static const char				*el_names[];
	static const std::string	messages[];

	DisplayCabinet(const DisplayCabinet& rhs);
	DisplayCabinet& operator=(const DisplayCabinet& rhs);

	int	DisplayList(Player *player,std::ostringstream& buffer);	// returns total objects added

	void	StartCabinet(const char **attrib);
	void	StartElement(const char *element,const char **attrib);	// override XML parser
	void	StartItem(const char **attrib);

public:
	DisplayCabinet(int max_size = BASE_SIZE) : ObjContainer(max_size)	{		}
	~DisplayCabinet();

	FedObject	*RemoveObject(const std::string & name);

	int	Display(Player *player,std::ostringstream& buffer);

	bool	AddObject(FedObject *obj);
	bool	CanTeleport()					{ return true;	}
	bool	Load(const std::string& directory);

	void	ExtensionMssg(Player *player);
	void	Store(const std::string& file_name);
};

#endif // DISPLAYCABINET_H
