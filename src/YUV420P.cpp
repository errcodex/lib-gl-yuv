#include "YUV420P.h"
#include "../src/YUV420Pimpl.h"

YUV420P::YUV420P() :impl(std::make_unique<YUV420Pimpl>())
{

}

bool YUV420P::Initialize()
{
	return impl->Initialize();
}

void YUV420P::Release()
{
	impl->Release();
}

void YUV420P::SendYUV(const unsigned char* data, unsigned int width, unsigned int height)
{
	impl->SendYUV(data, width, height);
}

void YUV420P::Render()
{
	impl->Render();
}

void YUV420P::SetRenderMode(RenderMode renderMode)
{
	impl->SetRenderMode((YUV420Pimpl::RenderMode)renderMode);
}

void YUV420P::SetViewportSize(unsigned int width, unsigned int height)
{
	impl->SetViewportSize(width, height);
}
