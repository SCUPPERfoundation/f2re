/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-7
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef BILLING_H
#define BILLING_H

#include <string>
#include <vector>

class Player;

class	Billing									// Handles billing/account queries/replies
{
public:
	enum	{ TRIAL, ACTIVE, COMP, SUSPENDED, CANCELLED, BLOCKED, UNKNOWN	};	// IB account status

private:
	enum	
	{ 
		B_RETVAL, B_NUM, B_AC_NAME, B_EMAIL, B_STATUS, B_CREATED,
		B_TERMINATED, B_LAST_PYT, B_PAYMENT_DAY, B_BILL_DATE, B_CC_NUM, 
		B_CC_EXPIRE, B_CC_CHECK, B_CURRENT_TIME_ON, B_CURRENT_GAME, 
		B_LAST_ON, B_LAST_GAME, B_POST_CODE, B_BILLIED, B_PYT_REF
	};												// BILL_SELECT return fields
	enum
	{
		NO_PROC, UPDATE_EMAIL, DISPLAY_EMAIL, UPDATE_PWD, 
		DISPLAY_ACC, ADMIN_EMAIL, ADMIN_PWD, ADMIN_STATUS,
		SET_ACC_STATUS, SEND_EMAIL, DISPLAY_ACCS_BY_EMAIL
	};												// Billing status 
		

	Player		*owner;
	std::string ib_account;
	std::string password;					// only in memory for current players
	int	sd;									// socket descriptor for our player
	int	status;								// what we are currently processing
	int	acc_status;							// current ib account status
	std::vector<std::string>	tokens;	//	tokenised input
	
	std::string em_reply_to;				// stored for SendEMail()
	std::string em_subject;
	std::string em_filename;

	bool	StatusCheck(const std::string& status_val);
	void	AccountStatus(const std::string& line);
	void	DisplayAccount(const std::string& line);
	void	DisplayAccountsByEmail(const std::string& reply);
	void	DisplayEMail(const std::string& line);
	void	ParseBillSelect(const std::string& line);
	void	SelectPlayer(const std::string *name = 0);
	void	SendEMail(const std::string& line);

public:
	Billing(Player *owner,const std::string& account,const std::string& pwd,int skt);
	~Billing()		{	}

	int	AccountStatus();

	void	AdminChange(const std::string& which,const std::string& ib_name,const std::string& new_value);
	void	GetAccount(const std::string& name);
	void	GetAccountByEmail(const std::string& e_mail);
	void	GetEMail();
	void	ProcessReply(const std::string& reply);
	void	SendEMail(const std::string& reply_to,const std::string& subject,const std::string& filename);
	void	UpdateEMail(const std::string& pwd,const std::string& address);
	void	UpdatePassword(const std::string& old_pw,const std::string& new_pw);
	void	UpdateStatusCache();
};

#endif
