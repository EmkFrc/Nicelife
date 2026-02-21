/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmartin <nmartin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 21:31:37 by nmartin           #+#    #+#             */
/*   Updated: 2026/02/21 22:03:02 by nmartin          ###   ########.fr       */
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
	_fd->events = POLLOUT;
}

void	Connection::redirect(int status, std::string url)
{
	_response.setStatus(status);
	_response.addHeader("Location", url);
    _response.addHeader("Connection", "close");
	_write_buf.clear();
	_write_buf = _response.build();
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
	_fd->events = POLLOUT;
}

void	Connection::directoryListing(std::string path)
{
	std::ostringstream			listingPage;
	struct dirent*				entry;
	std::vector<std::string>	files;
	std::vector<std::string>	dirs;
	std::string					fullpath;
	std::string					name;
	DIR*						dir = opendir(path.c_str());

	if (!dir)
	{
		sendErrorPage(500, "Failed to open directory");
		return;
	}
	listingPage << "<!DOCTYPE html>\n"
		<< "<html>\n"
		<< "<head>\n"
		<< "  <meta charset=\"UTF-8\">\n"
		<< "  <title>Index of " << _uri << "</title>\n"
		<< "  <style>\n"
		<< "    body { font-family: monospace; margin: 20px; }\n"
		<< "    h1 { font-size: 20px; }\n"
		<< "    hr { border: 0; border-top: 1px solid #ccc; }\n"
		<< "    a { text-decoration: none; color: #0066cc; }\n"
		<< "    a:hover { text-decoration: underline; }\n"
		<< "    pre { line-height: 1.5; }\n"
		<< "  </style>\n"
		<< "</head>\n"
		<< "<body>\n"
		<< "  <h1>Index of " << _uri << "</h1>\n"
		<< "  <hr>\n"
		<< "  <pre>\n";
	if (_uri != "/")
 		listingPage << "<a href=\"../\">../</a>\n";
	while ((entry = readdir(dir)) != NULL)
	{
		name = entry->d_name;
		if (name == "." || name == "..")
			continue;
		fullpath = path + "/" + name;
		struct stat st;
		if (stat(fullpath.c_str(), &st) == 0)
		{
			if (S_ISDIR(st.st_mode))
				dirs.push_back(name);
			else
				files.push_back(name);
		}
	}
	closedir(dir);
	std::sort(dirs.begin(), dirs.end());
	std::sort(files.begin(), files.end());
	for (size_t i = 0; i < dirs.size(); i++)
	{
		listingPage << "<a href=\"" << dirs[i] << "/\">" << dirs[i] << "/</a>\n";
	}
	for (size_t i = 0; i < files.size(); i++)
	{
		listingPage << "<a href=\"" << files[i] << "\">" << files[i] << "</a>\n";
	}
	listingPage	<< "  </pre>\n"
		<< "  <hr>\n"
		<< "</body>\n"
		<< "</html>";
	_response.setStatus(200);
	_response.addHeader("Content-Type", "text/html; charset=utf-8");
	_response.setBody(listingPage.str());
	_response.addHeader("Content-Length", intToString(listingPage.str().length()));
	_write_buf = _response.build();
	_fd->events = POLLOUT;
}

void	Connection::autoindex(void)
{
	std::string	filename;
	struct stat	sb;

	if (_location.find(_uri) != _location.end() && !_location[_uri].index.empty())
		_index = _location[_uri].index;
	else
		_index = _default_index;
	filename = _root + _uri + _index;
	if (stat(filename.c_str(), &sb) == 0 && S_ISREG(sb.st_mode))
		sendResponse(filename, false);
	else if (_location.find(_uri) != _location.end() && _location[_uri].autoindex)
		directoryListing(_root + _uri);
	else
		sendErrorPage(403, "Forbidden");
}

void	Connection::sendResponse(std::string filename, bool status)
{
	// std::cout << "reponse" << std::endl;
	std::ifstream		file(filename.c_str(), std::ios::binary);
	std::string			length;
	std::ostringstream	size;
	struct stat			sb;

	if (stat(filename.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
	{
 	   autoindex();
	   return ;
	}
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
	_write_buf.clear();
	_write_buf = _response.build();
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
	struct stat	sb;
	if (_uri.length() > 2048)
    {
        sendError(414, "URI too long");
        return;
    }
	if (_location.find(_uri) != _location.end() && !_location[_uri].root.empty())
		_root = _location[_uri].root;
	else
		_root = _default_root;
	if (_location.find(_uri) != _location.end() && !_location[_uri].return_url.empty())
		redirect(_location[_uri].return_code, _location[_uri].return_url);
	else if (is_cgi(_uri))
        start_cgi();
	else if (stat(_uri.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
		autoindex();
	else if (_uri == "/favicon.ico")
		sendIcon();
	else
		sendResponse(_root + _uri, false);
}
