/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: efranco <efranco@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 23:04:14 by nmartin           #+#    #+#             */
/*   Updated: 2026/02/22 15:35:41 by efranco          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request.hpp"

Connection::Connection() : _fd(NULL), _write_offset(0), _expected_length(0),
	_executing(false), _close(false)
{
}

Connection::Connection(struct pollfd *fd) : _fd(fd), _write_offset(0),
	_expected_length(0), _executing(false), _close(false)
{
	_read_buf.reserve(8162);
	_write_buf.reserve(8162);
}

Connection::~Connection()
{
}

void Connection::setConf(const ConfigServer &server)
{
	_default_root = server.getRoot();
	_default_index = server.getIndex();
	_error_pages = server.getErrorPages();
	_client_max_body_size = server.getClientMaxBodySize();
	_location = server.getLocations();
}

void Connection::sendData(void)
{
	ssize_t	n;

	_write_offset = 0;
	while (_write_offset < _write_buf.size())
	{
		n = send(_fd->fd, _write_buf.c_str() + _write_offset, _write_buf.size()
				- _write_offset, 0);
		if (n > 0)
			_write_offset += n;
		else
			break ;
	}
	_write_buf.clear();
	_write_offset = 0;
}
void Connection::sendError(int code, const std::string &message)
{
	_response.setStatus(code);
	if (_error_pages.find(code) != _error_pages.end())
	{
		sendResponse(_default_root + _error_pages[code], true);
		return ;
	}
	std::string statusMsg = _response.getStatusMessage(code);
	std::ostringstream html;
	html << "<!DOCTYPE html>\r\n"
			<< "<html>\r\n"
			<< "<head><title>" << code << " " << statusMsg << "</title></head>\r\n"
			<< "<body>\r\n"
			<< "<h1>" << code << " " << statusMsg << "</h1>\r\n"
			<< "<p>" << message << "</p>\r\n"
			<< "</body>\r\n"
			<< "</html>\r\n";
	_response.addHeader("Content-Type", "text/html");
	_response.setBody(html.str());
	_write_buf = _response.build();
	_fd->events = POLLOUT;
}

void Connection::recvData(void)
{
	char	buffer[BUFFER_SIZE];
	size_t	pos;

	ssize_t received(0);
	if (_expected_length == 0)
		_read_buf.clear();
	while (true)
	{
		// std ::cout << _fd->fd;
		received = recv(_fd->fd, buffer, BUFFER_SIZE, MSG_DONTWAIT);
		if (received > 0)
		{
			_read_buf.append(buffer, received);
			pos = _read_buf.find("\r\n\r\n");
			if (pos != std::string::npos)
			{
				_env.parse_query_string(_read_buf);
				_env.search_cookie_string(_read_buf);
				_env.extract_method(_read_buf);
				_env.parse_headers(_read_buf);
				_env.parse_body(_read_buf);
				return ;
			}
		}
		else if (received == 0)
		{
			if (_expected_length == 0 || _read_buf.size() >= _expected_length)
				_close = true;
			break ;
		}
		else
			break ;
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
	ssize_t	n;

	if (_write_buf.empty())
	{
		_fd->events = POLLIN;
		return ;
	}
	n = send(_fd->fd, _write_buf.c_str() + _write_offset, _write_buf.size()
			- _write_offset, MSG_NOSIGNAL);
	if (n > 0)
	{
		_write_offset += n;
		if (_write_offset >= _write_buf.size())
		{
			_write_buf.clear();
			_write_offset = 0;
			_fd->events = POLLIN;
		}
	}
}

void Connection::pollIn(void)
{
	size_t	header_end;
	size_t	pos;
	size_t	end;
	int		content_len;

	recvData();
	if (_read_buf.empty())
		return ;
	if (_close && _expected_length > 0 && _read_buf.size() < _expected_length)
	{
		std::cerr << "Connection closed prematurely! Expected " << _expected_length << " bytes but got " << _read_buf.size() << std::endl;
		_expected_length = 0;
		return ;
	}
	header_end = _read_buf.find("\r\n\r\n");
	if (header_end == std::string::npos)
	{
		if (_close)
		{
			std::cerr << "Connection closed without complete headers!" << std::endl;
			return ;
		}
		return ;
	}
	if (_expected_length == 0)
	{
		pos = _read_buf.find("Content-Length: ");
		if (pos != std::string::npos && pos < header_end)
		{
			end = _read_buf.find("\r\n", pos);
			if (end != std::string::npos)
			{
				content_len = atoi(_read_buf.substr(pos + 16, end
							- pos).c_str());
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
			return ;
		}
		_fd->events = POLLIN;
		return ;
	}
	std::cout << _read_buf << std::endl;
	if (_read_buf.length() > _client_max_body_size)
	{
		sendErrorPage(413, "Request Entity Too Large");
		_expected_length = 0;
		_fd->events = POLLOUT;
		return ;
	}
	requestData();
	if (_method == "GET" && (_location.find(_uri) == _location.end()
			|| (_location.find(_uri) != _location.end()
				&& _location[_uri].allowed_methods["GET"])))
		// enlever _location.find(_uri) == _location.end() si GET pas true par defaut
		get();
	else if (_method == "POST")
		post();
	else if (_method == "DELETE")
	{
		delete_function();
	}
	else
	{
		sendErrorPage(405, "Method not supported");
	}
	_expected_length = 0;
	_fd->events = POLLOUT;
}

bool Connection::closeRequest(void)
{
	return (_close && _write_buf.empty());
}
time_t Connection::getCurrentTimeSec()
{
	return (time(NULL));
}
double Connection::getElapsedTimeSec(time_t startTime)
{
	time_t	currentTime;

	currentTime = getCurrentTimeSec();
	return (difftime(currentTime, startTime));
}
