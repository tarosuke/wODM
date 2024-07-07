/** Skybox
 * Copyright (C) 2017,2019 tarosuke<webmaster@tarosuke.net>
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

#include <assert.h>
#include <syslog.h>

#include "gl/gl.h"
#include "gl/scenery.h"



#define elementsOf(a) (sizeof(a) / sizeof(a[0]))


class Skybox : public Scenery {

	Skybox(const Params& params, const tb::Image<tb::Pixel<tb::u8>>& image)
		: Scenery(params, image){};

	static constexpr int scale = 5000;
	struct Factory : tb::Factory<Scenery> {
		Scenery* New(const Param*) final;
		unsigned Score(const Param* param) final {
			const Scenery::Param* const p(
				dynamic_cast<const Scenery::Param*>(param));
			assert(p);

			const unsigned w(p->image.Width());
			const unsigned h(p->image.Height());
			if (!w || !h) {
				return 0;
			}
			const float r((3.0 * w) / (4.0 * h));
			const float d(1.0 - r);
			return 65536 / (d * d + 1);
		};
	};
	static Factory factory;
	static unsigned indexes[];
	static GL::VBO::V_UV vertexes[];
};


Skybox::Factory Skybox::factory;

unsigned Skybox::indexes[] = {
	0,	2,	1, 2,  3,  1, // left
	2,	4,	3, 4,  5,  3, // front
	4,	6,	5, 6,  7,  5, // right
	6,	8,	7, 8,  9,  7, // rear
	2,	10, 4, 10, 11, 4, // top
	12, 13, 9, 13, 7,  9 // bottom
};

GL::VBO::V_UV Skybox::vertexes[] = {
	// 0:左後方
	{{-scale, scale, scale}, {0.0 / 4, 1.0 / 3}},
	{{-scale, -scale, scale}, {0.0 / 4, 2.0 / 3}},
	// 2:左前方
	{{-scale, scale, -scale}, {1.0 / 4, 1.0 / 3}},
	{{-scale, -scale, -scale}, {1.0 / 4, 2.0 / 3}},
	// 4:右前方
	{{scale, scale, -scale}, {2.0 / 4, 1.0 / 3}},
	{{scale, -scale, -scale}, {2.0 / 4, 2.0 / 3}},
	// 6:右後方
	{{scale, scale, scale}, {3.0 / 4, 1.0 / 3}},
	{{scale, -scale, scale}, {3.0 / 4, 2.0 / 3}},
	// 8:左後方(後方テクスチャ用)
	{{-scale, scale, scale}, {4.0 / 4, 1.0 / 3}},
	{{-scale, -scale, scale}, {4.0 / 4, 2.0 / 3}},
	// 10:上後方左右
	{{-scale, scale, scale}, {1.0 / 4, 0.0 / 3}},
	{{scale, scale, scale}, {2.0 / 4, 0.0 / 3}},
	// 12:下後方左右
	{{-scale, -scale, -scale}, {4.0 / 4, 3.0 / 3}},
	{{scale, -scale, -scale}, {3.0 / 4, 3.0 / 3}},
};

Scenery* Skybox::Factory::New(const tb::Factory<Scenery>::Param* param) {
	const Scenery::Param* const p(dynamic_cast<const Scenery::Param*>(param));
	assert(p);

	const Params params = {
		numOfIndex : elementsOf(Skybox::indexes),
		index : Skybox::indexes,
		numOfVertex : elementsOf(Skybox::vertexes),
		vertex : Skybox::vertexes
	};

	return new Skybox(params, p->image);
};
