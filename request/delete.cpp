/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   delete.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: efranco <efranco@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 17:06:05 by efranco           #+#    #+#             */
/*   Updated: 2026/01/06 19:57:09 by efranco          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request.hpp"

bool	is_cgi_d(const std::string &str)
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
bool is_uploads(const std::string& str)
{
	size_t i;
	if (str[0] == '/')
		str.substr(1);
	else
		return (false);
	i = str.find_first_of('/');
	if (i != std::string::npos)
	{
		std::string new_str = str.substr(0, i);
		if (new_str == "uploads/")
			 return (true);
	}
	return (false);
}

// bool is_photo(const std::string &str)
// {
// 	size_t i = str.find_last_of('.');
// 	if (i != std::string::npos)
// 	{
// 		std::string tmp = str.substr(i);
// 		if (tmp == ".jpg" || tmp == ".png")
// 			return (true);
// 	}
// 	return (false);
// }
void Connection::delete_function()
{
	if (is_cgi_d(_uri))
	{
        start_cgi();
	}
	else
	{
		if (is_uploads(_uri))
		{
			std::cout << "200 verif ok " << std::endl;
		}
		else
		{
			_response.setStatus(404);
			_response.setBody("Not Found");
			_write_buf = _response.build();
		}
	}

}
