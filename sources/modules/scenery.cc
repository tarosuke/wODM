/** Scenery
 * Copyright (C) 2017,2019,2924 tarosuke<webmaster@tarosuke.net>
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

#include "gl/scenery.h"
#include <syslog.h>
#include <tb/canvas.h>



template <> tb::Factory<Scenery>* tb::Factory<Scenery>::start(0);
Scenery* Scenery::instance;


Scenery::Scenery(
	const Params& params, const tb::Image<tb::Pixel<tb::u8>>& image)
	: Model_C(params, image) {}

Scenery* Scenery::New(const std::filesystem::path& path) {
	if (path.empty()) {
		return 0;
	}
	try {
		// Imageを読む
		tb::Canvas canvas(path);
		tb::Canvas::Image image(canvas);
		Param param(image);

		// Imageに対応するSceneryをnewする
		Scenery* const s(Factory::Create(&param));

		// 取得てきたら入れ替える
		if (s) {
			if (instance) {
				delete instance;
			}
			instance = s;
			syslog(LOG_INFO, "new scenery was activated:");
		}
	} catch (const char* m) { syslog(LOG_ERR, "%s", m); } catch (...) {
		syslog(LOG_ERR, "Unknown exception:" __FILE__ "(%d)", __LINE__);
	}
	return 0;
}
