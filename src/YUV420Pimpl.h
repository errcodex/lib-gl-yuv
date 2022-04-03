#pragma once
#include "gl/glew.h"
#include <vector>
#include "YUV420P.h"
class YUV420Pimpl{
public:
	enum class RenderMode
	{
		FIT,
		FILL,
		HIDDEN
	};
public:
	YUV420Pimpl() = default;
	virtual ~YUV420Pimpl() = default;
	bool Initialize();
	void Release();
	void SendYUV(const unsigned char* data, unsigned int width, unsigned int height);
	void Render();
	void SetRenderMode(RenderMode renderMode);
	void SetViewportSize(unsigned int width, unsigned int height);

private:
	bool GenerateVertexShader(GLuint& shader, const char* source);
	bool GenerateFragmentShader(GLuint& shader, const char* source);
	bool GenerateShader(const GLenum type, GLuint& shader, const char* source);
	void ReleaseShader(GLuint& shader);
	bool GenerateProgram(GLuint& shader, std::vector<GLuint> shaders = {});
	bool GenerateTexture(GLuint& texture);
	void ApplyTexture(GLuint index, GLint position, GLuint texture, unsigned int width, unsigned int height, const unsigned char* pixels);
	void UpdateViewport();

	unsigned int glW = 320, glH = 180;
	unsigned int vW = 320, vH = 180;
	GLuint vertexShader, fragmentShader, shaderProgram;
	GLuint vertextBuffer;
	GLuint textureY, textureU, textureV;
	GLint posTexY, posTexU, posTexV;
	RenderMode renderMode = RenderMode::FIT;
	typedef struct {
		float x;
		float y;
	}COOR2D, TEX2D;
	typedef struct {
		COOR2D c;
		TEX2D t;
	}VERTEX;
	// ¶¥µã
	VERTEX vertices[4] = {
		//  --x-, --y-, texX, texY
			1.0f, 1.0f, 1.f, 1.f, // right top
			1.0f, -1.f, 1.f, 0.f, // right button
			-1.f, -1.f, 0.f, 0.f, // left bottom
			-1.f, 1.0f, 0.f, 1.f // left top
	};

	const char* vertexSource = R"GLSL(
		#version 460 core
		layout (location = 0) in vec4 position;
		layout (location = 1) in vec2 coordinate;
		out vec2 texture;
		void main()
		{
			gl_Position = position;
			texture.x = coordinate.x;
			texture.y = 1 - coordinate.y;
		}
	)GLSL";

	const char* fragmentSource = R"GLSL(
		#version 460 core
		in vec2 texture;
		uniform sampler2D tex_y;
		uniform sampler2D tex_u;
		uniform sampler2D tex_v;

		void main()
		{
			vec3 YUV;
            vec3 RGB;
            YUV.x = texture2D(tex_y, texture).r;
            YUV.y = texture2D(tex_u, texture).r;
            YUV.z = texture2D(tex_v, texture).r;
            RGB = mat3(
				1.0, 1.0, 1.0, 
                -0.00093, -0.3437, 1.77216,
                1.401687, -0.71417, 0.00099
			) * ( YUV + vec3(0, -0.5, -0.5));
            gl_FragColor = vec4(RGB, 1.0);
		}
	)GLSL";

};