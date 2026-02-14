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
	std::map<int, std::string>	error_pages;
	int							return_code;
	std::string					return_url; // redirection
	std::vector<std::string>	allowed_methods; // GET, POST, DELETE
	std::map<std::string, std::string>	cgi;
	std::string					upload_store;
	bool						upload_enable;

	// Valeurs par defaut
	Location() :
		path(""),
		root(""),
		index("index.html"),
		autoindex(false),
		return_code(0),
		return_url(""),
		cgi(),
		upload_store(""),
		upload_enable(false)
		{
			allowed_methods.push_back("GET");
		}
};

#endif
