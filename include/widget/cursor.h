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

#include "progress.h"
#include <gl/texture.h>
#include <tb/geometry/vector.h>
#include <tb/image.h>
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

		/***** カーソルセット設定
		 * imagesで示される画像カーソルセットを生成
		 * imagesは縦がカーソル画像の大きさ、横が各フレームに対応する
		 * 4フレーム構成なら「■■■■」のようになる
		 * カーソル画像の中心がカーソル位置に対応する
		 */
		Cursor(const tb::Image**);

		/***** カーソル描画
		 * カーソルが上にあるFrame::Trawから呼ばれる(なければRoot::TrawAll)
		 * 座標はFrameローカル
		 *  - 通常はイベントそのままだがtextのときに文字境界にもできる
		 * Stateの管理はFramme側
		 */
		void Draw(const tb::geometry::Vector<2, float>&, State);


	private:
		/****** カーソル画像
		 * カーソルそれ自体や進捗リングの描画
		 */
		struct Images {
			Images(const tb::Image&);
			void Draw(unsigned frame);

		protected:
			const unsigned nFrame; // フレーム数
		private:
			GL::Texture texture;
			const float uWidth; // フレームあたりのU座標の幅
		};


		struct Animation : Images {
			Animation(const tb::Image& image) : Images(image), frame(0) {};
			void Draw() {
				frame %= nFrame;
				Images::Draw(frame++);
			};

		private:
			unsigned frame;
		}* animations[nState];



		/***** 進捗リング
		 * 進捗値自体は::Progressで管理されるが表示の補間はこのクラスで
		 */
		static struct Progress : Images {
			Progress();

			static void Update();
			static bool IsActive() { return ::Progress::IsActive(value); };

		private:
			static const char gryph[][5];
			static constexpr int dump = 5;
			static int target;
			static int value;
		} progress;
	};
}
