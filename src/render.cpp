#include "StdAfx.h"
#include "render.h"
#include "global.h"
#include "misc.h"
#include "gl.h"

static ci::gl::TextureRef tex;
static ci::gl::TextureRef bloomTex;

namespace shaders
{
	static Shader a;
}

namespace render
{
	Image toDraw(imageSize,imageSize), bloomSurface(imageSize,imageSize);

	void init()
	{
		shaders::a = Shader("a", "a");
		gl::Texture::Format fmt;fmt.setInternalFormat(GL_RGBA16F);
		bloomTex = gl::Texture::create(image.w, image.h, fmt);
		tex = gl::Texture::create(image.w, image.h, fmt);
	}

	static void drawRect() {
		auto ctx = gl::context();

		ctx->getDrawTextureVao()->bind();
		//ctx->getDrawTextureVbo()->bind(); // this seems to be unnecessary

		ctx->drawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	void renderApp()
	{
		glClampColor(GL_CLAMP_READ_COLOR_ARB, GL_FALSE);
		glClampColor(GL_CLAMP_VERTEX_COLOR_ARB, GL_FALSE);
		glClampColor(GL_CLAMP_FRAGMENT_COLOR_ARB, GL_FALSE);

		tex->bind(0);
		//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
		//GETINT(maxAnisotropy, "min=0 max=32", 16);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.w, image.h, GL_RGB, GL_FLOAT, &toDraw.data[0]);
		//glGenerateMipmapEXT(GL_TEXTURE_2D);
		//glGenerateMipmap(GL_TEXTURE_2D);
		//tex.setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
		
		tex->setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		bloomTex->bind(0);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.w, image.h, GL_RGB, GL_FLOAT, &bloomSurface.data[0]);
		tex->setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		tex->bind(0);
		bloomTex->bind(1);
		glClearColor(0,0,0,0);
		gl::enableAlphaBlending();
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaders::a.id);
		shaders::a.SendConfigUniforms();
		glUniformMatrix3fv(glGetUniformLocation(shaders::a.id, "toHSL"), 1, false, (float*)&toHsv);
		glUniformMatrix3fv(glGetUniformLocation(shaders::a.id, "toHSLinv"), 1, false, (float*)&toHsvInv);
		glUniform1i(glGetUniformLocation(shaders::a.id, "tex"), 0);
		glUniform1i(glGetUniformLocation(shaders::a.id, "bloomTex"), 1);
		
		//GETFLOAT(bloomOpacity, "step=.001 group=bloom", .01);
		const float bloomOpacity = .01;
		glUniform1f(glGetUniformLocation(shaders::a.id, "bloomOpacity"), pow(2.0f, bloomOpacity)-1);

		glUniform2f(glGetUniformLocation(shaders::a.id, "tex_size"), image.w, image.h);
		
		gl::setMatricesWindow(ivec2(1, 1));
		drawRect();
		//glScalef(zoom,zoom,1);
		/*glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex2f(0, 0);
		glTexCoord2f(1, 0); glVertex2f(windowWidth, 0);
		glTexCoord2f(1, 1); glVertex2f(windowWidth, windowHeight);
		glTexCoord2f(0, 1); glVertex2f(0, windowHeight);
		glEnd();
		glPopMatrix();*/

	}
}