/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmartin <nmartin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 19:07:05 by efranco           #+#    #+#             */
/*   Updated: 2025/12/09 20:07:44 by nmartin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <unistd.h>
#include <sys/types.h>
#include "RequestEMK.hpp"
#include "Response.hpp"
#include "CGIExecution.hpp"
#include "Env.hpp"

enum ClientState
{
    RECEIVING,
    READY,
    EXECUTING_CGI,
    SENDING,
    DONE
};

std::string stateToString(ClientState state);

struct Client
{
    int fd;
    ClientState state;
    std::string recv_buffer;
    Request request;
    std::string response_data;
    size_t bytes_sent;

    CGIExecution* cgi;
    Env env;


    Client()
        : fd(-1),
          state(RECEIVING),
          recv_buffer(""),
          bytes_sent(0),
          cgi(NULL)
    {}


    Client(int client_fd)
        : fd(client_fd),
          state(RECEIVING),
          recv_buffer(""),
          bytes_sent(0),
          cgi(NULL)
    {}
    ~Client()
    {
        if (cgi != NULL)
        {
            if (cgi->pipe_fd != -1)
            {
                close(cgi->pipe_fd);
            }
            delete cgi;
        }
    };
};

inline std::string stateToString(ClientState state)
{
    switch (state)
    {
        case RECEIVING: return "RECEIVING";
        case READY: return "READY";
        case EXECUTING_CGI: return "EXECUTING_CGI";
        case SENDING: return "SENDING";
        case DONE: return "DONE";
        default: return "UNKNOWN";
    }
}
