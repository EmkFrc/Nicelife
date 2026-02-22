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
volatile sig_atomic_t flag_signal = 1;

void turn_signal(int sig)
{
    (void)sig;
    flag_signal = 0;
}

int	main(int ac, char **av)
{
	Data	data;

	std::signal(SIGINT, turn_signal);
    std::signal(SIGTERM, turn_signal);
	if (ac != 2)
	{
		std::cerr << "Error: Bad arguments!\nUsage: ./webserv [configuration file]" << std::endl;
		exit(2);
	}
	try {
		// 1. Tokenization
		std::vector<std::string> tokens = ConfigTokenizer::tokenize(av[1]);

		// 2. Parsing
		ConfigParser parser(tokens);
		parser.parse();
		std::vector<ConfigServer> servers = parser.getServers();

		// 3. Vérification du résultat
		std::cout << "Nombre de serveurs configurés : " << servers.size() << std::endl;
		for (size_t i = 0; i < servers.size(); ++i) {
			std::cout << "Serveur " << i << " sur le port " << servers[i].getPort() << std::endl;
			std::cout << "  - Root: " << servers[i].getRoot() << std::endl;
			std::cout << "  - Nombre de locations: " << servers[i].getLocations().size() << std::endl;
		}
        data.setServers(servers);
	} catch (const std::exception& e) {
		std::cerr << "Configuration error : " << e.what() << std::endl;
		return 1;
	}
	for (int serverIndex = 0; serverIndex < (int)data.getServers().size(); serverIndex++)
	{
		data.setAddrinfo(serverIndex);
		data.addListener(serverIndex);
	}
	data.pollLoop();
	data.clean();
}
