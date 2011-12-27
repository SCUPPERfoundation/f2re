/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-9
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
-----------------------------------------------------------------------*/

#ifndef IPC_H
#define IPC_H

#include <string>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/unistd.h>

#include "misc.h"

class	IPC
{
public:
	static const int	BUFFER_SIZE = 1024;
	enum	{ CLOSED, NEW, READ, LOST, BILLING };	// status used for callback

private:
	static const int	MAX_ERRORS = 2000;	// max number of select() non-system call errors
	static const int	ERROR_SIZE = 256;

	static int	ticks;

	int		ld;						// listen socket descriptor
	int		port;						// listening port number (-1 = not listening)
	int		billing_port;			// port being used for the billing service
	int		bd;						// billing socket descriptor
	int		last;						// highest numbered descriptor in use
	int		num_errors;				// number of non-system call select()errors
	fd_set	descs;					// read socket descriptor set
	char		error[ERROR_SIZE];	// description of last error
	char		input[BUFFER_SIZE];	//	input read buffer
	int		input_ticks[Game::MAX_PLAYERS + 100];	// tick value for last input on each skt desc

	void	BillingLogOn();
	void	OpenBillingLink();
	void	Read(int sd);

public:
	static void	IncTicks()			{ ++ticks;					}

	IPC(const char *server);
	~IPC();
	
	const char	*Error()				{ return(error);			}
	int	BillingPort()				{ return(billing_port);	}
	int	Port()						{ return(port);			}

	void	ClearSocket(int skt_desc);	
	void	GetInput();
	void	Send2Billing(const std::string& text);
	void	Send2Billing(const char *text);
};

#endif
