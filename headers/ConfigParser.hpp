#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

# include <iostream>
# include <vector>
# include "ConfigServer.hpp"

class	ConfigParser {
	private:
		std::vector<std::string>	_tokens;
		std::vector<ConfigServer>	_servers;
		size_t						_pos;

		// Fonctions de parsing (privees car interne sinon peut avoir un crash)
		ConfigServer parseServer();
		void parseLocation(ConfigServer& server);

		void checkSemicolon();
		void checkBrace();
		bool isNumber(const std::string& s);
		unsigned long	parseSize(std::string s);

	public:
		// Recoit les tokens
		ConfigParser(const std::vector<std::string>& tokens);
		~ConfigParser();

		// Parsing, ne doit rester aucun tokens apres parsing dexs blocs server sinon erreur
		void parse();

		// Recupere le resultat
		const std::vector<ConfigServer>& getServers() const;
};

#endif