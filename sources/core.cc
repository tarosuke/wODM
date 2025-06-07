/** OpenGL番Core
 * Copyright (C) 2024 tarosuke<webmaster@tarosuke.net>
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
#include "core.h"
#include "gl/gl.h"
#include "gl/glx.h"
#include "gl/scenery.h"
#include "widget.h"
#include <assert.h>
#include <stdio.h>
#include <syslog.h>
#include <tb/time.h>



tb::Timestamp Core::timestamp;
bool Core::keep(false);
template <> tb::Factory<Core>* tb::Factory<Core>::start(0);


void Core::Run() {
	pose.Identity();
	for (keep = true; keep;) {
		timestamp.Update();

		UpdatePose();

		// 各種Update
		Widget::UpdateAll(pose, timestamp);
		// World::Update();
		GL::Scenery::UpdateAll();

		for (auto& e : eyes) {
			GL::Framebuffer::Key fb(e.framebuffer);
			glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			glViewport(0, 0, e.width, e.height);

			glMatrixMode(GL_PROJECTION);
			glLoadMatrixf(e.projection);

			glMatrixMode(GL_MODELVIEW);

			// opaque stickies
			glLoadMatrixf(e.eye2Head);
			Widget::DrawNavigationAll();

			// opaque GUI
			Widget::DrawAll(e.eye2Head);

			// opaque World
			glLoadMatrixf(pose * e.eye2Head);
			// World::DrawAll();

			// Scenery
			GL::Scenery::DrawAll();

			// transparent World
			// World::TrawAll();

			// transparent GUI & Navigation
			Widget::TrawAll();

			Finish(e);
		}
		Finish();
	}
}
