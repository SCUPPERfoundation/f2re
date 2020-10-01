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

#include "inventory.h"

#include <iostream>

#include <cstring>

#include "db_player.h"
#include "fed_object.h"
#include "galaxy.h"
#include "msg_number.h"
#include "object.h"
#include "output_filter.h"

const int	Inventory::ONE_DAY = 60 * 60 * 24;
const int	Inventory::ONE_MONTH = ONE_DAY * 31;

const std::string	Inventory::keyring_desc = " in possession of a multi-dimensional \
keyring made by the prestigious TenBrane Corp. If you look carefully you can see \
the glow of the macro-superstring logo.";
const std::string	Inventory::exec_key_desc = "an executive washroom key. The ornate \
key is a much sought after key to the Galactic Administration executive washroom, \
and is genetically coded to its owner.";

const std::string	Inventory::inv_names[] =
{
	" a comm unit,", " a lamp,", " a ship permit,", " a warehouse building permit,",
	" a spy screen,", " an ID card,", " a long service medal,", " a wedding ring,",
	" a remote access upgrade certificate,", " premium service (placeholder)",
	" a keyring,", " an executive washroom key,", " a Mk1 teleporter control",
	" a Mk3.1 teleporter control",
	""
};

const std::string	Inventory::key_names[] =
{
	"unknown", "keyring", "executive washroom key"
};


Inventory::Inventory(std::string the_owner) : ObjContainer(MAX_INV_SIZE)
{
	owner = the_owner;
	customs_cert = price_check_sub = 0;
	tp_rental = 0;
}

Inventory::Inventory(DBPlayer *rec) : ObjContainer(MAX_INV_SIZE),
			owner(rec->name), keyring(rec->keyring), inv_flags(rec->inv_flags), 
			customs_cert(rec->customs_cert), price_check_sub(rec->price_check_sub)
{
	tp_rental = rec->tp_rental;

	if((tp_rental > 0) && ((std::time(0) - tp_rental) > ONE_MONTH))
	{
		tp_rental = 0;
		inv_flags.reset(TP_1);
	}
	
	if((owner == "Bella") || (owner == "Freya") || (owner == "Hazed"))
		tp_rental = std::time(0);

	ProcessTimeCerts();
	
	if(keyring.test(OLD_KEYRING))
		inv_flags.set(KEYRING);
	if(keyring.test(OLD_EXEC_KEY))
		inv_flags.set(EXEC_KEY);
}

bool	Inventory::AddObject(FedObject *obj)
{
	if(obj->Where() > FedObject::IN_INVENTORY)
		obj->Location(FedObject::IN_INVENTORY);
	return(AddListObject(obj));
}

void	Inventory::Carry(Player *player,const std::string& obj_name)
{
	std::ostringstream	buffer;
	FedObject	*obj = Find(obj_name);
	if(obj == 0)
		buffer << "You don't have one of those!\n";
	else
	{
		obj->ResetFlag(Object::CLIPPED);
		obj->SetFlag(Object::CARRIED);
		obj->ResetFlag(Object::WORN);
		buffer << "You take " << obj->c_str() << " out of your inventory and carry it.\n";
	}
	player->Send(buffer);
}

void	Inventory::ChangeCustomsCert(int num_days)
{
	if(customs_cert == 0)
	{
		customs_cert = std::time(0);
		customs_cert -=  ONE_DAY * 21;
	}
	else
		customs_cert += ONE_DAY * 10;
}

void	Inventory::Clip(Player *player,const std::string& obj_name)
{
	if(!InvFlagSet(KEYRING))
	{
		player->Send("You need to buy a keyring first!\n");
		return;
	}
	
	for(ObjList::iterator iter = obj_list.begin();iter != obj_list.end();++iter)
	{
		if((*iter)->IsClipped())
		{
			(*iter)->ResetFlag(Object::CLIPPED);
			break;
		}
	}

	std::ostringstream	buffer;
	FedObject	*obj = Find(obj_name);
	if(obj == 0)
		buffer << "You don't have one of those!\n";
	else
	{
		obj->SetFlag(Object::CLIPPED);
		obj->ResetFlag(Object::CARRIED);
		obj->ResetFlag(Object::WORN);
		buffer << "You take " << obj->c_str() << " out of your inventory and clip it on your keyring.\n";
	}
	player->Send(buffer);
}

