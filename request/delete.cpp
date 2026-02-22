/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   delete.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: efranco <efranco@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 17:06:05 by efranco           #+#    #+#             */
/*   Updated: 2026/02/22 15:39:42 by efranco          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request.hpp"

bool	Connection::is_cgi_d(const std::string &str)
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
bool Connection::is_uploads(const std::string& str, std::string& stock)
{
	size_t i;
	std::string tmp;
	if (str[0] == '/')
		tmp = str.substr(1);
	else
		return (false);
	i = tmp.find_last_of('/');
	if (i != std::string::npos)
	{
		std::string new_str = "/" + tmp.substr(0, i + 1);
		std::string	upload_root;
		if (_location.find(_uri) != _location.end() && !_location[_uri].root.empty())
			_root = _location[_uri].root;
		else
		_root = _default_root;
		if (_location.find("/upload.html") != _location.end())
		{
			upload_root = _location["/upload.html"].upload_store;
		}
		else
			upload_root = "data";
		if (new_str == _root + upload_root.substr(1) + "/")
		{
			stock = tmp.substr(i + 1);
			 return (true);
		}
	}
	return (false);
}
bool Connection::verif_path_traversal(const std::string& str)
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
bool Connection::verif_extension(const std::string& str)
{

	size_t i = str.find_last_of('.');
	if (i != std::string::npos)
	{
		std::string tmp = str.substr(i);
		if (tmp == ".jpg" || tmp == ".png" || tmp == ".pdf" || tmp == ".jpeg")
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

bool Connection::verif_username(const std::string& filename, const std::string& cookie_string)
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
bool	Connection::file_exists(const std::string &path)
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
		std::string upload_root;

		if (is_uploads(_uri, stock))
		{
			_path_upload = stock;
			if (verif_path_traversal(_path_upload)
				&& verif_extension(_path_upload) && verif_username(_path_upload,
					_env.get_Cookie_string()))
			{
				if (_location.find(_uri) != _location.end() && !_location[_uri].root.empty())
					_root = _location[_uri].root;
				else
					_root = _default_root;
				if (_location.find("/upload.html") != _location.end())
				{
					upload_root = _location["/upload.html"].upload_store;
				}
				else
					upload_root = "data";
				std::string filepath = "website" + upload_root + "/" + _path_upload;
				std::cout << filepath << std::endl;
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
	_fd->events = POLLOUT;
}

