/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Env.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: efranco <efranco@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 18:28:02 by efranco           #+#    #+#             */
/*   Updated: 2025/12/18 21:53:03 by efranco          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Cookies.hpp"
#include <iostream>
#include <sstream>
#include <string>

// #include "Request.hpp"

// enum EnvVariable
// {
// 	REQUEST_METHOD,
// 	SCRIPT_NAME,
// 	SERVER_PROTOCOL, SERVER_SOFTWARE, GATEWAY_INTERFACE,
// 	SERVER_NAME, SERVER_PORT,
// 	QUERY_STRING,
// 	REMOTE_ADDR
// };

class Env
{
  private:
	std::string Query_string;
	std::string Cookie_string;
	std::string method;
	std::string body;
	std::map<std::string, std::string> headers;

  public:
	Env() : Query_string(""), method("")
	{
	}
	~Env()
	{
	}
	void parse_query_string(std::string &httpRequest)
	{
		std::string	path;

		size_t pos = httpRequest.find("\r\n");
		if (pos != std::string::npos)
		{
			std::string first_line = httpRequest.substr(0, pos);
			size_t pos_space1 = first_line.find(' ');
			if (pos_space1 == std::string::npos)
				return ;
			size_t pos_space2 = first_line.find(' ', pos_space1 + 1);
			if (pos_space2 == std::string::npos)
				return ;
			path = first_line.substr(pos_space1 + 1, pos_space2 - pos_space1
					- 1);
			size_t pos2 = path.find('?');
			if (pos2 != std::string::npos)
			{
				Query_string = path.substr(pos2 + 1);
			}
		}
	};
	void extract_method(std::string &httpRequest)
	{
    size_t pos = httpRequest.find("\r\n");
    if (pos != std::string::npos)
    {
        std::string first_line = httpRequest. substr(0, pos);

        size_t pos_space = first_line.find(' ');
        if (pos_space == std::string::npos)
            return ;
        method = first_line.substr(0, pos_space);

        std::string new_line = first_line.substr(pos_space + 1);
        pos_space = new_line.find(' ');
        if (pos_space == std::string::npos)
            return ;
        std::string uri = new_line. substr(0, pos_space);

        size_t pos_question = uri. find('?');
        if (pos_question != std::string::npos)
            Query_string = uri.substr(pos_question + 1);
        else
            Query_string = "";
    }
	}
	void search_cookie_string(std::string &httpRequest)
	{
		size_t pos_cookie = httpRequest.find("Cookie: ");

		if (pos_cookie == std::string::npos)
		{
			Cookie_string = "";
			return ;
		}
		pos_cookie += 8;
		std::string Cookie_str_mid = httpRequest.substr(pos_cookie);

		size_t pos_end = Cookie_str_mid.find("\r\n");
		if (pos_end == std::string::npos)
		{
			Cookie_string = "";
			return ;
		}
		Cookie_string = Cookie_str_mid.substr(0, pos_end);
	}
	void parse_body(std::string &httpRequest)
	{
		size_t pos = httpRequest.find("\r\n\r\n");
		if (pos == std::string::npos)
		{
			body = "";
			return;
		}
		body = httpRequest.substr(pos + 4);
	}
	void parse_headers(std::string &httpRequest)
	{
		size_t pos = httpRequest.find("\r\n");
		if (pos == std::string::npos)
			return ;

		std::string new_string = httpRequest.substr(pos + 2);

		pos = new_string.find("\r\n\r\n");
		if (pos == std::string::npos)
			return ;

		new_string = new_string.substr(0, pos);

		std::vector<std::string> headers_parts = split(new_string, "\r\n");
		std::string first;
		std::string second;
		for (size_t i = 0; i < headers_parts.size(); ++i)
		{
			pos = headers_parts[i].find(':');
			if (pos == std::string::npos)
				continue ;
			first = trim(headers_parts[i].substr(0, pos));
			second = trim(headers_parts[i].substr(pos + 1));
			headers[first] = second;
		}
	}
	std::string to_upper(const std::string &str)
	{
		std::string result = str;
		for (size_t i = 0; i < result.length(); i++)
		{
			result[i] = std::toupper(result[i]);
		}
		return (result);
	}
	std::string replace_char(const std::string &str, char old_char,
		char new_char)
	{
		std::string result = str;
		for (size_t i = 0; i < str.length(); ++i)
		{
			if (result[i] == old_char)
				result[i] = new_char;
		}
		return (result);
	}
	std::string transform_header_name(const std::string &name)
	{
		if (name == "Content-Type")
			return ("CONTENT_TYPE");
		if (name == "Content-Length")
			return ("CONTENT_LENGTH");

		std::string result = "HTTP_" + name;
		result = to_upper(result);
		result = replace_char(result, '-', '_');
		return (result);
	}
	const std::string &get_query_string() const
	{
		return (Query_string);
	}
	const std::string &get_body() const
	{
		return (body);
	}
	const std::string &get_Cookie_string() const
	{
		return (Cookie_string);
	}
	const std::string &get_method() const
	{
		return (method);
	}
  	const std::map<std::string, std::string>& get_headers() const
    {
        return headers;
    }

    std::map<std::string, std::string>& get_headers()
    {
        return headers;
    }

};
