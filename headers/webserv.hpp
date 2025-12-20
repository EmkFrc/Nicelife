/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmartin <nmartin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/25 20:54:02 by nmartin           #+#    #+#             */
/*   Updated: 2025/12/11 20:09:51 by nmartin          ###   ########.fr       */
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

#include <ctime>
#include <sys/time.h>
#include <unistd.h>

#define MAX_FDS 250

class Connection;

class	Data
{
	public:
	
	Data();
	~Data();
	void			setAddrinfo(void);
	struct addrinfo	*getAddrinfo(void);
	void			addListener(void);
	void			newClient(int fd);
	void			clientRequest(int index);
	void			pollLoop(void);
	int				getFd(int index);
	void			clean(void);
	void			exit(int status);
	void			exitError(void);
	
	private:

	struct addrinfo	*			_addrinfo;
	struct pollfd				_fds[MAX_FDS];
	std::map<int, Connection>	_connections;
	int							_fdsNbr;
};

std::string	getTimestamp(void);
