/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookies.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmartin <nmartin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/20 18:37:32 by efranco           #+#    #+#             */
/*   Updated: 2025/12/09 21:04:15 by nmartin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include "RequestEMK.hpp"

struct CookieAttributes
{
	int maxAge;
	std::string path;
	std::string domain;
	bool httpOnly;
	bool secure;
	std::string sameSite;

	CookieAttributes()
	: maxAge(-1), path(""), domain(""), httpOnly(false), secure(false), sameSite("") {}
};

inline std::string intToString(int value)
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}

inline std::string buildSetCookieString(const std::string& name, const std::string& value, const CookieAttributes& attrs)
{
	std::string result;

    result = name + "=" + value;

    if (attrs.maxAge >= 0)
	{
        result += "; Max-Age=";
        result += intToString(attrs.maxAge);
    }

    if (!attrs.path.empty())
	{
        result += "; Path=";
        result += attrs.path;
    }

    if (!attrs.domain.empty())
	{
        result += "; Domain=";
        result += attrs.domain;
    }

    if (attrs.secure)
	{
        result += "; Secure";
    }

    if (attrs.httpOnly)
	{
        result += "; HttpOnly";
    }

    if (!attrs.sameSite.empty())
	{
        result += "; SameSite=";
        result += attrs.sameSite;
    }
    return result;
}


