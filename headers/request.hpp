/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmartin <nmartin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 15:04:18 by nmartin           #+#    #+#             */
/*   Updated: 2025/12/20 13:58:24 by nmartin          ###   ########.fr       */
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
	void		sendData(void);
	void		recvData(void);
	void		requestData(void);

	void		send404(void);
	void		setResponse(std::string filename);
	void		sendIcon(void);
	void		get(void);

	void		set_cgi_env(void);
	void		start_cgi(void);
	void		handle_executing_cgi(void);

	void		getFilename(t_upload *data, size_t headersLength);
	bool		getExec();
	void		upload(void);
	void		log(void);
	void		post(void);

	void		pollOut(void);
	void		pollIn(void);
	bool		closeRequest(void);
	
	private:
	
	struct pollfd	*_fd;
	std::string 	_read_buf;
	std::string		_write_buf;
	size_t			_write_offset;
	size_t			_expected_length;
	std::string 	_method;
	std::string 	_uri;
	std::string 	_version;
	Env				_env;
	Response		_response;
	CGIExecution* 	_cgi;
	bool			_executing;
	bool			_close;
};