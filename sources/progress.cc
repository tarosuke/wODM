/** Cursor
 * Copyright (C) 2026 tarosuke<webmaster@tarosuke.net>
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
#include "progress.h"
#include "widget/cursor.h"
#include <algorithm>



tb::List<Progress> Progress::list;


Progress::Progress() : progress(0) {
	if (completed < total) {
		total = already = 0;
	}
}

Progress::~Progress() {
	if (progress == completed) {
		// 自らは消えるので代わりに修了を記録しておく
		already += completed;
	}
	Update();
};



void Progress::Set(int v) {
	// 自身の値を更新して
	progress = std::clamp(v, 0, completed);
	// 再集計
	Update();
}


int Progress::already(completed + 1);
int Progress::total(completed + 1);
void Progress::Update() {
	total = already;
	if (!!list.Top()) {
		for (tb::List<Progress>::I i(list); ++i;) { total += (*i).progress; }
	} else {
		// Progressのインスタンスはいないので無効にする
		Deactivate();
	}
}
