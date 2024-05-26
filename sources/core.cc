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
#include "core.h"
#include <assert.h>
#include <tb/time.h>



Core::Layer* Core::Layer::layers(0);

Core::Layer::Layer(Depth d) : next(layers), depth(d) { layers = this; };

void Core::Layer::RegisterAll() {
	for (Layer* l(layers); l; l = l->next) {
		Core::Register(l, l->depth);
	}
}



Core::Layer* Core::layers[Layer::scenary + 1];
void Core::Register(Layer* const layer, Layer::Depth depth) {
	assert(!layers[depth]);
	layers[depth] = layer;
}

void Core::Run() {
	for (Init();;) {
		timestamp.Update();

		const auto& pose(Head());

		for (auto* l : layers) {
			assert(l);
			l->Update(pose);
		}

		while (Eye()) {
			for (auto* l : layers) {
				l->Draw();
			}
			for (unsigned d(Layer::scenary); d <= Layer::scenary; --d) {
				layers[d]->Traw();
			}
		}
	}
}



const tb::Vector<3, float> Core::front({0, 0, 1});
void Core::GetHeadDir(
	tb::Vector<3, float>& headDir, const tb::Matrix<4, 4, float> (&eyes)[2]) {
	headDir = (eyes[0] * front + eyes[1] * front) * 0.5;
}
