#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <sstream>

namespace TLOT
{
	enum class TokenType {
		TEXT,
		TAG
	};

	struct Token {
		TokenType type;
		std::string value;
	};

	inline std::vector<std::string> SplitString(const std::string& s, char delimiter) {
		std::vector<std::string> tokens;
		std::string token;
		std::istringstream tokenStream(s);
		while (std::getline(tokenStream, token, delimiter)) {
			tokens.push_back(token);
		}
		return tokens;
	}

	inline bool IsValidTag(const std::string& innerText) {
		if (innerText.empty()) {
			return true; 
		}

		std::vector<std::string> parts = SplitString(innerText, ',');
		
		for (const std::string& part : parts) {
			if (part.substr(0, 2) == "C:" && part.length() > 2) continue;
			
			if (part.substr(0, 2) == "X:" && part.length() > 2) continue;
			
			if (part.substr(0, 2) == "S:" && part.length() > 2) continue;
			
			return false; 
		}
		
		return true;
	}

	inline std::vector<Token> TokenizeString(const std::string& input) {
		std::vector<Token> tokens;
		std::string currentText = "";
		
		size_t i = 0;
		while (i < input.length()) {
			if (input[i] == '{') {

				size_t closingPos = input.find('}', i + 1);
				
				if (closingPos != std::string::npos) {

					std::string inner = input.substr(i + 1, closingPos - i - 1);

					inner.erase(std::remove(inner.begin(), inner.end(), ' '), inner.end());

					if (IsValidTag(inner)) {

						if (!currentText.empty()) {
							tokens.push_back({TokenType::TEXT, currentText});
							currentText = "";
						}

						tokens.push_back({TokenType::TAG, inner});
						
						i = closingPos + 1;
						continue;
					}
				}
				
				currentText += input[i];
				i++;
			} else {
				currentText += input[i];
				i++;
			}
		}
		
		if (!currentText.empty()) {
			tokens.push_back({TokenType::TEXT, currentText});
		}
		
		return tokens;
	}
}