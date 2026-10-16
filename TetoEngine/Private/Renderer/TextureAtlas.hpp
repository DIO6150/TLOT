#pragma once

#include <map>
#include <memory>
#include <unordered_map>

#include <TetoEngine/Common/ID.hpp>
#include <TetoEngine/Resource/Image.hpp>
#include <TetoEngine/Resource/ResourceView.hpp>

#include <glm/glm.hpp>


namespace TetoEngine
{
	class Application;
	
	struct Quad
	{
		float u, v;
		float w, h;
		float d;
	};

	class TextureAtlas
	{
	public:
		TextureAtlas(Application * app, uint32_t width, uint32_t height, uint32_t depth);

		void insertImage(ID64 handle);

		uint32_t width() const;
		uint32_t height() const;
		void generate();
		
		uint32_t handle() const;

	private:
		Application * app_ = nullptr;
		uint32_t width_;
		uint32_t height_;
		uint32_t depth_;

		uint32_t handle_ = 0;
		bool dirty_ = false;

		struct PendingImage
		{
			std::vector<uint8_t> data;
			uint32_t width, height;
			uint32_t x, y;
			uint32_t depth;

			ID64 handle;

			PendingImage() = default;
			PendingImage(ResourceView<Image> image)
				: data   {image->data}
				, width  {image->width}
				, height {image->height}
				, handle {image.handle()}
			{
				
			}
		};

		std::vector<Quad> free_;

		std::vector<PendingImage> pendingImages_;
		std::map<ID64, Quad> textures_;
	};
}
