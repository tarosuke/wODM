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
	void Eye::PixelByPixel() const { glScalef(0.5 / width, 0.5 / height, 1); };
	void Eye::Vertical11() const { glScalef(height / width, 1, 1); };
	void Eye::Horizontal11() const { glScalef(1, width / height, 1); };
	void Eye::Short11() const { glScalef(min / width, min / height, 1); };
	void Eye::Long11() const { glScalef(max / width, max / height, 1); };
	void Eye::Pose(const tb::Matrix<4, 4, float>& m) const {
		glLoadMatrixf(m);
	};
}
