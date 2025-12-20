/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmartin <nmartin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 23:04:14 by nmartin           #+#    #+#             */
/*   Updated: 2025/12/20 13:22:52 by nmartin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request.hpp"

Connection::Connection() : _fd(NULL), _write_offset(0), _expected_length(0), _executing(false), _close(false)
{
}

Connection::Connection(struct pollfd *fd) : _fd(fd), _write_offset(0), _expected_length(0), _executing(false), _close(false)
{
	_read_buf.reserve(8162);
	_write_buf.reserve(8162);
}

Connection::~Connection()
{
}

void Connection::sendData(void)
{
	size_t n;

	_write_offset = 0;
	while (_write_offset < _write_buf.size())
	{
		n = send(_fd->fd, _write_buf.c_str() + _write_offset, _write_buf.size() - _write_offset, 0);
		if (n > 0)
			_write_offset += n;
		// else
		// {
		// 	//TODO handle error
		// }
	}
	std::cout << std::endl;
}

void	Connection::recvData(void)
{
	char	buffer[BUFFER_SIZE];
	ssize_t	received(0);
	size_t	pos;

	if (_expected_length == 0)
		_read_buf.clear();	
	while (true)
	{
		std ::cout << _fd->fd;
		received = recv(_fd->fd, buffer, BUFFER_SIZE, MSG_DONTWAIT);

		if (received > 0)
		{
			_read_buf.append(buffer, received);
			pos =_read_buf.find("\r\n\r\n");
			if (pos != std::string::npos)
			{
				_env.parse_query_string(_read_buf);
				_env.search_cookie_string(_read_buf);
				_env.extract_method(_read_buf);
				_env.parse_headers(_read_buf);
				return ;
			}
		}
		else if (received == 0)
		{
			if (_expected_length == 0 || _read_buf.size() >= _expected_length)
				_close = true;
			break;
		}
		else
			break;//TODO peut etre dangereux
	}
}

void Connection::requestData(void)
{
	std::istringstream flx(_read_buf);
	std::string line;

	if (std::getline(flx, line))
	{
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		std::istringstream request(line);
		request >> _method >> _uri >> _version;
	}
	if (_read_buf.find("Connection: close") != std::string::npos)
		_close = true;
}

void Connection::pollOut(void)
{
	sendData();
	_expected_length = 0;
	_fd->events = POLLIN;
}

void	Connection::pollIn(void)
{
	recvData();
	
	if (_read_buf.empty())
		return;
	if (_close && _expected_length > 0 && _read_buf.size() < _expected_length)
	{
		std::cerr << "Connection closed prematurely! Expected " << _expected_length 
		          << " bytes but got " << _read_buf.size() << std::endl;
		_expected_length = 0;
		return;
	}
	size_t header_end = _read_buf.find("\r\n\r\n");
	if (header_end == std::string::npos)
	{
		if (_close)
		{
			std::cerr << "Connection closed without complete headers!" << std::endl;
			return;
		}
		return;
	}
	if (_expected_length == 0)
	{
		size_t pos = _read_buf.find("Content-Length: ");
		if (pos != std::string::npos && pos < header_end)
		{
			size_t end = _read_buf.find("\r\n", pos);
			if (end != std::string::npos)
			{
				int content_len = atoi(_read_buf.substr(pos + 16, end - pos).c_str());
				if (content_len > 0)
					_expected_length = header_end + 4 + content_len;
			}
		}
	}
	if (_expected_length > 0 && _read_buf.size() < _expected_length)
	{
		if (_close)
		{
			std::cerr << "Connection closed before receiving all data!" << std::endl;
			_expected_length = 0;
			return;
		}
		_fd->events = POLLIN;
		return;
	}
	std::cout << _read_buf << std::endl;
	requestData();
	if (_method == "GET")
		get();
	else if (_method == "POST")
		post();
	if (!_write_buf.empty())
		_fd->events = POLLOUT;
}

bool Connection::closeRequest(void)
{
	return (_close);
}