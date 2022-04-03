#pragma once

#include <memory>

class YUV420Pimpl;
class YUV420P {
public:
	enum class RenderMode
	{
		FIT,
		FILL,
		HIDDEN
	};
public:
	YUV420P();
	virtual ~YUV420P() = default;

	bool Initialize();
	void Release();
	void SendYUV(const unsigned char* data, unsigned int width, unsigned int height);
	void Render();
	void SetRenderMode(RenderMode renderMode);
	void SetViewportSize(unsigned int width, unsigned int height);

private:
	std::unique_ptr<YUV420Pimpl> impl = nullptr;
};