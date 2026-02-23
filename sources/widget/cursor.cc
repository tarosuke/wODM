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
#include "gl/gl.h"
#include "widget/prefs.h"
#include <algorithm>
#include <cmath>
#include <numbers>
#include <string.h>



namespace widget {

	tb::List<Cursor::Set> Cursor::sets;
	Frame* Cursor::on;
	tb::geometry::Vector<2, float> Cursor::bp;

	void Cursor::Draw(const tb::geometry::Vector<2, float>& p, State s) {
		if (Set* const set = sets.Top()) {
			glPushMatrix();
			glTranslatef(-p[0], -p[1], 0.0f);
			set->Draw(s);
			glPopMatrix();
		}
	}


	/***** カーソル画像セット
	 *
	 */
	unsigned Cursor::Set::State::frame;
	Cursor::Set::Progress* Cursor::Set::progress(0);

	Cursor::Set::Animation::Animation(const tb::Image& image) :
		texture(image),
		nFrame(image.Width() / image.Height()),
		uWidth(1.0f / nFrame),
		hSize(image.Height() * 0.5f) {}

	void Cursor::Set::Animation::Draw(unsigned frame) {
		frame %= nFrame;

		const float uL(uWidth * frame);
		const float uR(uWidth * (frame + 1));
		GL::Texture::Binder b(texture);
		glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(-hSize, -hSize);
		glTexCoord2f(-uL, 1.0f);
		glVertex2f(-hSize, hSize);
		glTexCoord2f(uR, 0.0f);
		glVertex2f(hSize, -hSize);
		glTexCoord2f(uR, 1.0f);
		glVertex2f(hSize, hSize);
		glEnd();
	}



