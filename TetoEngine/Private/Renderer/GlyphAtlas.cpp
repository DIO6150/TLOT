
#include <glad/glad.h>

#include <TetoEngine/Private/Renderer/GlyphAtlas.hpp>
#include <TetoEngine/Utils/Logger.hpp>

#include <algorithm>

using namespace TetoEngine;


void GlyphAtlas::insertGlyph(char character, Glyph glyph, uint32_t fontSize, ID64 font)
{
	pendingGlyphs_.push_back({character, fontSize, glyph, font});
	dirty_ = true;
}

TextureQuad GlyphAtlas::getGlyph(char character, uint32_t fontSize, ID64 font)
{
	ID64 handle = generateCharacterHash(character, fontSize, font);
    
    auto it = quads_.find(handle);
    if (it != quads_.end()) {
        return it->second;
    }
    
    // Retourne un quad vide/erreur si le caractère n'existe pas
    return TextureQuad {};
}

void GlyphAtlas::generate()
{
	if (!dirty_ || pendingGlyphs_.empty()) 
        return;
        
    quads_.clear();

    // 1. Identifier les polices uniques et leur assigner un index de Layer
    std::unordered_map<ID64, uint32_t> fontToLayer;
    uint32_t layerCount = 0;
    
    for (const auto& pending : pendingGlyphs_) {
        if (fontToLayer.find(pending.font_) == fontToLayer.end()) {
            fontToLayer[pending.font_] = layerCount++;
        }
    }

    // 2. Allocation du buffer 3D (Largeur * Hauteur * Couches)
    std::vector<uint8_t> atlasPixels(width_ * height_ * layerCount, 0);

    // 3. Tri des glyphes : d'abord par couche (police), puis par hauteur décroissante
    std::sort(pendingGlyphs_.begin(), pendingGlyphs_.end(), 
        [&fontToLayer](const PendingGlyph& a, const PendingGlyph& b) {
            if (a.font_ != b.font_) {
                return fontToLayer[a.font_] < fontToLayer[b.font_];
            }
            return a.data_.height > b.data_.height;
        });

    // Variables de packing
    size_t currentX = 0;
    size_t currentY = 0;
    size_t shelfHeight = 0;
    ID64 currentFont = 0;
    bool isFirst = true;

    // 4. Remplissage de l'atlas 3D
    for (const auto& pending : pendingGlyphs_) {
        const Glyph& glyph = pending.data_;
        uint32_t layerIndex = fontToLayer[pending.font_];

        // Si on change de police, on réinitialise le packing pour la nouvelle couche
        if (isFirst || pending.font_ != currentFont) {
            currentFont = pending.font_;
            currentX = 0;
            currentY = 0;
            shelfHeight = 0;
            isFirst = false;
        }

        // Shelf Packing standard (à l'échelle de la couche courante)
        if (currentX + glyph.width_ > width_) {
            currentX = 0;
            currentY += shelfHeight;
            shelfHeight = 0;
        }

        if (currentY + glyph.height > height_) {
            Logger::log(LogLevel::Error, "Atlas layer too small for this font layout!");
            break; 
        }

        // Calcul du décalage mémoire pour atteindre le bon Layer
        size_t layerOffset = static_cast<size_t>(layerIndex) * width_ * height_;

        // Copie des pixels vers la bonne couche
        for (unsigned int row = 0; row < glyph.height; ++row) {
            for (unsigned int col = 0; col < glyph.width_; ++col) {
                size_t atlasIndex = layerOffset + ((currentY + row) * width_) + (currentX + col);
                size_t glyphIndex = (row * glyph.width_) + col;

                atlasPixels[atlasIndex] = glyph.bitmap_[glyphIndex];
            }
        }

        // Coordonnées UV de texture
        TextureQuad quad;
        quad.x = static_cast<float>(currentX) / static_cast<float>(width_);
        quad.y = static_cast<float>(currentY) / static_cast<float>(height_);
        quad.w = static_cast<float>(glyph.width_) / static_cast<float>(width_);
        quad.h = static_cast<float>(glyph.height) / static_cast<float>(height_);
        
        // 🎯 On stocke l'index de la couche dans la variable depth !
        quad.depth = static_cast<float>(layerIndex); 

        // Enregistrement avec le nouveau système de Hash à 3 paramètres
        ID64 handle = generateCharacterHash(pending.character_, pending.fontSize_, pending.font_);
        quads_[handle] = quad;

        currentX += glyph.width_;
        if (glyph.height > shelfHeight) {
            shelfHeight = glyph.height;
        }
    }

    // 5. Envoi des données à OpenGL en tant que TEXTURE_2D_ARRAY
    glGenTextures(1, &handle_);
    glBindTexture(GL_TEXTURE_2D_ARRAY, handle_);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    // Allocation et envoi via glTexImage3D
    glTexImage3D(
        GL_TEXTURE_2D_ARRAY, 0, GL_RED, 
        width_, height_, layerCount, // Largeur, Hauteur, Profondeur (Layers)
        0, GL_RED, GL_UNSIGNED_BYTE, atlasPixels.data()
    );

    // Paramétrage des filtres de l'Array Texture
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    dirty_ = false;
}

ID64 GlyphAtlas::generateCharacterHash(char character, uint32_t fontSize, ID64 font)
{
    ID64 hash = font;
    hash ^= static_cast<ID64>(fontSize) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    hash ^= static_cast<ID64>(character) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    return hash;
}