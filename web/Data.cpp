/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Data.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: efranco <efranco@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/28 16:02:50 by nmartin           #+#    #+#             */
/*   Updated: 2026/02/22 14:52:11 by efranco          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "request.hpp"

Data::Data() : _fdsNbr(0)
{
	for (int i = 0; i < MAX_FDS; i++)
	{
		_fds[i].fd = -1;
		_clientServer[i] = -1;
	}
}

Data::~Data()
{
}

void	Data::setServers(std::vector<ConfigServer> servers)
{
	_servers = servers;
	_addrinfo.resize(_servers.size(), NULL);
}

void	Data::setAddrinfo(int index)
{
	int				status;
	struct addrinfo	hints;
	std::stringstream ss;
	ss << _servers[index].getPort();
	std::string port = ss.str();

	this->_addrinfo[index] = NULL;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	status = getaddrinfo(_servers[index].getHost().c_str(), port.c_str(), &hints, &this->_addrinfo[index]);
	if (status != 0)
	{
		std::cerr << "Error: getaddrinfo: " << gai_strerror(status) << std::endl;
		std::exit(status);
	}
}

struct addrinfo	*	Data::getAddrinfo(int index)
{
	return (_addrinfo[index]);
}

void	Data::addListener(int index)
{
	int	flags;

	_fds[_fdsNbr].fd = socket(_addrinfo[index]->ai_family, _addrinfo[index]->ai_socktype, _addrinfo[index]->ai_protocol);
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
	if (bind(_fds[_fdsNbr].fd, _addrinfo[index]->ai_addr, _addrinfo[index]->ai_addrlen) == -1)
		exitError();
	if (listen(_fds[_fdsNbr].fd, 1024) == -1)
		exitError();
	_servers[index].setListener(_fdsNbr);
	_fdsNbr++;
}

void	Data::newClient(int listener, int serverIndex)
{
	struct sockaddr_storage their_addr;
    socklen_t 				addr_size(sizeof(their_addr));
	int						newFd;

	newFd = accept(listener, (struct sockaddr *)&their_addr, &addr_size);//faire une loop normalement ?
	if (newFd == -1)
		exitError();
	int flags = fcntl(newFd, F_GETFL, 0);
    if (flags == -1)
    {
        std::cerr << "fcntl(F_GETFL) failed on fd=" << newFd << std::endl;
        close(newFd);
        return;
    }

    if (fcntl(newFd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        std::cerr << "fcntl(F_SETFL) failed on fd=" << newFd << std::endl;
        close(newFd);
        return;
    }
	for (int i = 0; i < MAX_FDS; i++)
	{
		if (_fds[i].fd == -1)
		{
			_fds[i].fd = newFd;
			_fds[i].events = POLLIN;
			_fds[i].revents = 0;
			_fdsNbr++;
			_clientServer[i] = serverIndex;
			// std::cout << "new Client!" << std::endl;
			return ;
		}
	}
	std::cerr << "Error: Server is full!" << std::endl;
}

void	Data::clientRequest(int index, int serverIndex)
{
	std::string				request;

	if (_fds[index].fd < 0)
    {
        std::cerr << "Invalid fd at index " << index << std::endl;
        return;
    }

	if (_connections.find(_fds[index].fd) == _connections.end())
	{
		_connections[_fds[index].fd] = Connection(&_fds[index]);
		_connections[_fds[index].fd].setConf(_servers[serverIndex]);
	}
	if (_fds[index].revents & POLLIN)
	{
		_connections[_fds[index].fd].pollIn();
	}
	else if (_fds[index].revents & POLLOUT)
	{
		_connections[_fds[index].fd].pollOut();
	}
	if (_connections[_fds[index].fd].closeRequest())
	{
		_connections.erase(_fds[index].fd);
	    close(_fds[index].fd);
	    _fds[index].fd = -1;
	    _fdsNbr--;
	}
}

void	Data::pollLoop(void)
{
	int						pollV;

	while (flag_signal == 1)
	{
		for (int i = 0; i < MAX_FDS; i++)
    		_fds[i].revents = 0;
		pollV = poll(_fds, MAX_FDS, 1000);
		if (flag_signal == 0)
			break;
		for (int serverIndex = 0; serverIndex < (int)_servers.size(); serverIndex++)
		{
			if (_fds[_servers[serverIndex].getListener()].revents & POLLIN)
			{
				std::cout << "client" << serverIndex << std::endl;
				newClient(_fds[_servers[serverIndex].getListener()].fd, serverIndex);
				pollV--;
			}
		}
		for (int i = 0; i < MAX_FDS; i++)
		{

			if (pollV == 0)
				break;
			if (_fds[i].fd == -1 || _clientServer[i] == -1)
				continue;
			else if (_connections.find(_fds[i].fd) != _connections.end() && _connections[_fds[i].fd].getExec())
				_connections[_fds[i].fd].executing_cgi();
			else if (_fds[i].revents & POLLIN)
			{
				clientRequest(i, _clientServer[i]);
				pollV--;
			}
			else if (_fds[i].revents & POLLOUT)
			{
				clientRequest(i, _clientServer[i]);
				pollV--;
			}
			else if (_fds[i].revents & POLLHUP || _fds[i].revents & POLLERR)
			{
				close(_fds[i].fd);
				_fds[i].fd = -1;
				_clientServer[i] = -1;
				pollV--;
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
	for (int serverIndex = 0; serverIndex < (int)_servers.size(); serverIndex++)
	{
		if(_addrinfo[serverIndex])
			freeaddrinfo(_addrinfo[serverIndex]);
	}
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