void	Inventory::CreateDBRec(DBPlayer *rec)
{
	rec->inv_flags = static_cast<unsigned int>(inv_flags.to_ulong());
	rec->customs_cert = static_cast<unsigned int>(customs_cert);
	rec->price_check_sub = static_cast<unsigned int>(price_check_sub);
	rec->keyring = static_cast<unsigned int>(keyring.to_ulong());
	rec->tp_rental = static_cast<unsigned int>(tp_rental);
}

void	Inventory::DestroyInventory(const std::string& map_title)
{
	for(ObjList::iterator iter = obj_list.begin();iter != obj_list.end();)
	{
		if((*iter)->HomeMap()->Title() == map_title)
		{
			delete *iter;
			obj_list.erase(iter++);
		}
		else
			iter++;
	}
}

bool	Inventory::DestroyInvObject(const std::string& map_name,const std::string& id_name)
{
	for(ObjList::iterator iter = obj_list.begin();iter != obj_list.end();++iter)
	{
		if((((*iter)->ID() == id_name) || ((*iter)->Name() == id_name))
															&& (map_name == (*iter)->HomeMap()->Name()))
		{
			FedObject	*object = *iter;
			obj_list.erase(iter);
			delete object;
			return(true);
		}
	}
	return(false);
}

int	Inventory::Display(Player *player,std::ostringstream& buff)
{
	std::ostringstream buffer;

	if(player->CommsAPILevel() > 0)
		return(XMLDisplay(player,buffer));

	if(player->Name() != owner)
		return(Display2Watcher(player,buffer));

	buffer.str("");
	DisplayPersonal(buffer);
	player->Send(buffer);
	DisplayKeyring(player,true);

	if(Size() > 0)
	{
		buffer.str("");
		buffer << "You have with you:\n";
		DisplayList(buffer);
		player->Send(buffer);

		buffer.str("");
		MakeFluff(buffer);
		player->Send(buffer);
		buffer.str("");
	}

	return(Size());
}

void	Inventory::DisplayCerts(std::ostringstream& buffer)
{
	time_t	now = std::time(0);
	if(customs_cert != 0)
	{
		buffer << " a customs exemption certificate (" << (31 - (now - customs_cert)/ONE_DAY);
		buffer << " days until expiry),";
	}
	if(price_check_sub != 0)
	{
		buffer << " a remote access subscription certificate (" << (31 - (now - price_check_sub)/ONE_DAY);
		buffer << " days until expiry),";
	}
}

int	Inventory::DisplayInventory(Player *player)
{
	std::ostringstream	buffer;

	DisplayPersonal(buffer);
	player->Send(buffer);
	DisplayKeyring(player,true);

	if(Size() > 0)
	{
		buffer.str("");
		buffer << "You have with you:\n";
		DisplayList(buffer);
		player->Send(buffer);

		buffer.str("");
		MakeFluff(buffer);
		player->Send(buffer);
		buffer.str("");
	}

	return(Size());
}

void	Inventory::DisplayKeyring(Player *player,bool self)
{
	std::ostringstream	buffer;
	if(inv_flags.test(KEYRING) || inv_flags.test(EXEC_KEY))
	{
		if(inv_flags.test(KEYRING))
		{
			if(self)
				buffer << "You are" << keyring_desc;
			else
				buffer << owner << " is" << keyring_desc;
		}
		if(inv_flags.test(EXEC_KEY))
			buffer << " It has on it " << exec_key_desc;

		for(ObjList::iterator iter = obj_list.begin();iter != obj_list.end();++iter)
		{
			if((*iter)->IsClipped())
			{
				buffer << " It also has " << (*iter)->c_str() << " on it. " << (*iter)->Desc();
				break;
			}
		}	
		buffer << "\n";
		player->Send(buffer);
	}
}

int	Inventory::DisplayList(std::ostringstream& buffer)
{
	int	size = obj_list.size();
	if(size == 0)
		return(0);
	for(ObjList::iterator iter = obj_list.begin();iter != obj_list.end();++iter)
	{
		buffer << "   " << (*iter)->c_str();
		if((*iter)->IsWorn())
			buffer << " [wearing]";
		if((*iter)->IsCarried())
			buffer << " [carried]";
		if((*iter)->IsClipped())
			buffer << " [on keyring]";
		if((*iter)->NoTeleport())
			buffer << " [no teleport]";
		buffer << "\n";
	}
//	buffer << size << " objects\n";
	return(size);
}

