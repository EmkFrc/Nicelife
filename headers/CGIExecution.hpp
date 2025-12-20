/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIExecution.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: efranco <efranco@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/28 13:35:37 by efranco           #+#    #+#             */
/*   Updated: 2025/12/18 23:05:06 by efranco          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "RequestEMK.hpp"
#include <iostream>
#include <string>

struct		CGIExecution
{
	pid_t	pid;
	int		pipe_fd;
	std::string output;
	bool	completed;
	int		exit_status;

	std::string cgi_headers;
	std::string cgi_body;
	bool	has_headers;
	CGIExecution() : pid(-1), pipe_fd(-1), completed(false), exit_status(0),
		cgi_headers(""), cgi_body(""), has_headers(false)
	{
	}
};

inline void parse_cgi_output(CGIExecution *cgi)
{
    std::string output = cgi->output;

    size_t n = output.find("\r\n\r\n");
    size_t separator_length = 4;

    if (n == std::string::npos)
    {
        n = output.find("\n\n");
        separator_length = 2;
    }

    if (n == std::string::npos)
    {
        cgi->has_headers = false;
        cgi->cgi_body = output;
    }
    else
    {
        cgi->has_headers = true;
        cgi->cgi_headers = output.substr(0, n);
        cgi->cgi_body = output.substr(n + separator_length);
    }
}

inline void	parse_cgi_headers(const std::string &headers_section,
		std::map<std::string, std::vector<std::string> >& parsed_headers)
{

	std::string headers_clean = headers_section;
	size_t pos = 0;
	while ((pos = headers_clean. find("\r\n", pos)) != std::string::npos)
	{
    	headers_clean. replace(pos, 2, "\n");
    	pos += 1;
	}
	std::vector<std::string> lines = split(headers_section, "\r\n");
	for (size_t i = 0; i < lines.size(); ++i)
	{
		std::string line = lines[i];
		if (line.empty())
			continue ;
		pos = line.find(':');
		if (pos == std::string::npos)
			continue ;
		std::string name = trim(line.substr(0, pos));
		std::string value = trim(line.substr(pos + 1));
		parsed_headers[name].push_back(value);
	}
}

inline int extractStatusCode(const std::string& str)
{
	size_t pos = str.find(' ');
	std::string code_str;
	if (pos == std::string::npos)
		code_str = str;
	else
		code_str = str.substr(0, pos);

	return (atoi(code_str.c_str()));
}

inline std::string build_cgi_response(CGIExecution *cgi, std::map<std::string,
	std::vector<std::string> > &parsed_headers)
{
	Response response;

	int status_code = 200;

	for (std::map<std::string, std::vector<std::string> >::iterator it = parsed_headers.begin(); it != parsed_headers.end(); ++it)
	{
		if (it->first == "Status")
		{
			status_code = extractStatusCode(it->second[0]);
			continue;
		}
		size_t i = 0;
		while (i < it->second.size())
		{
 			response.addHeader(it->first, it->second[i]);
    		i++;
		}
	}
	response.setStatus(status_code);
	response.setBody(cgi->cgi_body);
	// std::cout << "!!!!!!!!!" << cgi->cgi_body << "!!!!!!!!!!" << std::endl;
	return response.build();
}
