#ifndef CONFIGTOKENIZER_HPP
# define CONFIGTOKENIZER_HPP

# include <iostream>
# include <vector>
# include <sstream>
# include <fstream>

class	ConfigTokenizer {
	public:
		static std::vector<std::string> tokenize(std::string const& filePath) {
			// 1. Lecture de fichier
			std::ifstream file(filePath.c_str());
			if (!file.is_open()) {
				throw std::runtime_error("Error : Impossible to open file " + filePath);
			}

			std::string fullContent;
			std::string line;

			// 2. Lecture ligne par ligne + suppression des commentaires
			while (std::getline(file, line)) {
				size_t commentPos = line.find('#');
				if (commentPos != std::string::npos) {
					line = line.substr(0, commentPos);
				}
				fullContent += line + " ";
			}
			file.close();

			// 3. Espaces avant et apres les caracteres speciaux
			std::string spacedContent = spaceSpecialChars(fullContent);

			// 4. Recuperation des tokens
			std::vector<std::string> tokens;
			std::stringstream ss(spacedContent);
			std::string token;
			while (ss >> token) {
				tokens.push_back(token);
			}
			if (tokens.empty()) {
				throw std::runtime_error("Error : Configuration file is empty");
			}
			return tokens;
		}

	private:
		static std::string spaceSpecialChars(const std::string& input) {
			std::string result;
			for (size_t i = 0; i < input.length(); ++i) {
				if (input[i] == '{' || input[i] == '}' || input[i] == ';') {
					result += " ";
					result += input[i];
					result += " ";
				} else {
					result += input[i];
				}
			}
			return result;
		}
};

#endif
