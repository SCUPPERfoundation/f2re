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

#include "misc.h"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <fcntl.h>
#include <pwd.h>
#include <unistd.h>

#include <sys/resource.h>
#include <sys/time.h>

#include "fed.h"


int	main(int argc, char *argv[])
{
	sigset_t	sig_set;

	sigemptyset(&sig_set);
	sigaddset(&sig_set,SIGALRM);
	sigaddset(&sig_set,SIGTERM);
	sigaddset(&sig_set,SIGUSR1);
	sigaddset(&sig_set,SIGUSR2);
	sigaddset(&sig_set,SIGPIPE);
	sigprocmask(SIG_BLOCK,&sig_set,0);

	rlimit	info;
	const char	*ac_name;
	ac_name = "fed2";

	// we want core dumps...
	getrlimit(RLIMIT_CORE,&info);
	info.rlim_cur = info.rlim_max;
	setrlimit(RLIMIT_CORE,&info);

	// get us running under the correct account and in the correct directory
	const struct passwd	*pw = getpwnam(ac_name);
	if(pw == 0)
	{
		std::fprintf(stderr,"Cannot get password structure for '%s' account.\n",ac_name);
		std::fprintf(stderr,"%s\n",std::strerror(errno));
	}
	else
	{
		if(setgid(pw->pw_gid) == -1)
		{
			std::fprintf(stderr,"Cannot set the gid to that of '%s' (%d).\n",ac_name,pw->pw_gid);
			std::fprintf(stderr,"%s\n",std::strerror(errno));
		}
		else
		{
			if(setuid(pw->pw_uid) == -1)
			{
				std::fprintf(stderr,"Cannot set uid to that of 'f%s' (%d).\n",ac_name,pw->pw_uid);
				std::fprintf(stderr,"%s.\n",std::strerror(errno));
			}
			else
			{
#ifdef FEDTEST
				if(chdir("/var/opt/fed2test") == -1)
				{
					std::fprintf(stderr,"Cannot change directory to '/var/opt/fed2test'.\n");
					std::fprintf(stderr,"%s.\n",std::strerror(errno));
				}
#else
				if(chdir(pw->pw_dir) == -1)
				{
					std::fprintf(stderr,"Cannot change directory to '%s' home directory.\n",ac_name);
					std::fprintf(stderr,"%s.\n",std::strerror(errno));
				}
#endif
			}
		}
	}

	getrlimit(RLIMIT_CORE,&info);
	info.rlim_cur = info.rlim_max;
	setrlimit(RLIMIT_CORE,&info);

	HomeDir();

	// make sure we only have one copy running!
	if(!GetRunLock("fed2d.pid"))
	{
		std::fprintf(stderr,"***** Server already running! *****");
		exit(EXIT_FAILURE);
	}

	// we need every file descriptor we can lay hands on :)
	getrlimit(RLIMIT_NOFILE,&info);
	info.rlim_cur = info.rlim_max;
	setrlimit(RLIMIT_NOFILE,&info);

	std::srand(std::time(0));

	if(argc > 1)
	{
		int	opt;
		while((opt = getopt(argc, argv, "l:")) != -1)
		{
			switch(opt)
			{
				case 'l':	Game::load_billing_info = optarg;	break;
			}
		}
	}

	Game::fed = new Fed;
	Game::fed->Run();

	delete Game::fed;
	return(0);
}

