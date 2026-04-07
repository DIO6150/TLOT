#pragma once

#include <cstdint>
#include <vector>

#include <iomanip>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include <stb/stb_image.h>

#include <glm/glm.hpp>


// généré par gemini, j'y connais rien en fonction robuste de hashing
// TODO: rework this, and if its good keep it idk
// Fonction utilitaire pour combiner les hashs
inline uint64_t hash_combine(uint64_t seed, uint64_t value) {
	// On utilise une constante (Golden Ratio) pour disperser les bits
	// Le décalage et le XOR permettent de mélanger la valeur actuelle avec la précédente
	return seed ^ (value + 0x9e3779b97f4a7c15LL + (seed << 6) + (seed >> 2));
}

inline std::string ReadFile (std::string path)
{
	std::ifstream file;
	file.open (path);
	if (!file.is_open ()) {
		file.close ();
		std::cout << "Couldn't open file : " << path << "\n";
		// TODO : LOGGING
		return ("");
	}

	std::stringstream buffer;
	buffer << file.rdbuf ();

	file.close ();

	return (buffer.str ());
}

inline bool load_texture (std::filesystem::path path, unsigned char *& data, size_t & width, size_t & height)
{
	int nb_channels;
	int _width, _height;

	data = stbi_load(path.string ().c_str (), &_width, &_height, &nb_channels, 4);

	if (!data)
	{
		printf ("%s(%s) : (%s) %s\n", __func__, __FILE__, path.string ().c_str (), stbi_failure_reason ());
		return (false);
	}

	width  = _width;
	height = _height;
	
	return (true);
}

inline void printMat4(glm::mat4 m, const std::string& name = "Matrix") {
    std::cout << name << ":" << std::endl;
    for (int i = 0; i < 4; i++) {
        std::cout << "  [ ";
        for (int j = 0; j < 4; j++) {
            // Note : GLM utilise un stockage en colonnes (column-major)
            // m[j][i] permet d'afficher la matrice de façon "visuelle" (ligne par ligne)
            std::cout << std::fixed << std::setprecision(3) << std::setw(8) << m[j][i] << " ";
        }
        std::cout << "]" << std::endl;
    }
}

inline std::ostream & operator<<(std::ostream& out, const glm::vec3 & v) {
	out << "{" << v.x << ", " << v.y << ", " << v.z << "}";
	return out;
}
