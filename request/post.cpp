/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   post.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: efranco <efranco@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/01 13:52:30 by nmartin           #+#    #+#             */
/*   Updated: 2026/02/22 21:11:19 by efranco          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request.hpp"


void	Connection::getFilename(std::string username, t_upload *data, size_t headersLength)
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
	std::string upload_root;
	if (_location.find("/upload.html") != _location.end())
	{
		upload_root = _location["/upload.html"].upload_store;
	}
	else
		upload_root = "data";
	filename = upload_root + "/" + username + "_" + filename;
	filename.insert(filename.find("."), getTimestamp());
	data->filename = _root + filename;
}

bool	Connection::getExec(void)
{
	return (_executing);
}

bool createDirectories(const std::string &filepath)
{
	std::string path = filepath.substr(0, filepath.find_last_of('/'));
	if (path.empty())
		return true;
	size_t pos = 0;
	if (path[0] == '/')
		pos = 1;
	while ((pos = path.find('/', pos)) != std::string::npos)
	{
		std::string subdir = path.substr(0, pos);
		mkdir(subdir.c_str(), 0755);
		struct stat st;
		if (stat(subdir.c_str(), &st) != 0 || !S_ISDIR(st.st_mode))
			return false;
		pos++;
	}
	mkdir(path.c_str(), 0755);
	struct stat st;
	if (stat(path.c_str(), &st) != 0 || !S_ISDIR(st.st_mode))
		return false;
	return true;
}

bool validFormat(const std::string &path)
	{
		size_t dot_pos = path.find_last_of('.');
		if (dot_pos == std::string::npos)
			return (false);
		std::string extension = path.substr(dot_pos);
		if (extension == ".jpg" || extension == ".jpeg" || extension == ".png")
			return (true);
		else
			return (false);
};

void	Connection::upload(void)
{
	size_t		start;
	size_t		end;
	size_t		file_size;
	t_upload	data;
	std::string	username;
	std::string	format;

	std::string cookie = _env.get_Cookie_string();
	username = extract_username_from_cookie(cookie);
	if (cookie.empty() || username.empty())
    {
        std::cout << "→ No authentication" << std::endl;
        sendError(401, "Authentication required for upload");
        return;
    }
	std::cout << "→ Authenticated user: [" << username << "]" << std::endl;
	start = _read_buf.find("\r\n\r\n");
	if (start == std::string::npos)
	{
        sendError(400, "Invalid multipart format - no headers separator");
        return;
    }
	end = _read_buf.find("\r\n\r\n", start + 4);
	if (end == std::string::npos)
	{
		sendError(400, "Invalid multipart format - no content separator");
        return;
	}
	getFilename(username, &data, end - start);
	if (!validFormat(data.filename))
	{
		{
        	sendError(415, "Unsupported Media Type");
        	return;
		}
	}
	std::cout << data.filename << "   Content-Length body: " << data.contentLength << std::endl;
	if (data.filename.empty())
	{
        sendError(400, "Invalid or missing filename");
        return;
    }
	if (!createDirectories(data.filename))
    {
        sendError(500, "Failed to create upload directory");
        return;
    }
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
	if (!file.is_open())
	{
    	sendError(500, "Failed to save file on server");
        return;
	}

	file.write(_read_buf.c_str() + start, file_size);
	file.close();
	if (file.fail())
    {
        sendError(500, "Failed to write file data");
        return;
    }
	std::cout << "File uploaded successfully: " << data.filename << std::endl;
	_response.setStatus(201);
    _response.addHeader("Content-Type", "application/json");

    std::ostringstream json;
    json << "{"
         << "\"status\":\"success\","
         << "\"message\":\"File uploaded successfully\","
         << "\"filename\":\"" << data.filename << "\","
         << "\"size\":" << file_size
         << "}";

    _response.setBody(json.str());
    _write_buf = _response.build();
    _fd->events = POLLOUT;
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
	{
        start_cgi();
		return;
	}
	if (_read_buf.find("Transfer-Encoding: chunked") != _read_buf.npos)
	{
		sendError(501, "Chunked transfer encoding not implemented");
		return;
	}
	if (_read_buf.find("Content-Length: ") == std::string::npos)
    {
        sendError(411, "Content-Length header required");
        return;
    }
	size_t cl_pos = _read_buf.find("Content-Length: ");
    size_t cl_end = _read_buf.find("\r\n", cl_pos);
    if (cl_end == std::string::npos)
    {
        sendError(400, "Malformed Content-Length header");
        return;
    }
	if (_uri == "/upload.html")
	{
		if (_location.find("/upload.html") != _location.end() && !_location["/upload.html"].upload_enable)
			sendError(405, "Upload disbabled in server's configuration");
		else
			upload();
		return;
	}
	else
		sendError(404, "Resource not found");
}
