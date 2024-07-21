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
#include <openvr/openvr.h>
#include <syslog.h>
#include <tb/app.h>

#include "gl/scenery.h"
#include <core.h>



struct wODM : tb::App {
	int Main(uint, const char**) {
		syslog(LOG_CRIT, "stari wODM.");
		Core* const core(Core::New());
		if (core) {
			Scenery::New();
			core->Run();
			delete core;
		} else {
			syslog(LOG_CRIT, "no VRHMD found.");
		}
		return 0;
	};
} waodm;
