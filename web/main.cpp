// /* ************************************************************************** */
// /*                                                                            */
// /*                                                        :::      ::::::::   */
// /*   main.cpp                                           :+:      :+:    :+:   */
// /*                                                    +:+ +:+         +:+     */
// /*   By: efranco <efranco@student.42.fr>            +#+  +:+       +#+        */
// /*                                                +#+#+#+#+#+   +#+           */
// /*   Created: 2025/10/30 14:07:19 by efranco           #+#    #+#             */
// /*   Updated: 2025/12/02 18:22:28 by efranco          ###   ########.fr       */
// /*                                                                            */
// /* ************************************************************************** */

#include <errno.h>
#include <fcntl.h>
#include <map>
#include <sys/wait.h>
#include <fstream>
#include "Client.hpp"
#include "Env.hpp"
#include "request.hpp"
#include "webserv.hpp"

std::string	getTimestamp(void)
{
    struct tm time_info;
    struct timeval tv;
    char buf[32];
	std::time_t t = std::time(NULL);

    localtime_r(&t, &time_info);

    int year2 = (time_info.tm_year + 1900) % 100;
    int month = time_info.tm_mon + 1;
    int day = time_info.tm_mday;
    int seconds = time_info.tm_sec;
    int milli = 0;
    if (gettimeofday(&tv, NULL) == 0) {
        milli = static_cast<int>((tv.tv_usec / 1000) % 1000);
    } else {
        milli = 0;
    }
    std::sprintf(buf, "_upload_%02d%02d%02d%02d%03d", year2, month, day, seconds, milli);
    return std::string(buf);
}

int	main(int ac, char **av)
{
	Data	data;

	if (ac != 2)
	{
		std::cerr << "Error: Bad arguments!\nUsage: ./webserv [configuration file]" << std::endl;
		exit(2);
	}
	(void)av;
	//parsing conf file//JOUDY au charbon
	data.setAddrinfo();
	data.addListener();
	data.pollLoop();
	data.clean();
}
