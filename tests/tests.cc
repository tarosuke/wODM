#include "core.h"
#include "gl/scenery.h"
#include "widget/login.h"
#include <tb/app.h>
#include <tb/test.h>
#include <unistd.h>

#undef Complex
#include "dummy.h"



static void MkLogin() { /*new widget::Login*/ ; }

static struct Test : tb::App {
	int Main(uint, const char**) {
		DummyHMD core;
		GL::Scenery::New();
		core.Run(MkLogin);

		return 0;
	};
} app;
