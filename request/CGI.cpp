/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: efranco <efranco@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 20:08:27 by nmartin           #+#    #+#             */
/*   Updated: 2026/02/22 16:39:09 by efranco          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request.hpp"

void	set_nonblocking(int fd)
{
	int	flags;

	flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
	{
		perror("fcntl F_GETFL");
		return ;
	}
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		perror("fcntl F_SETFL");
		return ;
	}
}

void	Connection::set_cgi_env(void)
{
	setenv("REQUEST_METHOD", _env.get_method().c_str(), 1);
    setenv("SCRIPT_NAME", _uri.c_str(), 1);
    setenv("QUERY_STRING", _env. get_query_string().c_str(), 1);
    setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);
    setenv("SERVER_SOFTWARE", "WebServ/1.0", 1);
    setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
    setenv("SERVER_NAME", "localhost", 1);
    setenv("SERVER_PORT", "8080", 1);
    setenv("REMOTE_ADDR", "127.0.0.1", 1);

    if (!_env.get_Cookie_string().empty())
    {
        setenv("HTTP_COOKIE", _env.get_Cookie_string().c_str(), 1);
    }

	std::map<std::string, std::string>& http_headers = _env.get_headers();
	for(std::map<std::string, std::string>::iterator it = http_headers.begin(); it != http_headers.end(); ++it)
	{
		std::string env_name = _env.transform_header_name(it->first);
		setenv(env_name.c_str(), it->second.c_str(), 1);
	}
}

void Connection::start_cgi(void)
{
    size_t derniere_barre = _uri.find_last_of('/');
    std::string nom_fichier = _uri.substr(derniere_barre + 1);
    std::string chemin_reel = _root + nom_fichier;

    pid_t   pid;
    int     pipein[2], pipeout[2];

    char    *args[] = {(char *)"/usr/bin/python3", (char *)chemin_reel.c_str(), NULL};

    start_time = getCurrentTimeSec();
    if (pipe(pipein) == -1 || pipe(pipeout) == -1)
    {
        std::cerr << "error : pipe()" << std::endl;
        return ;
    }

    pid = fork();
    if (pid == -1)
    {
        std::cerr << "error : fork()" << std::endl;
        close(pipein[0]);
        close(pipein[1]);
        close(pipeout[0]);
        close(pipeout[1]);
        return;
    }

    if (pid == 0)
    {

        close(pipein[1]);
        if (_env.get_method() == "POST")
        {
            if (dup2(pipein[0], STDIN_FILENO) == -1)
            {
                std::cerr << "error : dup2(stdin)" << std::endl;
                exit(1);
            }
        }

        close(pipein[0]);


        close(pipeout[0]);
        if (dup2(pipeout[1], STDOUT_FILENO) == -1)
        {
            std::cerr << "error : dup2(stdout)" << std::endl;
            exit(1);
        }
        close(pipeout[1]);

        set_cgi_env();

        if (execve("/usr/bin/python3", args, environ) == -1)
        {
            std::cerr << "error : execve()" << std::endl;
            exit(1);
        }
    }
    if (pid > 0) // Parent
    {
        close(pipein[0]);
        close(pipeout[1]);

        if (_env.get_method() == "POST")
        {
            std::string body = _env.get_body();
            if (!body. empty())
            {
                write(pipein[1], body.c_str(), body.size());
            }
        }

        close(pipein[1]);

        set_nonblocking(pipeout[0]);

        _cgi = new CGIExecution();
        _cgi->pid = pid;
        _cgi->pipe_fd = pipeout[0];
        _cgi->output = "";
        _cgi->completed = false;
        _executing = true;
    }
}

void	Connection::executing_cgi(void)
{
	char		buffer[4096];
	ssize_t		n;
	int			status;
	int			result;

    if (_cgi == NULL)
	{
		std::cerr << "Erreur : cgi est NULL" << std::endl;
		return ;
	}

    pid_t pid = _cgi->pid;
	if (getElapsedTimeSec(start_time) > 15)
	{
		printf("TIMEOUT: Le CGI a dépassé 15 secondes\n");
		kill(pid, SIGKILL);
		waitpid(pid, NULL, 0);
		close(_cgi->pipe_fd);
		sendError(504, "Gateway Timeout");
		_executing = false;
		delete _cgi;
		_cgi = NULL;
		return;
	}
	result = waitpid(_cgi->pid, &status, WNOHANG);
	if (result == -1)
	{
		std::cerr << "error : waitpid()" << std::endl;
		close(_cgi->pipe_fd);
		delete _cgi;
		_cgi = NULL;
		return ;
	}
	if (result > 0)
	{

		_cgi->completed = true;
	}
	n = read(_cgi->pipe_fd, buffer, sizeof(buffer));
	if (n > 0)
		_cgi->output.append(buffer, n);
	if (n == 0)
		std::cout << "EOF sur le pipe" << std::endl;
	if (_cgi->completed == true)
	{
		_executing = false;
		parse_cgi_output(_cgi);
		std::map<std::string, std::vector<std::string> > parsed_headers;
		if (_cgi->has_headers)
			parse_cgi_headers(_cgi->cgi_headers, parsed_headers);
		else
		{
			parsed_headers["Content-Type"].push_back("text/html");
		}
		_write_buf.clear();
		_write_buf = build_cgi_response(_cgi, parsed_headers);
		close(_cgi->pipe_fd);
		delete _cgi;
		_cgi = NULL;
	}
}
