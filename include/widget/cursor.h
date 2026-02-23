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
#pragma once

#include "frame.h"
#include "progress.h"
#include <gl/texture.h>
#include <tb/geometry/vector.h>
#include <tb/image.h>
#include <tb/list.h>
#include <tb/prefs.h>



namespace widget {

	struct Cursor {
		enum class State {
			normal,		  // 通常
			outOfService, // 操作不可
			text,		  // 文字位置指定
			hidden, // 不可視(NOTE:Stateの数を兼ねているので最後にすること)
		};
		static constexpr unsigned nState = (unsigned)State::hidden;

		/***** カーソルセット
		 * 生成されるとカーソルセットスタックに積まれる
		 * 一番上のカーソルセットが有効
		 * imagesで示される画像カーソルセットを生成
		 * imagesは縦がカーソル画像の大きさ、横が各フレームに対応する
		 * 4フレーム構成なら「■■■■」のようになる
		 * カーソル画像の中心がカーソル位置に対応する
		 * 引数なしのコンストラクタはデフォルトのカーソルセット
		 */
		struct Set : tb::List<Set>::Node {
			static Set* New(); // デフォルトカーソル画像を使う
			template <class I> Set* New(const I (&images)[nState]) {
				auto* const s(new Set{images});
				sets.Insert(*s);
				return s;
			}
			void Draw(Cursor::State s) { states[(unsigned)s].Draw(); };

		private:
			template <class I>
			Set(const I (&images)[nState]) :
				states{images[0], images[1], images[2]} {}
			struct Animation {
				Animation(const tb::Image&);
				void Draw(unsigned frame);

			protected:
				GL::Texture texture;
				const unsigned nFrame; // アニメーションフレーム数
				const float uWidth;	   // フレームあたりのU座標の幅
				const float hSize;	   // カーソルサイズの半分
			};
			struct State : Animation {
				State(const tb::Image& image) : Animation(image) {};
				void Draw() { Animation::Draw(frame++); };

			private:
				static unsigned frame;
			} states[nState];
			static const struct Def {
				unsigned x;
				unsigned y;
				unsigned image[16];
			} cursorDefs[nState];
			static struct Progress : Animation {
				Progress* New();
				void Draw();

			private:
				static tb::Prefs<unsigned> size;
				static tb::Prefs<unsigned> color;
				static tb::Prefs<unsigned> guideColor;
				static tb::Prefs<unsigned> numColor;
				static tb::Prefs<unsigned> dump;
				struct ProgressParam {
					float radious;
					unsigned progress; // 0-100
				};
				static const unsigned char gryph[][5];
				float target;	// 真値
				float progress; // 表示地
				Progress(const tb::Image& image) : Animation(image) {};
				void Circle(tb::Image&,
					unsigned x,
					unsigned y,
					float in,
					float out,
					const tb::Color&);
				void Char(tb::Image&,
					unsigned frame,
					unsigned x,
					unsigned y,
					const unsigned char (&gryph)[5]);
			}* progress;

			// Imageの画素を透明度の低い方の色にする
			static void Shadow(
				tb::Image&, unsigned x, unsigned y, const tb::Color&);
		};


		/***** カーソル描画
		 * カーソルが上にあるFrame::Trawから呼ばれる(なければRoot::TrawAll)
		 * 座標はFrameローカル
		 *  - 通常はイベントそのままだがtextのときに文字境界にもできる
		 * Stateの管理はFramme側
		 */
		static void Draw(const tb::geometry::Vector<2, float>&, State);
		static void Draw(State s) { Draw(bp, s); };

	private:
		static tb::List<Set> sets;				  // カーソルセットのスタック
		static Frame* on;						  // カーソルが乗っているFrame
		static tb::geometry::Vector<2, float> bp; // 基準面上のカーソル位置
	};
}