void	Inventory::DisplayPersonal(std::ostringstream& buffer)
{
	buffer << "You check out your personal kit - ";

	for(int count = 0;count < MAX_INV_FLAGS;++count)
	{
		if(inv_flags.test(count))
		{
			switch(count)
			{
				case	COMM:
					if(inv_flags.test(PRICE_CHECK_PREMIUM))
						buffer <<  "a comm unit (the screen shows a premium ticker logo),";
					else
						buffer << inv_names[COMM];
					break;

				case	PRICE_CHECK_PREMIUM:
				case	KEYRING:
				case	EXEC_KEY:								break;

				case	TP_1:
					buffer << inv_names[count] << " (";
					buffer << (31 - (std::time(0) - tp_rental)/ONE_DAY) << " days left),";
					break;
				
				default:	buffer << inv_names[count];	break;
			}
		}
	}

	DisplayCerts(buffer);
	buffer << " and that seems to be it.\n";
}

int	Inventory::Display2Watcher(Player *player,std::ostringstream& buff)
{
	std::ostringstream	buffer;

	DisplayKeyring(player,false);

	bool	is_wearing = false;
	buffer << "\n" << owner << " is wearing";
	for(ObjList::iterator iter = obj_list.begin();iter != obj_list.end();++iter)
	{
		if((*iter)->IsWorn())
		{
			if(!is_wearing)
				buffer << ":\n";
			else
				buffer.str("");

			buffer << (*iter)->c_str(FedObject::UPPER_CASE) << ". " << (*iter)->Desc();
			is_wearing = true;
			player->Send(buffer);
		}
	}

	buffer.str("");
	bool	is_carrying = false;
	buffer << "\n" << owner << " is carrying";
	for(ObjList::iterator iter = obj_list.begin();iter != obj_list.end();++iter)
	{
		if((*iter)->IsCarried())
		{
			if(!is_carrying)
				buffer << ":\n";
			else
				buffer.str("");

			buffer << (*iter)->c_str(FedObject::UPPER_CASE) << ". " << (*iter)->Desc();
			is_carrying = true;
			player->Send(buffer);
		}
	}

	return(0);
}

void	Inventory::Doff(Player *player,const std::string& obj_name)
{
	std::ostringstream	buffer;
	bool indefinate = ((obj_name[0] == 'a') || (obj_name[0] == 'e') || (obj_name[0] == 'i')  ||
													(obj_name[0] == 'o') || (obj_name[0] == 'u'));
	FedObject	*obj = Find(obj_name);
	if(obj == 0)
	{
		buffer << "You don't have ";
		if(indefinate)
			buffer << "an ";
		else
			buffer << "a ";
		buffer << obj_name << "!\n";
	}
	else
	{
		if(!obj->IsWorn())
			buffer << "You're not wearing the " << obj_name << "!\n";
		else
		{
			obj->ResetFlag(Object::WORN);
			buffer << "You remove the " << obj_name << " and store it out of sight.\n";
		}
	}
	player->Send(buffer);
}

bool	Inventory::HasTeleporter(int which)
{
	if(inv_flags.test(TP_31))
		return(true);
	if((which == TP_1) && (inv_flags.test(TP_1)))
		return(true);
	return(false);
}

bool	Inventory::IsInInventory(const std::string& star_name,
						const std::string& map_name,const std::string& id_name)
{
	for(ObjList::iterator iter = obj_list.begin();iter != obj_list.end();++iter)
	{
		if(((*iter)->ID() == id_name) || ((*iter)->Name() == id_name))
		{
			if((*iter)->IsHomeMap(star_name,map_name))
				return(true);
		}
	}
	return(false);
}

void	Inventory::MakeFluff(std::ostringstream& buffer)
{
	static const int 	MAX_FLUFF = 15;
	buffer << "You also discover ";
	int fluff = (rand() % MAX_FLUFF) + 1;
	buffer << Game::system->GetMessage("player","showinventory",fluff);
}

void	Inventory::NewRemoteCert(Player *player)
{
	price_check_sub =  std::time(0);
	std::ostringstream	buffer;
	buffer << player->Name() << " has purchased a remote check cert";
	WriteLog(buffer);
}

