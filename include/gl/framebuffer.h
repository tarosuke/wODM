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

#pragma once

#include "texture.h"



namespace GL {

	struct Framebuffer : public Texture {
		/** Size
		 * サイズ
		 */
		struct Size {
			unsigned width;
			unsigned height;
		};

		/** Key
		 * RAIIによるアクティベート管理
		 */
		class Key {
			Key() = delete;
			Key(const Key&) = delete;
			void operator=(const Key) = delete;

		public:
			Key(Framebuffer&);
			~Key();

		private:
		};

		Framebuffer() : Texture(1, 1, Texture::RGB), fbID(0), dbID(0) {};
		Framebuffer(
			unsigned width,
			unsigned height,
			Format = Texture::RGB,
			bool withDepth = true);
		Framebuffer(Size, Format = Texture::RGB, bool withDepth = true);

		Framebuffer(Framebuffer&& o) :
			Texture(std::move(o)),
			fbID(o.fbID),
			dbID(o.dbID) {};
		void operator=(Framebuffer&& o) {
			*(Texture*)this = std::move(o);
			fbID = o.fbID;
			dbID = o.dbID;
			o.fbID = o.dbID = 0;
		}

		~Framebuffer();

		unsigned GetFBID() const { return fbID; };
		unsigned GetColorBufferID() const { return TextureID(); };
		unsigned GetDepthBufferID() const { return dbID; };

	private:
		unsigned fbID;
		unsigned dbID;
		static unsigned NewID();
		static unsigned NewDB();

		void Assign(unsigned width, unsigned height, Format, bool withDepth);

		Framebuffer(const Framebuffer&) = delete;
		void operator=(const Framebuffer&) = delete;
	};
};
