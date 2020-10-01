/*-----------------------------------------------------------------------
		        Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#include "output_filter.h"

#include <sstream>

#include <misc.h>

AttribList	OutputFilter::NullAttribs;

std::string&	OutputFilter::EscapeXML(std::string& text)
{
	std::ostringstream	buffer;
	int length = text.length();

	for (int count = 0;count < length;count++)
	{
		switch(text[count])
		{
			case  '<':	buffer << "&lt;";			break;
			case  '>':	buffer << "&gt;";			break;
			case  '&':	buffer << "&amp;";		break;
			case '\'':	buffer << "&apos;";		break;
			case '\"':	buffer << "&quot;";		break;

			default:		buffer << text[count];	break;
		}
	}
	text = buffer.str();
	return(text);
}

std::string&	OutputFilter::XmlNoAttrib(std::string command)
{
	if(type == XML)
	{
		if(text != "")
		{
			std::string temp = "<" + command + ">" + text + "</" + command + ">\n";
			text = temp;
		}
		else
		{
			std::string temp = "<" + command + "/>\n";
			text = temp;
 		}
	}
	return text;
}

std::string&	OutputFilter::XmlNormal(std::string command)
{
	if(attribs.empty())
		return(XmlNoAttrib(command));

	static std::ostringstream	temp;
	temp.str("");

	if(type == XML)
	{
		temp << "<" << command << " ";
		for(AttribList::iterator iter = attribs.begin();iter != attribs.end();++iter)
			temp << iter->first << "='" << EscapeXML(iter->second) << "' ";
		if(text.length() == 0)
			temp << "/>\n";
		else
			temp << ">" << text << "</" << command << ">\n";
		text = temp.str();
	}

	return text;
}

std::string& OutputFilter::Process()
{
	switch(type)
	{
		case JSON:	return text;   	// Not yet implimented
		case XML:	EscapeXML(text);	break;
		default:		return text;		// Leave it alone - ASCII

	}

	switch(cmd)
	{
		case DEFAULT:					return XmlNoAttrib("s-default");
		case EXAMINE:					return XmlNormal("s-examine");
		case LOC:						return XmlNoAttrib("s-loc");
		case SPYNET:					return XmlNoAttrib("s-spynet");
		case ADD_PLAYER:				return XmlNormal("s-add-player");
		case REMOVE_PLAYER:			return XmlNormal("s-remove-player");
		case ADD_CONTENTS:			return XmlNormal("s-contents");
		case REMOVE_CONTENTS:		return XmlNormal("s-remove-contents");
		case TIGHT_BEAM:				return XmlNormal("s-tb");
		case ADD_FACTORY:				return XmlNormal("s-add-factory");
		case REMOVE_FACTORY:			return XmlNormal("s-remove-factory");
		case EXCHANGE:					return XmlNormal("s-exchange");
		case PLAY_SOUND:				return XmlNormal("s-sound");
		case UPDATE_WORKERS:			return XmlNormal("s-update-workers");
		case BUILD_PLANET_INFO:		return XmlNormal("s-build-planet-info");
		case FACTORY_PLANET_INFO:	return XmlNormal("s-factory-planet-info");
		case PLAYER_STATS:			return XmlNormal("s-player-stats");
		case SHIP_STATS:				return XmlNormal("s-ship-stats");
		case NEW_MAP:					return XmlNormal("s-new-map");
		case PLANET_NAME:				return XmlNormal("s-planet-name");
		case JUMP_LINKS:				return XmlNoAttrib("s-jump-links");
		case LINK:						return XmlNormal("s-link");
		case EXCH_SELL:				return XmlNormal("s-exch-sell");
		case EXCH_BUY:					return XmlNormal("s-exch-buy");
		case GEN_PLANET_INFO:		return XmlNormal("s-gen-planet-info");
		case WARE_PLANET_INFO:		return XmlNormal("s-ware-planet-info");
		case DEPOT_PLANET_INFO:		return XmlNormal("s-depot-planet-info");
		case MAP_INFO:					return XmlNormal("s-map-info");
		case UPDATE_INFRA:			return XmlNoAttrib("s-update-infra");
		case NO_OP:						return XmlNoAttrib("s-no-op");
		case REV_NO_OP:				return XmlNoAttrib("s-rev-no-op");
		case NEW_LOC:					return XmlNormal("s-new-loc");
		case EXAMINE_START:			return XmlNormal("s-examine-start");
		case SPYNET_START:			return XmlNormal("s-spynet-start");
		case FEDTERM:					return XmlNoAttrib("s-fedterm");
		case MANIFEST:					return XmlNormal("s-manifest");
		case CARGO:						return XmlNormal("s-cargo");
		case WEAPON_STAT:				return XmlNormal("s-weapon-stats");
		case COMP_STATS:				return XmlNormal("s-computer-stats");
		case MESSAGE:					return XmlNoAttrib("s-message");
		case FULL_STATS:				return XmlNormal("s-full-stats");
		case WEAPONS:					return XmlNormal("s-weapons");

		default:							return text;
	}
}

