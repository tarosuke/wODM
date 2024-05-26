/*****************************************************************************
 * Copyright (C) 2024 tarosuke<webmaster@tarosuke.net>
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

#include <tb/factory.h>
#include <tb/list.h>
#include <tb/matrix.h>



struct Core {
	struct F : tb::Factory<Core> {
		tb::uint Score() override;
		Core* New() override;
	};

	struct Layer {
		Layer() = delete;
		Layer(const Layer*) = delete;
		void operator=(const Layer*) = delete;

		enum Depth { sticky, widget, world, scenary }; // Layerの奥行き
		Layer(Depth depth);

		virtual void Register() = 0;
		virtual void Update(const tb::Vector<3, float>&) {};
		virtual void Draw() {};
		virtual void Traw() {};

		static void RegisterAll();

	private:
		static Layer* layers;
		Layer* next;
		const Depth depth;
	};



	virtual ~Core(){};

	void Run();
	const tb::Timestamp& Timestamp() { return timestamp; };

	/***** そのVRHMD固有の初期化
	 * 最初に一度だけ呼ばれる
	 */
	virtual void Init() = 0;

	/***** 状況単位の初期化
	 * 頭の向きベクトルへの参照を返すこと(次に呼ぶまで有効なこと)
	 */
	virtual const tb::Vector<3, float>& Head() = 0;

	/***** フレームバッファ単位の初期化
	 * 適切なフレームバッファを有効にすること
	 * 必要があれば描画なりOpenVRサーバなりに送ること
	 * すべてのフレームバッファについて完了していたら偽を返すこと
	 */
	virtual bool Eye() = 0;

protected:
	/***** eyes[](0,0,1)から頭の方向を計算する
	 */
	static void GetHeadDir(
		tb::Vector<3, float>& headDir,
		const tb::Matrix<4, 4, float> (&eyes)[2]);
	static const tb::Vector<3, float> front;

private:
	tb::Timestamp timestamp;

	static Layer* layers[Layer::scenary + 1];
	static void Register(Layer* const layer, Layer::Depth deyth);
};
