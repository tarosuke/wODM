/*****************************************************************************
 * Copyright (C) 2025 tarosuke<webmaster@tarosuke.net>
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

#include "base.h"



namespace widget {

	/***** 大きさを持つ
	 * 中身は持たない
	 * 奥行制御しないのはここまで
	 */
	struct Rect : Base {
		// 移動、リサイズ
		void Move(const P&);   // 引数を差分として移動
		void MoveTo(const P&); // 引数位置へ移動
		void Jump(const P&);   // 引数を差分として即時移動
		void JumpTo(const P&); // 引数位置へ即時移動
		void ReSize(const S&); // リサイズ(即時)
		void ReDepth(float depth, float thick) override; // 奥行だけ設定

		Notify Update(const tb::Timestamp&) override;

	protected:
		static tb::Prefs<float> movingRatio;

		// Rで持つと加算誤差が複数要素で蓄積するためPとSで持つ
		P target;
		P position;
		S spread;
		R rect; // Drawで計算し、Trawで用いるキャッシュ

		void Draw() override;
		void Draw(const R&) override;
		void Traw() override;
		virtual void DrawContent() {};
		virtual void TrawContent() {};

		Rect(const P& p, const S& s) : target(p), position(p), spread(s) {};

		float thick; // 使える奥行:ReDepthではこれを子要素に分配する
	};



}
