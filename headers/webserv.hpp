/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: efranco <efranco@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/25 20:54:02 by nmartin           #+#    #+#             */
/*   Updated: 2026/02/22 20:39:20 by efranco          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <unistd.h>
#include <poll.h>
#include <map>
#include <sstream>
#include <fstream>
#include <fcntl.h>
#include <csignal>
#include <ctime>
#include <sys/time.h>
#include <unistd.h>

#include "ConfigParser.hpp"
#include "ConfigTokenizer.hpp"

#define MAX_FDS 1024

extern volatile sig_atomic_t	flag_signal;

void turn_signal(int sig);

class Connection;

class	Data
{
	public:

	Data();
	~Data();
	void							setServers(std::vector<ConfigServer> _servers);
	void							setAddrinfo(int index);
	struct addrinfo					*getAddrinfo(int index);
	void							addListener(int index);
	void							newClient(int fd, int serverIndex);
	void							clientRequest(int index, int serverIndex);
	void							pollLoop(void);
	int								getFd(int index);
	void							clean(void);
	void							exit(int status);
	void							exitError(void);
	std::vector<ConfigServer>&		getServers() { return _servers; }

	private:

	std::vector<struct addrinfo	*>	_addrinfo;
	struct pollfd					_fds[MAX_FDS];
	int								_clientServer[MAX_FDS];
	std::map<int, Connection>		_connections;
	int								_fdsNbr;
	std::vector<ConfigServer>		_servers;
	std::string						_root;
};

std::string	getTimestamp(void);
