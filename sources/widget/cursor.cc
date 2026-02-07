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
#include "widget/cursor.h"
#include <algorithm>
#include <string.h>



namespace widget {



	/***** 進捗リング補間
	 *
	 */
	Cursor::Progress Cursor::progress;
	int Cursor::Progress::target(0);
	int Cursor::Progress::value(0);
	void Cursor::Progress::Update() {
		if (::Progress::IsActive()) {
			target = ::Progress::Get();
			// あるいはCursor::Progressもその後を追ってactiveかどうかを判定する
		}

		const int d(target - value);
		if (0 < d ? d < dump : -dump < d) {
			value = target;
		} else {
			value += d / dump;
		}
	}

	const char Cursor::Progress::gryph[][5] = {
		{7, 5, 5, 5, 7}, // '0'
		{2, 2, 2, 2, 2}, // '1'
		{7, 1, 7, 4, 7}, // '2'
		{7, 1, 7, 1, 7}, // '3'
		{5, 5, 7, 1, 1}, // '4'
		{7, 4, 7, 1, 7}, // '5'
		{6, 4, 7, 5, 7}, // '6'
		{7, 1, 1, 1, 1}, // '7'
		{7, 5, 7, 5, 7}, // '8'
		{7, 5, 7, 1, 3}, // '9'
		{0, 0, 0, 0, 0}, // ' '
		{5, 1, 2, 4, 5}, // '%'
		{0, 0, 0, 0, 2}, // '.'
	};



	/***** デフォルトカーソル
	 * 最小限の画像データが埋め込まれているカーソル
	 * 他に何も設定されなかった場合に使われる
	 */
	struct DefaultCursor : Cursor {
		DefaultCursor() : Cursor(MakeCursorImage()) { CleanCursorImage(); };


	private:
		static const struct Def {
			unsigned x;
			unsigned y;
			unsigned image[16];
		} cursorDefs[Cursor::nState];

		static void Shadow(
			tb::Image& image, unsigned x, unsigned y, unsigned c) {
			const unsigned oc(image.Get(x, y).ARGB32());
			image.Set(x, y, tb::Color(std::max(oc, c)));
		};
		static const tb::Image** MakeCursorImage() {
			for (unsigned n(0); n < nState; ++n) {
				images[n] = new tb::BufferedImage(
					tb::Color::Format::Select(tb::Color::Format::ARGB8888), 32,
					32);
				tb::Image& image(*images[n]);
				const DefaultCursor::Def& def(cursorDefs[n]);

				// 画像のクリア
				memset(image.Data(), 0, image.Width() * image.Height());

				// 影の描画
				for (unsigned y(0); y < 16; ++y) {
					unsigned b(def.image[y]);
					for (unsigned x(0); x < 16; ++x, b <<= 1) {
						if (b & 0x8000) {
							const unsigned xx(x + 16 - def.x);
							const unsigned yy(y + 16 - def.y);
							// imageのx,yの上下左右を不透過度0.5で白に塗る
							Shadow(image, xx - 1, yy + y, 0x7fffffff);
							Shadow(image, xx + 1, yy + y, 0x7fffffff);
							Shadow(image, xx, yy + y - 1, 0x7fffffff);
							Shadow(image, xx, yy + y + 1, 0x7fffffff);
							// imageのx,yの斜めを不透過度0.35で白に塗る
							Shadow(image, xx + x - 1, yy + y - 1, 0x59ffffff);
							Shadow(image, xx + x + 1, yy + y - 1, 0x59ffffff);
							Shadow(image, xx + x - 1, yy + y + 1, 0x59ffffff);
							Shadow(image, xx + x + 1, yy + y + 1, 0x59ffffff);
						}
					}
				}
				// 本体の描画
				const tb::Color black(0xff000000);
				for (unsigned y(0); y < 16; ++y) {
					unsigned b(def.image[y]);
					for (unsigned x(0); x < 16; ++x, b <<= 1) {
						if (b & 0x8000) {
							// 該当画素を黒に塗る
							image.Set(x + 16 - def.x, y + 16 - def.y, black);
						}
					}
				}
			}

			return const_cast<const tb::Image**>(images);
		};
		static void CleanCursorImage() {
			for (unsigned n(0); n < nState; ++n) { delete images[n]; }
		};


		static tb::Image* images[nState]; // 画像置き場
	};


	tb::Image* DefaultCursor::images[nState];


	/***** 埋め込みカーソルセットの元データ
	 * 構築子にてカーソル画像を生成してCursorに渡す
	 */
	const DefaultCursor::Def DefaultCursor::cursorDefs[Cursor::nState] = {
		{.x = 1,
			.y = 1,
			.image = {0x0000, 0x4000, 0x4000, 0x3000, 0x3000, 0x3800, 0x3800,
				0x3c00, 0x3c00, 0x3e00, 0x3e00, 0x3f00, 0x3f00, 0xfc00, 0x0c00,
				0x0000}},
		{.x = 8,
			.y = 8,
			.image = {0x0000, 0x4004, 0x600c, 0x3018, 0x1830, 0x0c60, 0x06c0,
				0x03c0, 0x03c0, 0x06c0, 0x0c60, 0x1830, 0x3018, 0x600c, 0x4004,
				0x0000}},
		{.x = 2,
			.y = 8,
			.image = {0x0000, 0xe000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000,
				0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0xe000,
				0x0000}}};
}
