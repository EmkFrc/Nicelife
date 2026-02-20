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
	std::map<std::string, bool>	allowed_methods; // GET, POST, DELETE
	std::map<std::string, std::string>	cgi;
	std::string					upload_store;
	bool						upload_enable;


	Location() :
		path(""),
		root(""),
		index("index.html"),
		autoindex(false),
		return_code(-1),
		return_url(""),
		cgi(),
		upload_store("data"),
		upload_enable(true)
		{
			allowed_methods["GET"] = true;
			allowed_methods["POST"] = false;
			allowed_methods["DELETE"] = false;
		}
};

#endif
