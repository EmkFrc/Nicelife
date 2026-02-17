/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmartin <nmartin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 21:31:37 by nmartin           #+#    #+#             */
/*   Updated: 2026/02/17 15:21:05 by nmartin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request.hpp"

void	Connection::send404(bool status)
{
	_response.setStatus(404);
	if (!status && _error_pages.find(404) != _error_pages.end())
	{
		sendResponse(_root + _error_pages[404], true);
		return ;
	}
	_response.addHeader("Content-Type", "text/html");
	_response.setBody("<h1>404 - File Not Found</h1>");
	_write_buf.clear();
	_write_buf = _response.build();
    sendData();
	_fd->events = POLLOUT;
}

void	Connection::sendErrorPage(int status, std::string errorMsg)
{
	_response.setStatus(status);
	if (_error_pages.find(status) != _error_pages.end())
	{
		sendResponse(_default_root + _error_pages[status], true);
		return ;
	}
	_response.addHeader("Content-Type", "text/html");
	std::ostringstream oss;
	oss << "<h1>" << status << " - " << errorMsg << "</h1>";
	std::string message = oss.str();
	_response.setBody(message);
	_write_buf.clear();
	_write_buf = _response.build();
    sendData();
	_fd->events = POLLOUT;
}

void	Connection::sendResponse(std::string filename, bool status)
{
	// std::cout << "reponse" << std::endl;
	std::ifstream	file(filename.c_str(), std::ios::binary);
	std::string		length;
	std::ostringstream size;

	std::cout <<filename <<"888888888888" << std::endl;
	if (!file)
		return (send404(status));
	std::string content((std::istreambuf_iterator<char>(file)),
                    		std::istreambuf_iterator<char>());
	file.close();
	size << content.size();
	length = size.str();
	if (!status)
		_response.setStatus(200);
	_response.addHeader("Content-Type", _response.get_content_type(filename));
	_response.setBody(content);
	// _write_buf += "Content-Length" + length + "\r\n";//TODO faire content length
	_write_buf.clear();
	_write_buf = _response.build();
	// std::cout << "------------------------" << std::endl;
	// std::cout << _write_buf << std::endl;
	// std::cout << "------------------------" << std::endl;
	sendData();
}

void	Connection::sendIcon(void)
{
// 	std::cout << "icon" << std::endl;
	std::string		filename(_root + "Imran.ico");
	std::ifstream	icon(filename.c_str(), std::ios::binary);
	std::string		length;
	std::ostringstream size;

	if (!icon)
		return (send404(false));
	std::string content((std::istreambuf_iterator<char>(icon)),
                    		std::istreambuf_iterator<char>());
	icon.close();
	size << content.size();
	length = size.str();
	_write_buf.clear();
	_write_buf = "HTTP/1.1 200 OK\r\n";
    _write_buf += "Content-Type: image/x-icon\r\n";
    _write_buf += "Content-Length: " + length + "\r\n";
    _write_buf += "Connection: keep-alive\r\n";
    _write_buf += "\r\n";
    _write_buf += content;
    sendData();
}
bool	is_cgi(const std::string &str)
{
	size_t	i;

	i = str.find_last_of('.');
	if (i != std::string::npos)
	{
		std::string tmp = str.substr(i);
		if (tmp == ".php" || tmp == ".py" || tmp == ".sh")
			return (true);
	}
	return (false);
}
void	Connection::get(void)
{
	if (_uri.length() > 2048)
    {
        sendError(414, "URI too long");
        return;
    }
	if (_location.find(_uri) != _location.end() && !_location[_uri].root.empty())
		_root = _location[_uri].root;
	else
		_root = _default_root;
	if (is_cgi(_uri))
        start_cgi();
	else if (_uri == "/")
		sendResponse(_root + _index, false);
	else if (_uri == "/favicon.ico")
		sendIcon();
	else
		sendResponse(_root + _uri, false);
}
