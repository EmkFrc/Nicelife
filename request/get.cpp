/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmartin <nmartin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 21:31:37 by nmartin           #+#    #+#             */
/*   Updated: 2026/01/29 15:59:35 by nmartin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request.hpp"



void	Connection::send404(void)
{
	_response.setStatus(404);
	_response.addHeader("Content-Type", "text/html");
	_response.setBody("<h1>404 - File Not Found</h1>");
	_write_buf.clear();
	_write_buf = _response.build();
    sendData();
	_fd->events = POLLOUT;
}

void	Connection::sendResponse(std::string filename)
{
	// std::cout << "reponse" << std::endl;
	std::ifstream	file(filename.c_str(), std::ios::binary);
	std::string		length;
	std::ostringstream size;

	if (!file)
		return (send404());
	std::string content((std::istreambuf_iterator<char>(file)),
                    		std::istreambuf_iterator<char>());
	file.close();
	size << content.size();
	length = size.str();
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
	std::string		filename(_root + "website/Imran.ico");
	std::ifstream	icon(filename.c_str(), std::ios::binary);
	std::string		length;
	std::ostringstream size;

	if (!icon)
		return (send404());
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
	if (is_cgi(_uri))
        start_cgi();
	else if (_uri == "/")
		sendResponse(_root + "website/" + _index);
	else if (_uri == "/favicon.ico")
		sendIcon();
	else
		sendResponse(_root + "website/" + _uri);
}
