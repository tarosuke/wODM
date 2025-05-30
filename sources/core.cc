﻿/** OpenGL番Core
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
		// Stickies::UpdateAll();
		Widget::UpdateAll(pose);
		// World::UpdateAll();
		GL::Scenery::UpdateAll();

		for (auto& e : eyes) {
			GL::Framebuffer::Key fb(e.framebuffer);
			glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			glViewport(0, 0, e.width, e.height);

			glMatrixMode(GL_PROJECTION);
			glLoadMatrixf(e.projection);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			// Stickies::DrawAll();
			Widget::DrawAll();
			glLoadMatrixf(pose * e.eye2Head);
			// World::DrawAll();
			GL::Scenery::DrawAll();
			// World::TrawAll();
			Widget::TrawAll();
			// Stickies::TrawAll();

			Finish(e);
		}
		Finish();
	}
}
