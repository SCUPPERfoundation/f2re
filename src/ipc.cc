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

#include "ipc.h"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <fstream>
#include <iostream>
#include <sstream>

#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/dir.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "misc.h"
#include "player_index.h"

int			IPC::ticks = 0;

IPC::IPC(const char *server)
{
	// open listen socket...
	if((ld = socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		std::cerr << "Can't open listen socket" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	//	...bind it to our 'well known' port address...
	int		set_flag = 1;
	servent	*sptr;
	setsockopt(ld,SOL_SOCKET,SO_REUSEADDR,&set_flag,sizeof(set_flag));
	if((sptr = getservbyname(server,"tcp")) == 0)
	{
		std::cerr << server << " is not in /etc/services" << std::endl;
		shutdown(ld,SHUT_RDWR);
		close(ld);
		std::exit(EXIT_FAILURE);
	}

	sockaddr_in	name;
	name.sin_family = AF_INET;
	name.sin_port = sptr->s_port;
	name.sin_addr.s_addr = INADDR_ANY;

	if(bind(ld,(sockaddr *)&name,sizeof(name)) < 0)
	{
		std::cerr << "Can't bind listen socket - errno is " << errno << std::endl;
		shutdown(ld,SHUT_RDWR);
		close(ld);
		std::exit(EXIT_FAILURE);
	}

	port = htons(name.sin_port);

	//	...set the relevent flags...
	int	flags = fcntl(ld,F_GETFD);
	fcntl(ld,F_SETFD,flags | FD_CLOEXEC);
	flags = fcntl(ld,F_GETFL,0);
	fcntl(ld,F_SETFL,flags | O_NONBLOCK);

	// and, finally, listen on our socket!
	if(listen(ld,50) < 0)
	{
		std::cerr << "Error setting socket to listen - errno is " << errno << std::endl;
		shutdown(ld,SHUT_RDWR);
		close(ld);
		std::exit(EXIT_FAILURE);
	}

	last = ld;
	num_errors = 0;
	FD_ZERO(&descs);
	FD_SET(ld,&descs);
	error[0] = input[0] = '\0';
	for(int count = 0;count < (Game::MAX_PLAYERS + 100);input_ticks[count++] = 0)
		;
}

IPC::~IPC()
{
	shutdown(ld,2);
	close(ld);
}

void	IPC::ClearSocket(int sd)
{
	if(!FD_ISSET(sd,&descs))
		return;

	shutdown(sd,SHUT_RD);
	close(sd);

	FD_CLR(sd,&descs);
	if(sd >= last)
	{
		int	count;
		for(count = last = 0;count < FD_SETSIZE;count++)
		{
			if(FD_ISSET(count,&descs))
				last = count;
		}
	}
}

void	IPC::GetInput()
{
	static const std::string	log_str("SIGTERM signal received");

	sigset_t	set;
	sigemptyset(&set);
	sigaddset(&set,SIGALRM);
	sigaddset(&set,SIGTERM);
	sigaddset(&set,SIGUSR1);
	sigaddset(&set,SIGUSR2);
	sigaddset(&set,SIGPIPE);
	sigaddset(&set,SIGHUP);

	fd_set		readfds;
	timeval		time_val;
	socklen_t	len;
	sockaddr_in	cliaddr;
	int			select_status, sd, flags;
	char			buffer[BUFFER_SIZE];
	for(;;)
	{
		time_val.tv_sec = 0;			// have select return in half second (== macro delays time)
		time_val.tv_usec = 500;
		FD_ZERO(&readfds);
		for(int count = 0; count <= (last + 1);count++)
		{
			if(FD_ISSET(count,&descs) && (ticks != input_ticks[count]))
				FD_SET(count,&readfds);
		}
		FD_SET(ld,&readfds);

		sigprocmask(SIG_UNBLOCK,&set,0);		// allow signals only while in select()

		do
		{
			if(Game::wrap_up != 0)
			{
				sigprocmask(SIG_BLOCK,&set,0);
				WriteLog(log_str);
				return;
			}
			select_status = select(last + 1,&readfds,0,0,&time_val);
		} while((select_status == -1) && (errno == EINTR));	//system call interrupt

		sigprocmask(SIG_BLOCK,&set,0);	// block signals while processing input
		if(select_status < 0)
		{
			std::cerr << "Select status is " << select_status << ", errno is " << errno << std::endl;
			if(++num_errors > MAX_ERRORS)
			{
				std::cerr << "Maximum number of select() errors exceeded - bailing out" << std::endl;
				Game::wrap_up = 1;
				continue;
			}
		}
		else
			num_errors = 0;

		if(select_status > 0)		// stuff to process
		{
			// Check for new connections...
			if(FD_ISSET(ld,&readfds))
			{
				len = sizeof(cliaddr);
				if((sd = accept(ld,(sockaddr *)&cliaddr,&len)) < 0)
				{
					std::cerr << "Accept error - errno " << errno << std::endl;
					if(++num_errors > MAX_ERRORS)
					{
						std::cerr << "Maximum number of select() errors exceeded - bailing out" << std::endl;
						Game::wrap_up = 1;
						continue;
					}
				}
				else
				{
					inet_ntop(AF_INET,&cliaddr.sin_addr,buffer,sizeof(buffer));
					if(!IPCCallBack(NEW,sd,buffer))	// problem - close the socket
						close(sd);
					else
					{
						// ...set descriptor to non-blocking and add to selector set...
						flags = fcntl(sd,F_GETFD);
						fcntl(sd,F_SETFD,flags | FD_CLOEXEC);
						flags = fcntl(sd,F_GETFL,0);
						fcntl(sd,F_SETFL,flags | O_NONBLOCK);
						input_ticks[sd] = ticks;

						FD_SET(sd,&descs);
						if(sd > last)
							last = sd;
					}
				}
			}

			// ...and check the rest of the connections for input.
			for(int count = 0;count <= last;count++)
			{
				if(count == ld)	// skip the listen socket
					continue;
				if(FD_ISSET(count,&readfds))
					Read(count);
			}
		}
	}
}

void	IPC::Read(int sd)
{
	int	read_type, count = 0;
	read_type = READ;
	input_ticks[sd] = ticks;

	// read in the first char and make sure the line hasn't been dropped
	if(read(sd,&input[count++],1) < 1)
	{
		if(errno != 4)
			Game::player_index->ReportSocketError(sd,errno);
		char buffer[80];
		std::strcpy(buffer,"IPC - line lost"); // stop compiler whining about const/non-const conversions :)
		IPCCallBack(LOST,sd,buffer);
		ClearSocket(sd);
		return;
	}
	else
	{
		if(input[count -1] == '\r')
			count--;
		else
		{
			if(input[count -1] == '\n')
			{
				input[count] = '\0';
				if(!IPCCallBack(read_type,sd,input))
					ClearSocket(sd);
				return;
			}
		}
	}

	// now read in the rest of the input, stopping if a '\n' is found
	for(;count < (BUFFER_SIZE -2);count++)
	{
		if(read(sd,&input[count],1) <= 0)	// end of input - send for processing
		{
			input[count] = '\0';
			if(!IPCCallBack(read_type,sd,input))
				ClearSocket(sd);
			return;
		}

		if(input[count] == '\r')
		{
			if(--count < 0)
				count = 0;
			continue;
		}

		if(input[count] == '\n')	// <eol> stop reading and send for processing
		{
			input[count +1] = '\0';
			if(!IPCCallBack(read_type,sd,input))
				ClearSocket(sd);
			return;
		}

	}

	// oversized line
	std::cerr << "Oversized line received" << std::endl;
	input[BUFFER_SIZE -1] = '\0';
	std::cerr << input;
}

