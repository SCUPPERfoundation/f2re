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