void	Inventory::Pocket(Player *player,const std::string& obj_name)
{
	std::ostringstream	buffer;
	bool indefinate = ((obj_name[0] == 'a') || (obj_name[0] == 'e') || (obj_name[0] == 'i')  ||
													(obj_name[0] == 'o') || (obj_name[0] == 'u'));
	FedObject	*obj = Find(obj_name);
	if(obj == 0)
	{
		buffer << "You don't have ";
		if(indefinate)
			buffer << "an ";
		else
			buffer << "a ";
		buffer << obj_name << "!\n";
	}
	else
	{
		if(!obj->IsCarried())
			buffer << "You're not carrying the " << obj_name << "!\n";
		else
		{
			obj->ResetFlag(Object::CARRIED);
			buffer << "You remove the " << obj_name << " and store it in a convenient pocket.\n";
		}
	}
	player->Send(buffer);
}

void	Inventory::ProcessTimeCerts()
{
	time_t	now = std::time(0);

	// leave these two tests in to clean up uninitialise
	// fields that might still be around :(
	if((31 - (now - customs_cert)/ONE_DAY) > 41)
		customs_cert = 0;
	if((31 - (now - price_check_sub)/ONE_DAY) > 41)
		price_check_sub = 0;

	if(customs_cert != 0)
	{
		if((now - customs_cert) >= ONE_MONTH)
			customs_cert = 0;
	}
	if(price_check_sub != 0)
	{
		if((now - price_check_sub) >= ONE_MONTH)
			price_check_sub = 0;
	}
}

FedObject	*Inventory::RemoveObject(const std::string & name)
{
	return(RemoveListObject(name));
}

FedObject	*Inventory::RemoveObjectIDName(const std::string& id_name)
{
	std::string	lc_name(id_name);
	int length = lc_name.length();
	for(int count = 0;count < length;count++)
		lc_name[count] = std::tolower(lc_name[count]);
	for(ObjList::iterator iter = obj_list.begin();iter != obj_list.end();++iter)
	{
		if(((*iter)->LcName() == lc_name) || ((*iter)->ID() == id_name))
		{
			FedObject *object = (*iter);
			obj_list.erase(iter);
			return(object);
		}
	}
	return(0);
}

void Inventory::SetTpRental()
{
	inv_flags.set(TP_1);
	tp_rental = std::time(0);
}

void	Inventory::Store(const std::string& owner,DBObject *object_db)
{
	object_db->DeleteStoredObjects(owner,FedObject::IN_INVENTORY);
	ObjList::iterator iter;
	for(iter = obj_list.begin();iter != obj_list.end();++iter)
		object_db->StoreObjectToDB(owner,dynamic_cast<const Object *>(*iter));
}

void	Inventory::Unclip(Player *player)
{
	for(ObjList::iterator iter = obj_list.begin();iter != obj_list.end();++iter)
	{
		if((*iter)->IsClipped())
		{
			(*iter)->ResetFlag(Object::CLIPPED);
			player->Send("Only a key is now left on the keyring\n");
			return;
		}
	}
	player->Send("Nothing to unclip!\n");
}

void	Inventory::Wear(Player *player,const std::string& obj_name)
{
	std::ostringstream	buffer;
	bool indefinate = ((obj_name[0] == 'a') || (obj_name[0] == 'e') || (obj_name[0] == 'i')  ||
													(obj_name[0] == 'o') || (obj_name[0] == 'u'));
	FedObject	*obj = Find(obj_name);
	if(obj == 0)
	{
		buffer << "You don't have ";
		if(indefinate)
			buffer << "an ";
		else
			buffer << "a ";
		buffer << obj_name << "!\n";
	}
	else
	{
		obj->ResetFlag(Object::CLIPPED);
		obj->ResetFlag(Object::CARRIED);
		obj->SetFlag(Object::WORN);
		buffer << "You take " << obj->c_str() << " out of your inventory and put it on.\n";
	}
	player->Send(buffer);
}

int	Inventory::WeightCarried()
{
	int total = 0;
	for(ObjList::iterator iter = obj_list.begin();iter != obj_list.end();++iter)
		total += (*iter)->Weight();
	return(total);
}

int	Inventory::XMLDisplay(Player *player,std::ostringstream& buff)
{
	std::ostringstream buffer;
	std::string	text;
	AttribList attribs;

	if(player->Name() != owner)
		return(XMLDisplay2Watcher(player));
	XMLDisplayPersonal(player);
	XMLDisplayKeyring(player,true);

	if(Size() > 0)
	{
		text = "You have with you:";
		player->Send(text,OutputFilter::EXAMINE,attribs);
		XMLDisplayList(player);
		buffer.str("");
		MakeFluff(buffer);
		text = buffer.str();
		player->Send(text,OutputFilter::EXAMINE,attribs);
	}
	return(Size());
}

