/*****************************************************************************
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

#include "gl/eye.h"
#include "gl/gl.h"



/***** GL用のEye
 */


namespace GL {

	Eye::Eye(unsigned width, unsigned height) :
		::Eye(width, height),
		framebuffer(width, height) {};

	void Eye::Prepare() const {
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);

		glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glViewport(0, 0, width, height);
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(projection);
		glMatrixMode(GL_MODELVIEW);
	};


	void Eye::Identity() const { glLoadIdentity(); };
	void Eye::PixelByPixel(float depth) const {
		glLoadIdentity();
		glScalef(2.0f / max, 2.0f / max, 1);
		glMultMatrixf(eye2Head);

#if 0
		// チェック用スケール(256ごとに点)
		glBegin(GL_POINTS);
		for (unsigned x(0); x < width / 2; x += 256) {
			for (unsigned y(0); y < height / 2; y += 256) {
				glVertex3f(x, y, -1);
			}
		}
		const float w(0.5f * width - 1);
		const float h(0.5f * height - 1);
		glVertex3f(-w, -h, -1);
		glVertex3f(w, -h, -1);
		glVertex3f(-w, h, -1);
		glVertex3f(w, h, -1);
		glEnd();
#endif
	};
	void Eye::GUI(float depth) const {
		glLoadIdentity();
		glScalef(2.0f / max, -2.0f / max, 1);
		glMultMatrixf(eye2Head);
	};
	void Eye::Vertical11() const {
		glLoadIdentity();
		glScalef(height / width, 1, 1);
		glMultMatrixf(eye2Head);
	};
	void Eye::Horizontal11() const {
		glLoadIdentity();
		glScalef(1, width / height, 1);
		glMultMatrixf(eye2Head);
	};
	void Eye::Short11() const {
		const float r(min / max);
		glLoadIdentity();
		glScalef(r, r, 1);
		glMultMatrixf(eye2Head);
	};
	void Eye::Long11() const {
		const float r(max / min);
		glLoadIdentity();
		glScalef(r, r, 1);
		glMultMatrixf(eye2Head);
	};
	void Eye::Pose(const tb::Matrix<4, 4, float>& m) const {
		glLoadMatrixf(m);
		glMultMatrixf(eye2Head);
	};
}
