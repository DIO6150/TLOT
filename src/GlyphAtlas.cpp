#include <algorithm>

#include <OpenGL/TextureAtlas/GlyphAtlas.hpp>
#include <Core/Logger.hpp>

#include "glad/glad.h"

using namespace TLOT;


void GlyphAtlas::InsertGlyph(char character, Glyph glyph, uint32_t fontSize, ResourceHandle font)
{
	m_pendingGlyphs.push_back({character, fontSize, glyph, font});
	m_shouldGenerate = true;
}

TextureQuad GlyphAtlas::GetGlyph(char character, uint32_t fontSize, ResourceHandle font)
{
	ResourceHandle handle = GenerateCharacterHash(character, fontSize, font);
    
    auto it = m_quads.find(handle);
    if (it != m_quads.end()) {
        return it->second;
    }
    
    // Retourne un quad vide/erreur si le caractère n'existe pas
    return TextureQuad {};
}

void GlyphAtlas::Generate()
{
	if (!m_shouldGenerate || m_pendingGlyphs.empty()) 
        return;
        
    m_quads.clear();

    // 1. Identifier les polices uniques et leur assigner un index de Layer
    std::unordered_map<ResourceHandle, uint32_t> fontToLayer;
    uint32_t layerCount = 0;
    
    for (const auto& pending : m_pendingGlyphs) {
        if (fontToLayer.find(pending.font) == fontToLayer.end()) {
            fontToLayer[pending.font] = layerCount++;
        }
    }

    // 2. Allocation du buffer 3D (Largeur * Hauteur * Couches)
    std::vector<uint8_t> atlasPixels(m_width * m_height * layerCount, 0);

    // 3. Tri des glyphes : d'abord par couche (police), puis par hauteur décroissante
    std::sort(m_pendingGlyphs.begin(), m_pendingGlyphs.end(), 
        [&fontToLayer](const PendingGlyph& a, const PendingGlyph& b) {
            if (a.font != b.font) {
                return fontToLayer[a.font] < fontToLayer[b.font];
            }
            return a.data.height > b.data.height;
        });

    // Variables de packing
    size_t currentX = 0;
    size_t currentY = 0;
    size_t shelfHeight = 0;
    ResourceHandle currentFont = 0;
    bool isFirst = true;

    // 4. Remplissage de l'atlas 3D
    for (const auto& pending : m_pendingGlyphs) {
        const Glyph& glyph = pending.data;
        uint32_t layerIndex = fontToLayer[pending.font];

        // Si on change de police, on réinitialise le packing pour la nouvelle couche
        if (isFirst || pending.font != currentFont) {
            currentFont = pending.font;
            currentX = 0;
            currentY = 0;
            shelfHeight = 0;
            isFirst = false;
        }

        // Shelf Packing standard (à l'échelle de la couche courante)
        if (currentX + glyph.width > m_width) {
            currentX = 0;
            currentY += shelfHeight;
            shelfHeight = 0;
        }

        if (currentY + glyph.height > m_height) {
            Logger::log(LogLevel::Error, "Atlas layer too small for this font layout!");
            break; 
        }

        // Calcul du décalage mémoire pour atteindre le bon Layer
        size_t layerOffset = static_cast<size_t>(layerIndex) * m_width * m_height;

        // Copie des pixels vers la bonne couche
        for (unsigned int row = 0; row < glyph.height; ++row) {
            for (unsigned int col = 0; col < glyph.width; ++col) {
                size_t atlasIndex = layerOffset + ((currentY + row) * m_width) + (currentX + col);
                size_t glyphIndex = (row * glyph.width) + col;

                atlasPixels[atlasIndex] = glyph.bitmap[glyphIndex];
            }
        }

        // Coordonnées UV de texture
        TextureQuad quad;
        quad.x = static_cast<float>(currentX) / static_cast<float>(m_width);
        quad.y = static_cast<float>(currentY) / static_cast<float>(m_height);
        quad.w = static_cast<float>(glyph.width) / static_cast<float>(m_width);
        quad.h = static_cast<float>(glyph.height) / static_cast<float>(m_height);
        
        // 🎯 On stocke l'index de la couche dans la variable depth !
        quad.depth = static_cast<float>(layerIndex); 

        // Enregistrement avec le nouveau système de Hash à 3 paramètres
        uint64_t handle = GenerateCharacterHash(pending.character, pending.fontSize, pending.font);
        m_quads[handle] = quad;

        currentX += glyph.width;
        if (glyph.height > shelfHeight) {
            shelfHeight = glyph.height;
        }
    }

    // 5. Envoi des données à OpenGL en tant que TEXTURE_2D_ARRAY
    glGenTextures(1, &m_handle);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_handle);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    // Allocation et envoi via glTexImage3D
    glTexImage3D(
        GL_TEXTURE_2D_ARRAY, 0, GL_RED, 
        m_width, m_height, layerCount, // Largeur, Hauteur, Profondeur (Layers)
        0, GL_RED, GL_UNSIGNED_BYTE, atlasPixels.data()
    );

    // Paramétrage des filtres de l'Array Texture
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    m_shouldGenerate = false;
}

uint64_t GlyphAtlas::GenerateCharacterHash(char character, uint32_t fontSize, ResourceHandle font)
{
    uint64_t hash = font;
    hash ^= static_cast<uint64_t>(fontSize) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    hash ^= static_cast<uint64_t>(character) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    return hash;
}