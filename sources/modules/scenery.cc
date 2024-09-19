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
Scenery* Scenery::stack(0);

// コマンドラインオプション--scenery
tb::Prefs<tb::String> Scenery::path("--scenery", "背景指定");


Scenery::Scenery(const Params& params, const tb::Image& image)
	: Model_C(params, image), next(stack) {
	stack = this;
}

Scenery* Scenery::New(const std::filesystem::path* p) {
	const char* pp(0);
	if (p && p->string().size()) {
		// パスが指定されている
		pp = p->string().c_str();
	} else {
		const tb::String& ppp(path);
		if (ppp.size()) {
			// パスが設定されている
			pp = ppp.c_str();
		}
	}

	if (pp) {
		try {
			// Imageを読んでParam型に格納
			tb::Canvas canvas(pp);
			tb::Canvas::Image image(canvas);
			Param param(image);

			// Imageに対応するSceneryをnewする
			UpdateInstance(Factory::Create(&param));
			syslog(LOG_INFO, "Scenery(%s) was activated.", pp);

			return stack;
		} catch (...) {
			syslog(
				LOG_WARNING,
				"Failed to load scenery(%s). Falling back...",
				pp);
		}
	}
	// デフォルトを設定
	UpdateInstance(Factory::Create());
	return stack;
}

void Scenery::UpdateInstance(Scenery* s) {
	if (!s) {
		throw "empty scenery";
	}
	syslog(LOG_INFO, "new scenery was activated:");
}