	Cursor::Set* Cursor::Set::New() {
		const tb::Color::Format& format(
			tb::Color::Format::Select(tb::Color::Format::ARGB8888));
		tb::BufferedImage images[nState]{
			{format, 32, 32},
			{format, 32, 32},
			{format, 32, 32},
		};
		const tb::Color white(0x7fffffff);
		const tb::Color transparentWhite(0x59ffffff);
		const tb::Color black(0xff000000);

		for (unsigned n(0); n < nState; ++n) {
			tb::Image& image(images[n]);
			const Set::Def& def(cursorDefs[n]);

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
						image.Set(xx - 1, yy, white);
						image.Set(xx + 1, yy, white);
						image.Set(xx, yy - 1, white);
						image.Set(xx, yy + 1, white);
						// imageのx,yの斜めを不透過度0.35で白に塗る
						Shadow(image, xx - 1, yy - 1, transparentWhite);
						Shadow(image, xx + 1, yy - 1, transparentWhite);
						Shadow(image, xx - 1, yy + 1, transparentWhite);
						Shadow(image, xx + 1, yy + 1, transparentWhite);
					}
				}
			}
			// 本体の描画
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

		// 確保して登録
		auto* const s(new Set(images));
		sets.Insert(*s);
		return s;
	}

	/***** imageの画素を不当門の高い方の色にする
	 *
	 */
	void Cursor::Set::Shadow(
		tb::Image& image, unsigned x, unsigned y, const tb::Color& color) {
		const tb::Color oc(image.Get(x, y));
		image.Set(x, y, color.A() < oc.A() ? oc : color);
	}

	/***** 埋め込みカーソルセットの元データ
	 * 構築子にてカーソル画像を生成してCursorに渡す
	 */
	const Cursor::Set::Def Cursor::Set::cursorDefs[Cursor::nState] = {
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


	/***** 進捗リング
	 *
	 */
	tb::Prefs<unsigned> Cursor::Set::Progress::size(
		"widget/progress/size", 32, "進捗リングの外径");
	tb::Prefs<unsigned> Cursor::Set::Progress::color(
		"widget/progress/color", 0xff0000ff, "進捗リングの色");
	tb::Prefs<unsigned> Cursor::Set::Progress::guideColor(
		"widget/progress/guideColor", 0xffffffff, "進捗ガイドリングの色");
	tb::Prefs<unsigned> Cursor::Set::Progress::numColor(
		"widget/progress/numColor", 0xffffffff, "数値の色");
	tb::Prefs<unsigned> Cursor::Set::Progress::dump(
		"widget/progress/dump", 5, "進捗率の緩衝度合い");


	// x,yがin,outの間にあったらc色の点を打つ
	void Cursor::Set::Progress::Circle(tb::Image& image,
		unsigned x,
		unsigned y,
		float in,
		float out,
		const tb::Color& c) {
		const float xx(x - 15.5f);
		const float yy(y - 15.5f);
		const float r(xx * xx + yy * yy);
		if (in * in <= r && r <= out * out) {
			image.Set(x, y, c);
		}
	}
	// x,yから文字を描く
	void Cursor::Set::Progress::Char(tb::Image& image,
		unsigned frame,
		unsigned x,
		unsigned y,
		const unsigned char (&gryph)[5]) {
		const tb::Color c(numColor);
		const unsigned o(image.Height() * frame);
		for (unsigned yy(0); yy < 5; ++yy) {
			// bit 3,2,1,0を描画
			unsigned mask(4);
			for (unsigned xx(0); xx < 3; ++x, mask >>= 1) {
				if (gryph[yy] & mask) {
					image.Set(o + x + xx, y + yy, c);
				}
			}
		}
	}
	Cursor::Set::Progress* Cursor::Set::Progress::New() {
		// 百コマ分の画像
		tb::BufferedImage image(
			tb::Color::Format::Select(tb::Color::Format::ARGB8888), 3200, 32);

		for (unsigned n(0); n < 100; ++n) {
			for (unsigned y(0); y < 32; ++y) {
				const float yy(-15.5f + y);
				for (unsigned x(0); x < 32; ++x) {
					const float xx(-15.5f + x);
					tb::Color c(0);

					// 進捗(中心15.5、半径12-15)
					if ((50.0f + 100.0f * std::atan2(xx, -yy) /
									 std::numbers::pi) <= progress) {
						// 点が進捗の上なら点を打つ
						Circle(image, xx, yy, 12, 15, tb::Color(color));
					}

					// ゲージの円(中心15.5、半径15)
					Circle(image, xx, yy, 15, 15, tb::Color(guideColor));
				}
			}
			// 数値
			unsigned nn(n);
			Char(image, n, 20, 18, gryph[nn % 10]);
			nn /= 10;
			Char(image, n, 16, 18, gryph[nn % 10]);
			nn /= 10;
			Char(image, n, 12, 18, gryph[12]);
			Char(image, n, 8, 18, gryph[nn % 10]);
		}


		return new Cursor::Set::Progress(image);
	}


	void Cursor::Set::Progress::Draw() {
		if (::Progress::IsActive()) {
			// あるいはCursor::Progressもその後を追ってactiveかどうかを判定する
			target = ::Progress::Get();
			Animation::Draw(progress);
		}

		progress += (target - progress) / dump;
	}
	const unsigned char Cursor::Set::Progress::gryph[][5] = {
		{2, 5, 5, 5, 2}, // '0'
		{2, 2, 2, 2, 2}, // '1'
		{6, 1, 2, 4, 7}, // '2'
		{6, 1, 6, 1, 6}, // '3'
		{5, 5, 7, 1, 1}, // '4'
		{7, 4, 6, 1, 6}, // '5'
		{4, 4, 7, 5, 2}, // '6'
		{7, 1, 1, 1, 1}, // '7'
		{2, 5, 2, 5, 2}, // '8'
		{2, 5, 3, 1, 1}, // '9'
		{0, 0, 0, 0, 0}, // ' '
		{5, 1, 2, 4, 5}, // '%'
		{0, 0, 0, 0, 2}, // '.'
	};



}
