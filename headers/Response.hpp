/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: efranco <efranco@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 09:15:35 by efranco           #+#    #+#             */
/*   Updated: 2025/12/18 23:05:10 by efranco          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Cookies.hpp"
#include <map>
#include <vector>
#include <sstream>

class Response
{
	private:
	int _statusCode;
    std::map<std::string, std::string> _headers;
    std::vector<std::string> _setCookies;
    std::string _body;

	public:

	Response() : _statusCode(200), _body("") {}

	void setStatus(int code)
	{
		this->_statusCode = code;
	};

	void addCookie(const std::string& name, const std::string& value, const CookieAttributes& attrs)
	{
		std::string result;

		result = buildSetCookieString(name, value, attrs);

		this->_setCookies.push_back(result);
	};

	std::string getStatusMessage(int code)
	{
    	switch(code)
    	{
        	case 200: return "OK";
        	case 201: return "Created";
        	case 204: return "No Content";
        	case 301: return "Moved Permanently";
        	case 302: return "Found";
        	case 400: return "Bad Request";
        	case 401: return "Unauthorized";
        	case 403: return "Forbidden";
     		case 404: return "Not Found";
        	case 500: return "Internal Server Error";
        	case 502: return "Bad Gateway";
        	case 503: return "Service Unavailable";
        	default: return "Unknown";
   		}
	};

	std::string get_content_type(const std::string &path)
	{
		// std::cout << "++++++++++++++++++++++++" << path << "+++++++++++++++++++" << std::endl;
    	size_t dot_pos = path.find_last_of('.');
    	if (dot_pos == std::string::npos)
    	    return "text/plain";

    	std::string extension = path.substr(dot_pos);

    	if (extension == ".html" || extension == ".htm")
    	    return "text/html; charset=utf-8";
    	else if (extension == ".css")
    	    return "text/css; charset=utf-8";
    	else if (extension == ".js")
    	    return "application/javascript; charset=utf-8";
    	else if (extension == ".json")
    	    return "application/json";
    	else if (extension == ".png")
    	    return "image/png";
    	else if (extension == ".jpg" || extension == ".jpeg")
    	    return "image/jpeg";
    	else
    	    return "text/plain";
	};

	void addHeader(const std::string& name, const std::string& value)
	{
		if (name == "Set-Cookie")
		{
			_setCookies.push_back(value);
		}
		else
		{
			_headers[name] = value;
		}
	};

	void setBody(const std::string& body)
	{
		this->_body = body;
	};

	std::string build()
	{
		std::string result;

		std::ostringstream length;
		length << _body.size();
		addHeader("Content-Length", length.str());
		result = "HTTP/1.1 " + intToString(_statusCode) + " " + getStatusMessage(this->_statusCode)  + "\r\n";
		for (std::map<std::string, std::string>::iterator it = this->_headers.begin(); it != this->_headers.end() ; ++it)
		{
			result += it->first + ": " + it->second + "\r\n";
		}
		for (size_t i = 0; i < this->_setCookies.size(); ++i)
		{
			result += "Set-Cookie: " + this->_setCookies[i] + "\r\n";
 		}
		result += "\r\n";
		result += this->_body;
		return result;
	};
};
