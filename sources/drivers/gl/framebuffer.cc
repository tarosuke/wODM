/** Framebuffer object
 * Copyright (C) 2016,2024 tarosuke<webmaster@tarosuke.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "gl/framebuffer.h"
#include "gl/gl.h"
#include <assert.h>



namespace GL {

	Framebuffer::Framebuffer(
		unsigned width, unsigned height, Format format, bool withDepth) :
		Texture(width, height, format),
		fbID(NewID()),
		dbID(NewDB()) {
		Assign(width, height, format, withDepth);
	}

	Framebuffer::Framebuffer(Size size, Format format, bool withDepth) :
		Texture(size.width, size.height, format),
		fbID(NewID()),
		dbID(NewDB()) {
		Assign(size.width, size.height, format, withDepth);
	}

	void Framebuffer::Assign(
		unsigned width, unsigned height, Format format, bool withDepth) {
		Key k(*this);

		// デプスバッファ確保
		if (withDepth) {
			glBindRenderbuffer(GL_RENDERBUFFER, dbID);
			glRenderbufferStorage(
				GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}

		// カラーバッファ割り当て
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureID(),
			0);

		// デプスバッファ割り当て
		glFramebufferRenderbuffer(
			GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, dbID);
	}

	unsigned Framebuffer::NewID() {
		unsigned id(0);
		glGenFramebuffers(1, &id);
		return id;
	}
	unsigned Framebuffer::NewDB() {
		unsigned id(0);
		glGenRenderbuffers(1, &id);
		return id;
	}



	Framebuffer::~Framebuffer() {
		if (fbID) {
			glDeleteFramebuffers(1, &fbID);
		}
		if (dbID) {
			glDeleteRenderbuffers(1, &dbID);
		}
	}


	/** Key/CanKey
	 * RAIIによるアクティベート管理
	 */
	Framebuffer::Key::Key(Framebuffer& fb) {
		glBindFramebuffer(GL_FRAMEBUFFER, fb.fbID);
	}
	Framebuffer::Key::~Key() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

}
