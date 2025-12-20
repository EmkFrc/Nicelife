/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Data.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmartin <nmartin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/28 16:02:50 by nmartin           #+#    #+#             */
/*   Updated: 2025/12/16 13:54:07 by nmartin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "request.hpp"

#define PORT "6969"

Data::Data() : _addrinfo(NULL), _fdsNbr(0)
{
	for (int i = 0; i < MAX_FDS; i++)
		_fds[i].fd = -1;
}

Data::~Data()
{
}

void	Data::setAddrinfo(void)
{
	int				status;
	struct addrinfo	hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	status = getaddrinfo(NULL, PORT, &hints, &this->_addrinfo);
	if (status != 0)
	{
		std::cerr << "Error: getaddrinfo: " << gai_strerror(status) << std::endl;
		std::exit(status);
	}
}

struct addrinfo	*Data::getAddrinfo(void)
{
	return (_addrinfo);
}

void	Data::addListener(void)
{
	int	flags;

	_fds[_fdsNbr].fd = socket(_addrinfo->ai_family, _addrinfo->ai_socktype, _addrinfo->ai_protocol);
	if (_fds[_fdsNbr].fd == -1)
		exitError();
	flags = fcntl(_fds[_fdsNbr].fd, F_GETFL, 0);
	if (flags == -1)
	{
		perror("fcntl F_GETFL");
		return ;
	}
	if (fcntl(_fds[_fdsNbr].fd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		perror("fcntl F_SETFL");
		return ;
	}
	_fds[_fdsNbr].events = POLLIN;
	int yes = 1;
	setsockopt(_fds[_fdsNbr].fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
	if (bind(_fds[_fdsNbr].fd, _addrinfo->ai_addr, _addrinfo->ai_addrlen) == -1)
		exitError();
	if (listen(_fds[_fdsNbr].fd, 10) == -1)
		exitError();
	_fdsNbr++;
}

void	Data::newClient(int listener)
{
	struct sockaddr_storage their_addr;
    socklen_t 				addr_size(sizeof(their_addr));
	int						newFd;
	
	newFd = accept(listener, (struct sockaddr *)&their_addr, &addr_size);//faire une loop normalement ?
	if (newFd == -1)
		exitError();
	for (int i = 0; i < MAX_FDS; i++)
	{
		if (_fds[i].fd == -1)
		{
			_fds[i].fd = newFd;
			_fds[i].events = POLLIN;
			_fdsNbr++;
			std::cout << "new Client!" << std::endl;
			return ;
		}
	}
	std::cerr << "Error: Serveur is full!" << std::endl;
}

void	Data::clientRequest(int index)
{
	std::string				request;

	std::cout << "New request from client N." << index << "!" << _fds[index].fd <<std::endl;
	
	if (_connections.find(_fds[index].fd) == _connections.end())
	{
		_connections[_fds[index].fd] = Connection(&_fds[index]);
		std::cout << "New connection created for fd " << _fds[index].fd << std::endl;
	}
	if (_fds[index].revents & POLLIN)
	{
		std::cout << "pollin !" << std::endl;
		_connections[_fds[index].fd].pollIn();
	}
	else if (_fds[index].revents & POLLOUT)
	{
		std::cout << "pollout !" << std::endl;
		_connections[_fds[index].fd].pollOut();
	}
	std::cout << "finished" <<std::endl;
	if (_connections[_fds[index].fd].closeRequest())
	{
		_connections.erase(_fds[index].fd);
	    close(_fds[index].fd);
	    _fds[index].fd = -1;
	    _fdsNbr--;
		std::cout << index << "closed !" << std::endl;
	}
}

void	Data::pollLoop(void)
{
	int						pollV;

	while (1)
	{
		pollV = poll(_fds, _fdsNbr, 1000);
		if (pollV == -1)
			exitError();
		else if (pollV == 0)
			continue ;
		else if (_fds[0].revents & POLLIN)
		{
			newClient(_fds[0].fd);
			pollV--;
		}
		else
		{
			for (int i = 1; i < MAX_FDS; i++)
			{
				if (pollV == 0)
					break;
				if (_fds[i].fd == -1)
					continue;
				else if (_connections.find(_fds[i].fd) != _connections.end() && _connections[_fds[i].fd].getExec())
					_connections[_fds[i].fd].handle_executing_cgi();
				else if (_fds[i].revents & POLLIN)
				{
					clientRequest(i);
					pollV--;
				}
				else if (_fds[i].revents & POLLOUT)
				{
					clientRequest(i);
					pollV--;
				}
				else if (_fds[i].revents & POLLHUP || _fds[i].revents & POLLERR)
				{
					std::cout << i << "disconnected !" << std::endl;
					close(_fds[i].fd);
					_fds[i].fd = -1;
					pollV--;
				}
			}
		}
	}
}

int	Data::getFd(int index)
{
	return (_fds[index].fd);
}

void	Data::clean(void)
{
	if(_addrinfo)
		freeaddrinfo(_addrinfo);
	for (int i = 0; i < _fdsNbr; i++)
	{
		if (_fds[i].fd != -1)
			close(_fds[i].fd);
	}
}

void	Data::exit(int	status)
{
	clean();
	std::exit(status);
}

void	Data::exitError(void)
{
	clean();
	std::cerr << "Error: " << strerror(errno) << std::endl;
	std::exit(errno);
}