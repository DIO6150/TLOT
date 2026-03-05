#pragma once

#include <stdint.h>

#include <engine/utils/ShortcutTypes.hpp>

namespace Engine::Internal {
	class FrameBuffer {
	public:
		void AddColorAttachment ();

		uint32_t Get () const;
		uint32_t GetColorAttachment (uint32_t binding) const;


		FrameBuffer  (int32_t width, int32_t height);
		FrameBuffer (const FrameBuffer & other) = default;
		FrameBuffer (FrameBuffer & other)       = default;
		FrameBuffer (FrameBuffer && other)      = default;
		~FrameBuffer ();

	private:
		uint32_t         mFrameBuffer;
		uint32_t         mRenderBuffer;
		Vector<uint32_t> mColorAttachments;

		int32_t mWidth;
		int32_t mHeight;
	};
}