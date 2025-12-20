/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   post.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: efranco <efranco@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/01 13:52:30 by nmartin           #+#    #+#             */
/*   Updated: 2025/12/19 18:05:42 by efranco          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request.hpp"


void	Connection::getFilename(t_upload *data, size_t headersLength)
{
	std::istringstream	flx(_read_buf);
	std::string			line;
	std::string			next_line;
	std::string			filename;
	size_t				index;
	size_t				end;

	index = _read_buf.find("Content-Length: ");
	if (index == std::string::npos)
		return ;
	end = _read_buf.find("\r\n", index);
	if (end == std::string::npos)
		return ;
	data->contentLength = atoi(_read_buf.substr(index + 16, end - index).c_str()) - headersLength;
	while (std::getline(flx, line) && line.find("boundary=") == std::string::npos);
	index = line.find("boundary=");
	if (index == std::string::npos)
		return ;
	if (!line.empty() && line[line.size() - 1] == '\r')
		line.erase(line.size() - 1);
	next_line = line.substr(index + strlen("boundary="));
	while (std::getline(flx, line) && line.find(next_line) == std::string::npos);
	std::getline(flx, line);
	index = line.find("filename=\"");
	if (index == std::string::npos)
		return ;
	index += 10;
	end = line.find("\"", index);
    if (end == std::string::npos)
		return ;
	filename = line.substr(index, end - index);
	filename = "data/" + filename;
	filename.insert(filename.find("."), getTimestamp());
	data->filename = filename;
}

bool	Connection::getExec(void)
{
	return (_executing);
}

void	Connection::upload(void)
{
	size_t		start;
	size_t		end;
	size_t		file_size;
	t_upload	data;

	start = _read_buf.find("\r\n\r\n");
	if (start == std::string::npos)
		return ;
	end = _read_buf.find("\r\n\r\n", start + 4);
	if (end == std::string::npos)
		return ;
	getFilename(&data, end - start);
	std::cout << data.filename << "   Content-Length body: " << data.contentLength << std::endl;
	if (data.filename.empty())
		return ;

	// Début du fichier = après le 2ème \r\n\r\n
	start = end + 4;

	// Trouver la fin du fichier (avant le boundary de fermeture)
	end = _read_buf.find("\r\n--", start);
	if (end == std::string::npos)
	{
		std::cerr << "Error: boundary end not found!" << std::endl;
		std::cerr << "Buffer size: " << _read_buf.size() << std::endl;
		std::cerr << "Searching from pos: " << start << std::endl;
		std::cerr << "Last 200 chars:\n[" << _read_buf.substr(_read_buf.size() - 200) << "]" << std::endl;
		return ;
	}

	file_size = end - start;
	std::cout << "File size: " << file_size << " bytes (from pos " << start << " to " << end << ")" << std::endl;

	std::ofstream file(data.filename.c_str(), std::ios::binary);
	if (!file)
	{
    	std::cerr << "Error: file creation failed" << std::endl;
   		return;
	}

	file.write(_read_buf.c_str() + start, file_size);
	file.close();
	std::cout << "File uploaded successfully: " << data.filename << std::endl;
}
bool	is_cgi_post(const std::string &str)
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
void	Connection::post(void)
{
	if (is_cgi_post(_uri))
        start_cgi();
	if (_read_buf.find("Transfer-Encoding: chunked") != _read_buf.npos)
	{
	//TODO send 411 Length Required
		return ;
	}
	if (_uri == "/upload")
		upload();
	// else if (_uri == "/login.py")
	// 	add_env
}
