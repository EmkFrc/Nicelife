#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <iostream>
# include <vector>
# include <map>

struct	Location {
	std::string					path;
	std::string					root;
	std::string					index;
	bool						autoindex; // on/off
	std::string					url; // redirection
	std::vector<std::string>	allowed_methods; // GET, POST, DELETE
	std::map<std::string, std::string>	cgi;

	// Valeurs par defaut
	Location() : path(""), root(""), index("index.html"), autoindex(false), url("") {
		allowed_methods.push_back("GET");
	}
};

#endif
