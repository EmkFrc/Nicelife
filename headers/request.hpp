/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: efranco <efranco@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 15:04:18 by nmartin           #+#    #+#             */
/*   Updated: 2026/02/22 16:37:32 by efranco          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <unistd.h>
#include <poll.h>
#include <map>
#include <sstream>
#include <fstream>
#include <fcntl.h>
#include "Env.hpp"
#include "Response.hpp"
#include "webserv.hpp"
#include "CGIExecution.hpp"
#include <sys/stat.h>
#include <dirent.h>
#include <algorithm>
#define BUFFER_SIZE 1048576

typedef	struct s_upload
{
	std::string	filename;
	size_t		contentLength;
}				t_upload;

class	Connection
{
	public:

	Connection();
	Connection(struct pollfd *fd);
	~Connection();
	void		setConf(const ConfigServer &server);
	void		sendData(void);
	void		recvData(void);
	void		requestData(void);

	void		send404(bool status);
	void		redirect(int status, std::string url);
	void		sendErrorPage(int status, std::string errorMsg);
	void		directoryListing(std::string path);
	void		autoindex(void);
	void		sendResponse(std::string filename, bool status);
	void		sendIcon(void);
	void		get(void);

	void		set_cgi_env(void);
	void		start_cgi(void);
	void		executing_cgi(void);

	void		getFilename(std::string username, t_upload *data, size_t headersLength);
	bool		getExec();
	void		upload(void);
	void		post(void);

	void		delete_function(void);

	void		pollOut(void);
	void		pollIn(void);
	bool		closeRequest(void);
	time_t getCurrentTimeSec();
	double getElapsedTimeSec(time_t startTime);

	bool	is_cgi_d(const std::string &str);
	bool is_uploads(const std::string& str, std::string& stock);
	bool verif_path_traversal(const std::string& str);
	bool verif_extension(const std::string& str);
	bool verif_username(const std::string& filename, const std::string& cookie_string);
	bool	file_exists(const std::string &path);


	void sendError(int code, const std::string& message);
	private:

	struct pollfd						*_fd;
	std::string 						_read_buf;
	std::string							_write_buf;
	std::string							_path_upload;
	size_t								_write_offset;
	size_t								_expected_length;
	std::string 						_method;
	std::string 						_uri;
	std::string 						_version;
	Env									_env;
	Response							_response;
	std::string							_root;
	std::string							_default_root;
	std::string							_index;
	std::string							_default_index;
	std::map<int, std::string>			_error_pages;
	unsigned int						_client_max_body_size;
	std::map<std::string, Location>		_location;
	CGIExecution* 						_cgi;
	bool								_executing;
	bool								_close;
	time_t start_time;
};


std::string extract_username_from_cookie(const std::string& cookie_string);
