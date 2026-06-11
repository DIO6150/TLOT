#pragma once

#include <vector>

namespace TLOT
{
	struct Glyph {
		std::vector<unsigned char> bitmap; // Les pixels monochromes (0 à 255)
		int width;                // Largeur du glyphe en pixels
		int height;               // Hauteur du glyphe en pixels
		int bearingX;                      // Décalage horizontal depuis l'origine
		int bearingY;                      // Décalage vertical depuis la ligne de base
		int advance;              // Distance jusqu'au prochain caractère
	};
}