/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton 1985-2017
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

// disk storage structures for player accounts

#ifndef BILLING2ASCII_H
#define BILLING2ASCII_H

#include <ctime>

struct DBAccount					// Disk Image of account record per robert
{
	char		name[24];
	unsigned char	password[16];
	char		email[80];
   
	int	status;
   
	time_t	created;
	int		terminated_d;			// Date account [to be|was] cancelled - day
	int		terminated_m;			// Date account [to be|was] cancelled - month
	int		terminated_y;			// Date account [to be|was] cancelled - year
	time_t	last_payment;
	int		payment_day;			// Day of month to bill
	int		next_bill_date_d;		// Day of next bill date
	int		next_bill_date_m;		// Month of next bill date
	int		next_bill_date_y;		// Year of next bill date
	char		cc_number[16];			// Credit card number.  Held as an array of integers - NOT A STRING.
	char		cc_exp_date[4];		// Credit card expiry date.  Held as cc_number - NOT A STRING.
	char		cc_check_digits[3];	// Credit card check digits.  Held as cc_number - NOT A STRING.
	char		post_code[16];
	
	bool		been_billed;
	char		payment_ref[32];
   
	time_t	current_on;
	time_t	last_on;
	int		current_game;
	int		last_game;
	
public:
	bool		setName( char *name );
};

#endif
