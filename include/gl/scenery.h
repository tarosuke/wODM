/** Scenery
 * Copyright (C) 2017,2019,2024,2025 tarosuke<webmaster@tarosuke.net>
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
 *
 * スカイスフィアやスカイボックスなどの抽象
 */
#pragma once

#include <filesystem>
#include <syslog.h>

#include <tb/factory.h>
#include <tb/image.h>
#include <tb/prefs.h>
#include <tb/string.h>

#include "gl/gl.h"
#include "gl/texture.h"
#include "model.h"


namespace GL {

	class Scenery : public Model_C {
		Scenery() = delete;
		Scenery(const Scenery&) = delete;
		void operator=(const Scenery&) = delete;

	public:
		using Factory = tb::Factory<Scenery, const tb::Image&>;
		using NullFactory = tb::Factory<Scenery>;
		static Scenery* New(const std::filesystem::path* path = 0);
		static void DrawAll() {
			if (stack) {
				// Allと言いつつstackのtopだけ描画
				glColor3f(1, 1, 1);
				glDisable(GL_CULL_FACE);
				stack->Draw();
				glEnable(GL_CULL_FACE);
				if (const auto e = glGetError()) {
					syslog(LOG_ERR, "%s:%u(%x).", __FILE__, __LINE__, e);
				}
			}
		};
		static void UpdateAll() {
			if (stack) {
				// Allと言いつつスタックトップだけが対象
				stack->Update();
			}
		};

	protected:
		static constexpr float scale = 9990; // 10km(far端よりは小さくする)
		Scenery(
			const Params&,
			const tb::Image&,
			const GL::Texture::Style& style = GL::Texture::defaultStyle);
		~Scenery() { stack = next; }
		virtual void Update() {};

	private:
		static Scenery* stack;
		Scenery* const next;
		static tb::Prefs<tb::String> path;
		static void UpdateInstance(Scenery*) noexcept(false);
	};

}
