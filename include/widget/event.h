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

#include <tb/time.h>
#include <tb/vector.h>



namespace widget {

	struct KeyEvent {
		// モディファイアキー状態
		static const unsigned leftShift = 0x01;
		static const unsigned rightShift = 0x02;
		static const unsigned shiftMask = 0x03;
		static const unsigned leftCtrl = 0x04;
		static const unsigned rightCtrl = 0x08;
		static const unsigned ctrlMask = 0x0c;
		static const unsigned leftAlt = 0x10;
		static const unsigned rightAlt = 0x20;
		static const unsigned altMask = 0x30;
		static const unsigned caps = 0x40;
	};
	struct PtEvent {
		using P = tb::Vector<3, float>;
		using T = tb::Timestamp;
		using P2 = tb::Vector<2, float>;

		/***** インスタンス生成
		 * ①イベント発生時のインスタンス生成
		 * ②originを元にpositionでローカル座標に変換したインスタンスを生成
		 * どちらも対象面の奥行きをz = 0とする
		 */
		PtEvent(const P& position, unsigned button, unsigned modifiers); // ①
		PtEvent(const PtEvent& origin, const P& position);				 // ②

		// 座標系に沿った平面との媒介変数値、交点を求める(0 <= d <= 2)
		float Tee(unsigned d, float v) const {
			return (origin[d] - v) / dir[d];
		};
		P2 ZCrossPoint(float t) {
			return P2{origin[0] + dir[0] * t, origin[1] + dir[1] * t};
		};


		const T time;

		// 原点と向き
		P origin;
		P dir;

		// この３つはマウスボタンの状態とその変化
		unsigned state;
		unsigned down;
		unsigned up;

		// モディファイアキーの状態(内容はKeyEventにある)
		unsigned modifiers;
	};

	/****** widgetがUpdateしたときの戻り値
	 * 親要素に状態変化を伝える
	 */
	union Notify {
		unsigned raw;
		struct {
			/***** priorityUpdating
			 * 可視状態のPaneがテクスチャをUpdate中
			 * すべての要素の状態の論理和がBase::priorityUpdatingに反映される
			 */
			bool priorityUpdating : 1;

			/***** widthUpdated / heightUpdated / thickUpdated
			 * サイズが変更された
			 * これを受け取ったら自身の幅を変更して必要であれば再配置
			 * 軸別なのは各軸がそれぞれ別のクラスで扱われるため
			 */
			bool widthUpdated : 1;
			bool heightUpdated : 1;
			bool thickUpdated : 1;
		};
		Notify() : raw(0) {};
		Notify(unsigned r) : raw(r) {};
	};
}
