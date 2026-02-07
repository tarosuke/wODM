/** Progress
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
#pragma once

#include <tb/list.h>



struct Progress : tb::List<Progress>::Node {
	static constexpr int completed = 100; // この値を超えている時は無効

	Progress();
	~Progress();
	void Set(int);

	static int Get() { return total; };
	static bool IsActive(int v) { return v <= completed; };
	static bool IsActive() { return IsActive(total); };

private:
	static tb::List<Progress> list;
	static int already; // 既に終了した分の合計
	static int total;

	int progress;

	static void Update();
	static void Deactivate() { already = total = completed + 1; };
};
