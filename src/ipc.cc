/*-----------------------------------------------------------------------
		Copyright (c) Alan Lenton & Interactive Broadcasting 2003-9
	All Rights Reserved. No part of this software may be reproduced,
	transmitted, transcribed, stored in a retrieval system, or translated
	into any human or computer language, in any form or by any means,
	electronic, mechanical, magnetic, optical, manual or otherwise,
	without the express written permission of the copyright holder.
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

const int	IPC::BUFFER_SIZE;
const int	IPC::MAX_ERRORS;
const int	IPC::ERROR_SIZE;
int			IPC::ticks = 0;

IPC::IPC(const char *server)
{
	port = billing_port = -1;

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
	OpenBillingLink();

}

IPC::~IPC()
{
	// shut down billing & listening sockets
	// player sockets should already be shut down
	shutdown(bd,2);
	close(bd);
	shutdown(ld,2);
	close(ld);
}


void	IPC::BillingLogOn()
{
	static const std::string	b_error("Unknown text from billing server");
	static const char	*logon = "BILL_Login|fed2d|Highway61|\n";

	char	buffer[BUFFER_SIZE];
	int	index = 0;
	for(int count = 0;count < 100;count++);
	{
		while(read(bd,&buffer[index],1) == 1)
		{
			if(index >= (BUFFER_SIZE - 1))
			{
				std::cerr << "Unknown text from billing server" << std::endl;
				shutdown(ld,SHUT_RDWR);
				close(ld);
				shutdown(bd,SHUT_RDWR);
				close(bd);
				std::exit(EXIT_FAILURE);
			}

			if(buffer[index] == '\n')
			{
				buffer[index] = '\0';
				if(std::strcmp("BILL_OK|",buffer) == 0)
				{
					int flags = fcntl(bd,F_GETFD);
					fcntl(bd,F_SETFD,flags | FD_CLOEXEC);
					flags = fcntl(bd,F_GETFL,0);
					fcntl(bd,F_SETFL,flags | O_NONBLOCK);
					FD_SET(bd,&descs);
					if(bd > last)
						last = bd;
					std::ostringstream	mssg;
					mssg << "Logged in to billing server at port " << billing_port;
					WriteLog(mssg);
					return;
				}

				if(std::strcmp("BILL_Start|",buffer) == 0)
				{
					write(bd,logon,std::strlen(logon));
					index = 0;
					continue;	// while(...)
				}
			}
			index++;
		}
	}
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
//			if(FD_ISSET(count,&descs) && (ticks > (input_ticks[count] + 2)))
			if(FD_ISSET(count,&descs) && (ticks != input_ticks[count]))
				FD_SET(count,&readfds);
		}
		FD_SET(bd,&readfds);
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

void	IPC::OpenBillingLink()
{
	static const std::string	error_mssg("Incorrect data in billing.dat. Should be in the format 'full host name:port number'");

	// where is the billing server?
	char	file_name[MAXNAMLEN +1];
	std::snprintf(file_name,MAXNAMLEN +1,"%s/data/billing.dat",HomeDir());
	std::ifstream	file(file_name,std::ios::in);
	if(!file)
	{
		std::cerr << "Unable to open 'data/billing.dat" << std::endl;
		shutdown(ld,SHUT_RDWR);
		close(ld);
		std::exit(EXIT_FAILURE);
	}

	char	buffer[BUFFER_SIZE];
	file.getline(buffer,BUFFER_SIZE);
	char	*token = std::strtok(buffer,":");
	if(token == 0)
	{
		std::cerr << error_mssg << std::endl;
		shutdown(ld,SHUT_RDWR);
		close(ld);
		std::exit(EXIT_FAILURE);
	}
	char host_name[80];
	std::strncpy(host_name,token,80);
	host_name[79] = '\0';
	if((token = std::strtok(0,":\n")) == 0)
	{
		std::cerr << error_mssg << std::endl;
		shutdown(ld,SHUT_RDWR);
		close(ld);
		std::exit(EXIT_FAILURE);
	}
	int	port_number = std::atoi(token);
	file.close();

	// ...now we are ready to open a connection to it...
	hostent		*hptr;
	sockaddr_in	sin;

	sin.sin_family = AF_INET;
	sin.sin_port = htons((u_short)port_number);

	// ...map the host name to the address...
	if((hptr = gethostbyname(host_name)) == 0)
	{
		std::cerr << "Can't find the host name given by billing.dat" << std::endl;
		shutdown(ld,SHUT_RDWR);
		close(ld);
		std::exit(EXIT_FAILURE);
	}
	std::memcpy((char *)&sin.sin_addr,hptr->h_addr,hptr->h_length);

	// ...open the socket...
	if((bd = socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		std::cerr << "Can't open a socket to billing" << std::endl;
		shutdown(ld,SHUT_RDWR);
		close(ld);
		std::exit(EXIT_FAILURE);
	}

	//	...and connect to billing server!
	if(connect(bd,(sockaddr *)&sin,sizeof(sin)) < 0)
	{
		std::cerr << "Error connecting socket to billing - errno is " << errno << std::endl;
		shutdown(ld,SHUT_RDWR);
		close(ld);
		shutdown(bd,SHUT_RDWR);
		close(bd);
		std::exit(EXIT_FAILURE);
	}

	billing_port = port_number;
	BillingLogOn();

}

void	IPC::Read(int sd)
{
	int	read_type, count = 0;
	if(sd == bd)
		read_type = BILLING;
	else
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

void	IPC::Send2Billing(const std::string& text)
{
	char	buffer[BUFFER_SIZE];
	std::strncpy(buffer,text.c_str(),BUFFER_SIZE);
	buffer[BUFFER_SIZE -1] = '\0';
	Send2Billing(buffer);
}

void	IPC::Send2Billing(const char *text)
{
	write(bd,text,std::strlen(text));
}

