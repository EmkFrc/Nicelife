/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestEMK.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmartin <nmartin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 16:05:41 by efranco           #+#    #+#             */
/*   Updated: 2025/12/09 21:04:26 by nmartin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Cookies.hpp"
#include <map>
#include <vector>
#include <sstream>
#include <cstdlib>
inline bool isaspace(char c)
{
	return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
}
inline std::string trim(const std::string& str)
{
	if (str.empty())
    	return "";

	size_t start = 0;
	size_t end = str.size();

	for(size_t i = 0; i < str.size(); ++i)
	{
		if(!isaspace(str[i]))
		{
			start = i;
			break;
		}
	}
	for(size_t i = str.size() - 1; i > 0; --i)
	{
		if(!isaspace(str[i]))
		{
			end = i;
			break;
		}
	}
	if (isaspace(str[0]) && start == 0)
    {
        return "";
    }
	return str.substr(start, end - start + 1);
}
inline std::vector<std::string> split(const std::string& str, const std::string& sep)
{
	std::vector<std::string> array;

	size_t pos = 0;

	while (true)
	{
		size_t pos_sep = str.find(sep, pos);

		if (pos_sep == std::string::npos)
		{
			std::string pieces = str.substr(pos);
			array.push_back(pieces);
			break;
		}
		else
		{
			size_t length = pos_sep - pos;
			std::string pieces = str.substr(pos, length);
			array.push_back(pieces);
		}
		pos = pos_sep + sep.length();
	}
	return array;
}
class Request
{
	private :
		std::map<std::string , std::string> _cookies;

	public :

		void parseCookieHeader(const std::string& str)
		{
			std::vector<std::string> pieces = split(str, ";");

			for(size_t i = 0; i < pieces.size() ; ++i)
			{
				std::string piece = trim(pieces[i]);

				size_t pos = piece.find("=");

				if (pos == std::string::npos)
    				continue;

				std::string nom = trim(piece.substr(0, pos));
				std::string valeur = trim(piece.substr(pos + 1));

				this->_cookies[nom] = valeur;
			}
		};
		std::string getCookie(const std::string& name) const
		{
			std::map<std::string, std::string>::const_iterator it = _cookies.find(name);

			if (it != this->_cookies.end())
				return it->second;
			else
				return "";
		}
    	bool hasCookie(const std::string& name) const
    	{
        	return this->_cookies.find(name) != _cookies.end();
    	}
   		const std::map<std::string, std::string>& getAllCookies() const
    	{
        	return this->_cookies;
    	}
};
