#include "YUV420Pimpl.h"
#include <algorithm>

bool YUV420Pimpl::Initialize()
{
	if (!GenerateVertexShader(vertexShader, vertexSource))
		return false;
	if (!GenerateFragmentShader(fragmentShader, fragmentSource)) {
		ReleaseShader(vertexShader);
		return false;
	}
	if (!GenerateProgram(shaderProgram, { vertexShader,fragmentShader }))
	{
		ReleaseShader(vertexShader);
		ReleaseShader(fragmentShader);
		return false;
	}
	ReleaseShader(vertexShader);
	ReleaseShader(fragmentShader);

	// 顶点数据
	glGenBuffers(1, &vertextBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertextBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// 顶点&片段 着色器
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(VERTEX), 0); // vertex position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (void*)sizeof(COOR2D)); // texture position
	glEnableVertexAttribArray(1);

	posTexY = glGetUniformLocation(shaderProgram, "tex_y");
	posTexU = glGetUniformLocation(shaderProgram, "tex_u");
	posTexV = glGetUniformLocation(shaderProgram, "tex_v");

	// 分别创建Y/U/V三个分量的纹理
	GenerateTexture(textureY);
	GenerateTexture(textureU);
	GenerateTexture(textureV);
	//TODO
	return true;
}

void YUV420Pimpl::Release()
{
	glDeleteProgram(shaderProgram);
}

void YUV420Pimpl::SendYUV(const unsigned char* data, unsigned int width, unsigned int height)
{
	vW = width;
	vH = height;
	ApplyTexture(0, posTexY, textureY, width, height, data);
	ApplyTexture(1, posTexU, textureU, width / 2, height / 2, data + width * height);
	ApplyTexture(2, posTexV, textureV, width / 2, height / 2, data + width * height * 5 / 4);
	Render();
}

void YUV420Pimpl::Render()
{
	glDrawArrays(GL_QUADS, 0, 4);
}

void YUV420Pimpl::SetRenderMode(RenderMode renderMode)
{
	if (this->renderMode != renderMode) {
		this->renderMode = renderMode;
		UpdateViewport();
	}
}

void YUV420Pimpl::UpdateViewport()
{
	int x = 0, y = 0;
	unsigned int w = glW, h = glH;
	double glRatio = (double)glW / glH, videoRatio = (double)vW / vH;

	switch (this->renderMode)
	{
	case YUV420Pimpl::RenderMode::FIT:
		if (glRatio > videoRatio) //同等高度下，宽度gl>video
		{
			w = videoRatio * glH;
			h = glH;
			y = 0;
			x = labs(glW - w) / 2;
		}
		else if (glRatio < videoRatio) {
			w = glW;
			h = glW / videoRatio;
			x = 0;
			y = labs(glH - h) / 2;
		}
		break;
	case YUV420Pimpl::RenderMode::FILL:
		break;
	case YUV420Pimpl::RenderMode::HIDDEN:
		if (glRatio > videoRatio) //同等高度下，宽度gl>video
		{
			w = glW;
			h = glW / videoRatio;
			x = 0;
			y = (int)(glH - h) / 2;
		}
		else if (glRatio < videoRatio) {
			w = glH * videoRatio;
			h = glH;
			y = 0;
			x = (int)(glW - w) / 2;
		}
		break;
	default:
		break;
	}
	glViewport(x, y, w, h);
}

void YUV420Pimpl::SetViewportSize(unsigned int width, unsigned int height)
{
	if (glW != width || glH != height)
	{
		glW = width;
		glH = height;
		UpdateViewport();
	}
}

bool YUV420Pimpl::GenerateVertexShader(GLuint& shader, const char* source) {
	return GenerateShader(GL_VERTEX_SHADER, shader, source);
}

bool YUV420Pimpl::GenerateFragmentShader(GLuint& shader, const char* source) {
	return GenerateShader(GL_FRAGMENT_SHADER, shader, source);
}

bool YUV420Pimpl::GenerateShader(const GLenum type, GLuint& shader, const char* source)
{
	shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	return success;
}

void YUV420Pimpl::ReleaseShader(GLuint& shader) {
	glDeleteShader(shader);
	glDeleteTextures(1, &textureY);
	glDeleteTextures(1, &textureU);
	glDeleteTextures(1, &textureV);
	glDeleteBuffers(1, &vertextBuffer);
}

bool YUV420Pimpl::GenerateProgram(GLuint& shader, std::vector<GLuint> shaders)
{
	shader = glCreateProgram();

	std::for_each(shaders.begin(), shaders.end(), [&shader](GLuint _shader) {
		glAttachShader(shader, _shader);
		});
	glLinkProgram(shader);
	glUseProgram(shader);

	GLint success;
	glGetProgramiv(shader, GL_LINK_STATUS, &success);
	return success;
}

bool YUV420Pimpl::GenerateTexture(GLuint& texture)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//TODO
	return true;
}

void YUV420Pimpl::ApplyTexture(GLuint index, GLint position, GLuint texture, unsigned int width, unsigned int height, const unsigned char* pixels)
{
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);
	glUniform1i(position, index);
}
