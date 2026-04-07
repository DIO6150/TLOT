#pragma once

#include <stdint.h>
#include <vector>

namespace TLOT
{
    class FBO {
	public:
		void AddColorAttachment ();

		uint32_t Get () const;
		uint32_t GetColorAttachment (uint32_t binding) const;


		FBO (int32_t width, int32_t height);
		FBO (FBO const & other) = default;
		FBO (FBO & other)       = default;
		FBO (FBO && other)      = default;
		~FBO ();

	private:
        std::vector<uint32_t> mColorAttachments;

		uint32_t mFrameBuffer;
		uint32_t mRenderBuffer;

		int32_t mWidth;
		int32_t mHeight;
	};
}