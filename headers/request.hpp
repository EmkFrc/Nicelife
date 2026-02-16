/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmartin <nmartin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 15:04:18 by nmartin           #+#    #+#             */
/*   Updated: 2026/02/16 15:36:55 by nmartin          ###   ########.fr       */
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
#define BUFFER_SIZE 1024

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
	void		sendErrorPage(int status, std::string errorMsg);
	void		sendResponse(std::string filename, bool status);
	void		sendIcon(void);
	void		get(void);

	void		set_cgi_env(void);
	void		start_cgi(void);
	void		handle_executing_cgi(void);

	void		getFilename(std::string username, t_upload *data, size_t headersLength);
	bool		getExec();
	void		upload(void);
	void		post(void);

	void		delete_function(void);

	void		pollOut(void);
	void		pollIn(void);
	bool		closeRequest(void);

	void sendError(int code, const std::string& message);
	private:

	struct pollfd				*_fd;
	std::string 				_read_buf;
	std::string					_write_buf;
	std::string					_path_upload;
	size_t						_write_offset;
	size_t						_expected_length;
	std::string 				_method;
	std::string 				_uri;
	std::string 				_version;
	Env							_env;
	Response					_response;
	std::string					_root;
	std::string					_index;
	std::map<int, std::string>	_error_pages;
	unsigned int				_client_max_body_size;
	std::vector<Location>		_location;
	CGIExecution* 				_cgi;
	bool						_executing;
	bool						_close;
};


std::string extract_username_from_cookie(const std::string& cookie_string);