void	Inventory::XMLDisplayKeyring(Player *player,bool self)
{
	if(inv_flags.test(KEYRING) || inv_flags.test(EXEC_KEY))
	{
		std::ostringstream buffer;
		std::string	text;
		AttribList attribs;

		if(inv_flags.test(KEYRING))
		{
			if(self)
				buffer << "You are" << keyring_desc;
			else
				buffer << owner << " is" << keyring_desc;
		}
		if(inv_flags.test(EXEC_KEY))
			buffer << " It has on it " << exec_key_desc;
		for(ObjList::iterator iter = obj_list.begin();iter != obj_list.end();++iter)
		{
			if((*iter)->IsClipped())
			{
				buffer << " It also has " << (*iter)->c_str() << " on it. " << (*iter)->Desc();
				break;
			}
		}
		text = buffer.str();
		player->Send(text,OutputFilter::EXAMINE,attribs);
	}
}

int	Inventory::XMLDisplayList(Player *player)
{
	int	size = obj_list.size();
	if(size == 0)
		return(0);

	std::ostringstream buffer;
	std::string	text;
	AttribList attribs;

	for(ObjList::iterator iter = obj_list.begin();iter != obj_list.end();++iter)
	{
		buffer.str("");
		buffer << "   " << (*iter)->c_str();
		if((*iter)->IsWorn())
			buffer << " [wearing]";
		if((*iter)->IsCarried())
			buffer << " [carried]";
		if((*iter)->IsClipped())
			buffer << " [on keyring]";
		text = buffer.str();
		player->Send(text,OutputFilter::EXAMINE,attribs);
	}

	buffer.str("");
	buffer << size << " objects";
	text = buffer.str();
	player->Send(text,OutputFilter::EXAMINE,attribs);

	return(size);
}

void	Inventory::XMLDisplayPersonal(Player *player)
{
	std::ostringstream buffer;
	std::string	text;
	AttribList attribs;

	buffer << "You check out your personal kit - ";

	for(int count = 0;count < MAX_INV_FLAGS;++count)
	{
		if(inv_flags.test(count))
		{
			switch(count)
			{
				case	COMM:
					if(inv_flags.test(PRICE_CHECK_PREMIUM))
						buffer <<  "a comm unit (the screen glows with a premium ticker logo),";
					else
						buffer << inv_names[COMM];
					break;

				case	PRICE_CHECK_PREMIUM:
				case	KEYRING:
				case	EXEC_KEY:								break;

				case	TP_1:
					buffer << inv_names[count] << " (";
					buffer << (31 - (std::time(0) - tp_rental)/ONE_DAY) << " days left),";
					break;

				default:	buffer << inv_names[count];	break;
			}
		}
	}

	DisplayCerts(buffer);
	buffer << " and that seems to be it.";
	text = buffer.str();
	player->Send(text,OutputFilter::EXAMINE,attribs);
}

int	Inventory::XMLDisplay2Watcher(Player *player)
{
	std::ostringstream buffer;
	std::string	text;
	AttribList attribs;

	XMLDisplayKeyring(player,false);

	bool	is_wearing = false;
	buffer << owner << " is wearing";
	for(ObjList::iterator iter = obj_list.begin();iter != obj_list.end();++iter)
	{
		if((*iter)->IsWorn())
		{
			if(!is_wearing)
				buffer << ":\n";
			else
				buffer.str("");

			buffer << (*iter)->c_str(FedObject::UPPER_CASE) << ". " << (*iter)->Desc() << "\n";
			is_wearing = true;
			text = buffer.str();
			player->Send(text,OutputFilter::EXAMINE,attribs);
		}
	}

	buffer.str("");
	bool	is_carrying = false;
	buffer << owner << " is carrying";
	for(ObjList::iterator iter = obj_list.begin();iter != obj_list.end();++iter)
	{
		if((*iter)->IsCarried())
		{
			if(!is_carrying)
				buffer << ":\n";
			else
				buffer.str("");

			buffer << " " << (*iter)->c_str(FedObject::UPPER_CASE) << ". " << (*iter)->Desc() << "\n";
			is_carrying = true;
			text = buffer.str();
			player->Send(text,OutputFilter::EXAMINE,attribs);
		}
	}

	return(0);
}
