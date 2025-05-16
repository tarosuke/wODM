/** Skybox
 * Copyright (C) 2017,2019,2025 tarosuke<webmaster@tarosuke.net>
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

#include <assert.h>
#include <stdlib.h>
#include <syslog.h>

#include "gl/gl.h"
#include "gl/scenery.h"
#include "gl/texture.h"

#define elementsOf(a) (sizeof(a) / sizeof(a[0]))



namespace GL {

	class Skybox : public Scenery {

		Skybox(
			const Params& params,
			const tb::Image& image,
			const GL::Texture::Style& style) :
			Scenery(params, image, style) {};

		static constexpr int scale = 5000;
		struct F : Scenery::Factory {
			// 画像が指定された場合
			Scenery* New(const tb::Image&) final;
			uint Score(const tb::Image&) final;
		};
		struct NF : Scenery::NullFactory {
			// 画像指定なしの場合
			Scenery* New() final;
			uint Score() final;
		};
		static F factory;
		static NF nullFactory;
		static unsigned indexes[];
		static GL::VBO::V_UV vertexes[];
		static constexpr int defaultTextureScale = 6;
		static GL::VBO::V_UV defaultVertexes[];
		static const GL::Texture::Style defaultTextureStyle;
		static const tb::u32 defaultTexture[8][8];
		static constexpr tb::u32 bc = 0x00101020;
		static constexpr tb::u32 fc = 0x00404060;
	};

	Skybox::F Skybox::factory;
	Skybox::NF Skybox::nullFactory;

	unsigned Skybox::indexes[] = {
		0,	2,	1, 2,  3,  1, // left
		2,	4,	3, 4,  5,  3, // front
		4,	6,	5, 6,  7,  5, // right
		6,	8,	7, 8,  9,  7, // rear
		2,	10, 4, 10, 11, 4, // top
		12, 13, 9, 13, 7,  9  // bottom
	};

	GL::VBO::V_UV Skybox::vertexes[] = {
		// 0:左後方
		{{-scale, scale, scale}, {0.0 / 4, 1.0 / 3}},
		{{-scale, -scale, scale}, {0.0 / 4, 2.0 / 3}},
		// 2:左前方
		{{-scale, scale, -scale}, {1.0 / 4, 1.0 / 3}},
		{{-scale, -scale, -scale}, {1.0 / 4, 2.0 / 3}},
		// 4:右前方
		{{scale, scale, -scale}, {2.0 / 4, 1.0 / 3}},
		{{scale, -scale, -scale}, {2.0 / 4, 2.0 / 3}},
		// 6:右後方
		{{scale, scale, scale}, {3.0 / 4, 1.0 / 3}},
		{{scale, -scale, scale}, {3.0 / 4, 2.0 / 3}},
		// 8:左後方(後方テクスチャ用)
		{{-scale, scale, scale}, {4.0 / 4, 1.0 / 3}},
		{{-scale, -scale, scale}, {4.0 / 4, 2.0 / 3}},
		// 10:上後方左右
		{{-scale, scale, scale}, {1.0 / 4, 0.0 / 3}},
		{{scale, scale, scale}, {2.0 / 4, 0.0 / 3}},
		// 12:下後方左右
		{{-scale, -scale, -scale}, {4.0 / 4, 3.0 / 3}},
		{{scale, -scale, -scale}, {3.0 / 4, 3.0 / 3}},
	};
	GL::VBO::V_UV Skybox::defaultVertexes[] = {
		// 0:左後方
		{{-scale, scale, scale},
		 {0.0 * defaultTextureScale, 1.0 * defaultTextureScale}},
		{{-scale, -scale, scale},
		 {0.0 * defaultTextureScale, 2.0 * defaultTextureScale}},
		// 2:左前方
		{{-scale, scale, -scale},
		 {1.0 * defaultTextureScale, 1.0 * defaultTextureScale}},
		{{-scale, -scale, -scale},
		 {1.0 * defaultTextureScale, 2.0 * defaultTextureScale}},
		// 4:右前方
		{{scale, scale, -scale},
		 {2.0 * defaultTextureScale, 1.0 * defaultTextureScale}},
		{{scale, -scale, -scale},
		 {2.0 * defaultTextureScale, 2.0 * defaultTextureScale}},
		// 6:右後方
		{{scale, scale, scale},
		 {3.0 * defaultTextureScale, 1.0 * defaultTextureScale}},
		{{scale, -scale, scale},
		 {3.0 * defaultTextureScale, 2.0 * defaultTextureScale}},
		// 8:左後方(後方テクスチャ用)
		{{-scale, scale, scale},
		 {4.0 * defaultTextureScale, 1.0 * defaultTextureScale}},
		{{-scale, -scale, scale},
		 {4.0 * defaultTextureScale, 2.0 * defaultTextureScale}},
		// 10:上後方左右
		{{-scale, scale, scale},
		 {1.0 * defaultTextureScale, 0.0 * defaultTextureScale}},
		{{scale, scale, scale},
		 {2.0 * defaultTextureScale, 0.0 * defaultTextureScale}},
		// 12:下後方左右
		{{-scale, -scale, -scale},
		 {4.0 * defaultTextureScale, 3.0 * defaultTextureScale}},
		{{scale, -scale, -scale},
		 {3.0 * defaultTextureScale, 3.0 * defaultTextureScale}},
	};

	// デフォルト背景
	uint Skybox::NF::Score() { return Certitude::passiveMatch; };
	Scenery* Skybox::NF::New() {
		const Params params = {
			numOfIndex : elementsOf(Skybox::indexes),
			index : Skybox::indexes,
			numOfVertex : elementsOf(Skybox::defaultVertexes),
			vertex : Skybox::defaultVertexes
		};
		const tb::Image image(
			(void*)defaultTexture,
			tb::Color::Format::Select(tb::Color::Format::XRGB0888), 8, 8, 32);
		return new Skybox(params, image, defaultTextureStyle);
	};

	// 指定画像を背景にする
	uint Skybox::F::Score(const tb::Image& i) {
		// skyboxなら画像は4:3のはず
		return abs((int)i.Height() * 4 - (int)i.Width() * 3) < 4
				   ? Certitude::activeMatch
				   : 0;
	}
	Scenery* Skybox::F::New(const tb::Image& i) {
		const Params params = {
			numOfIndex : elementsOf(Skybox::indexes),
			index : Skybox::indexes,
			numOfVertex : elementsOf(Skybox::vertexes),
			vertex : Skybox::vertexes
		};
		return new Skybox(params, i, GL::Texture::defaultStyle);
	}

	/***** デフォルトのテクスチャ
	 */
	const GL::Texture::Style Skybox::defaultTextureStyle = {
		wrap_s : GL_MIRRORED_REPEAT,
		wrap_t : GL_MIRRORED_REPEAT,
		filter_mag : GL_LINEAR,
		filter_min : GL_LINEAR,
		texture_mode : GL_REPLACE,
		pointSprite : false,
	};
	const tb::u32 Skybox::defaultTexture[8][8] = {
		{bc, bc, bc, bc, bc, bc, bc, fc}, {bc, bc, bc, bc, bc, bc, bc, fc},
		{bc, bc, bc, bc, bc, bc, bc, fc}, {bc, bc, bc, bc, bc, bc, bc, fc},
		{bc, bc, bc, bc, bc, bc, bc, fc}, {bc, bc, bc, bc, bc, bc, bc, fc},
		{bc, bc, bc, bc, bc, bc, bc, fc}, {fc, fc, fc, fc, fc, fc, fc, fc},
	};

}
