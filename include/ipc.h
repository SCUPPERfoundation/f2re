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
	enum	{ CLOSED, NEW, READ, LOST };	// status used for callback

private:
	static const int	MAX_ERRORS = 2000;	// max number of select() non-system call errors
	static const int	ERROR_SIZE = 256;

	static int	ticks;

	int		ld;						// listen socket descriptor
	int		port;						// listening port number (-1 = not listening)
	int		last;						// highest numbered descriptor in use
	int		num_errors;				// number of non-system call select()errors
	fd_set	descs;					// read socket descriptor set
	char		error[ERROR_SIZE];	// description of last error
	char		input[BUFFER_SIZE];	//	input read buffer
	int		input_ticks[Game::MAX_PLAYERS + 100];	// tick value for last input on each skt desc

	void	Read(int sd);

public:
	static void	IncTicks()			{ ++ticks;					}

	IPC(const char *server);
	~IPC();
	
	const char	*Error()				{ return(error);			}
	int	Port()						{ return(port);			}

	void	ClearSocket(int skt_desc);	
	void	GetInput();
};

#endif
