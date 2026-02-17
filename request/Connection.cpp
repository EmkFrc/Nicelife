/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmartin <nmartin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 23:04:14 by nmartin           #+#    #+#             */
/*   Updated: 2026/02/17 17:03:49 by nmartin          ###   ########.fr       */
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

void	Connection::setConf(const ConfigServer &server)
{
	_default_root = server.getRoot();
	_default_index = server.getIndex();
	_error_pages = server.getErrorPages();
	_client_max_body_size = server.getClientMaxBodySize();
	_location = server.getLocations();	
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
	}
	std::cout << std::endl;
}
void Connection::sendError(int code, const std::string& message)
{
    _response.setStatus(code);
	if (_error_pages.find(code) != _error_pages.end())
	{
		sendResponse(_default_root + _error_pages[code], true);
		return ;
	}
    _response.addHeader("Content-Type", "application/json");

    std::ostringstream json;
    json << "{"
         << "\"error\":\"" << _response.getStatusMessage(code) << "\","
         << "\"code\":" << code << ","
         << "\"message\":\"" << message << "\""
         << "}";

    _response.setBody(json.str());
    _write_buf = _response.build();
    sendData();
	_fd->events = POLLOUT;
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
		// std ::cout << _fd->fd;
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
				_env.parse_body(_read_buf);
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
			break;
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
	std::string response;
	recvData();
	_fd->events = POLLIN;
}
bool	is_cgi_d(const std::string &str)
{
    if (str.find("/uploads/") == 0)
	{
        return false;
	}
	size_t	i = str.find_last_of('.');
	if (i != std::string::npos)
	{
		std::string tmp = str.substr(i);
		if (tmp == ".php" || tmp == ".py" || tmp == ".sh")
			return (true);
	}
	return (false);
}
bool is_uploads(const std::string& str, std::string& stock)
{
	size_t i;
	std::string tmp;
	if (str[0] == '/')
		tmp = str.substr(1);
	else
		return (false);
	i = tmp.find_first_of('/');
	if (i != std::string::npos)
	{
		std::string new_str = tmp.substr(0, i + 1);
		if (new_str == "uploads/")
		{
			stock = tmp.substr(i + 1);
			 return (true);
		}
	}
	return (false);
}
bool verif_path_traversal(const std::string& str)
{
	if (str[0] == '/')
	{
		return (false);
	}
	if (str.find("//") != std::string::npos)
	{
		return (false);
	}
	return (true);
}
bool verif_extension(const std::string& str)
{

	size_t i = str.find_last_of('.');
	if (i != std::string::npos)
	{
		std::string tmp = str.substr(i);
		if (tmp == ".jpg" || tmp == ".png" || tmp == ".pdf")
			return (true);
	}
	return (false);
}
std::string extract_username_from_cookie(const std::string& cookie_string)
{
    size_t pos = cookie_string.find("username=");
    if (pos == std::string::npos)
        return "";
    size_t start = pos + 9;
    size_t end = cookie_string.find(';', start);
    if (end == std::string::npos)
        end = cookie_string.length();
    std::string username = cookie_string.substr(start, end - start);

    size_t first = username.find_first_not_of("\t");
    if (first == std::string::npos)
        return "";
    size_t last = username.find_last_not_of(" \t");

    return username.substr(first, last - first + 1);
}

bool verif_username(const std::string& filename, const std::string& cookie_string)
{
    std::string username = extract_username_from_cookie(cookie_string);

    if (username.empty())
    {
        return false;
    }
    std::string prefix = username + "_";
    bool result = (filename.find(prefix) == 0);

    return result;
}
bool	file_exists(const std::string &path)
{
	struct stat	buffer;

	return (stat(path.c_str(), &buffer) == 0);
}
void Connection::delete_function()
{
	if (is_cgi_d(_uri))
	{
		start_cgi();
		return ;
	}
	else
	{
		std::string stock;
		if (is_uploads(_uri, stock))
		{
			_path_upload = stock;
			if (verif_path_traversal(_path_upload)
				&& verif_extension(_path_upload) && verif_username(_path_upload,
					_env.get_Cookie_string()))
			{
				std::string filepath = "data/" + _path_upload;
				if (!file_exists(filepath))
				{
					std::cout << "File not found (404)" << std::endl;
					_response.setStatus(404);
					_response.addHeader("Content-Type", "application/json");
					_response.setBody("{\"error\": \"File not found\"}");
					_write_buf = _response.build();
					return ;
				}
				if (std::remove(filepath.c_str()) == 0)
				{
					_response.setStatus(200);
					_response.addHeader("Content-Type", "application/json");
					_response.setBody("{"
										"\"status\": \"success\","
										"\"message\": \"File deleted successfully\","
										"\"file\": \"" +
										_path_upload +
										"\""
										"}");
					_write_buf = _response.build();
				}
				else
				{
					std::cout << "Failed to delete file" << std::endl;
					_response.setStatus(500);
					_response.addHeader("Content-Type", "application/json");
					_response.setBody("{"
										"\"error\": \"Internal server error\","
										"\"details\": \"Failed to delete file\""
										"}");
					_write_buf = _response.build();
				}
			}
			else
			{
				_response.setStatus(403);
				_response.setBody("Forbidden");
				_write_buf = _response.build();
			}
		}
		else
		{
			_response.setStatus(404);
			_response.setBody("Not Found");
			_write_buf = _response.build();
		}
	}
	sendData();
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
	if (_read_buf.length() > _client_max_body_size)
	{
		sendErrorPage(413, "Request Entity Too Large");
		_expected_length = 0;
		_fd->events = POLLOUT;
		return ;
	}
	requestData();
	if (_method == "GET")
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
	return (_close);
}
