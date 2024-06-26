.PHONY : clean test RELEASE DEBUG COVERAGE

# -lopenvr_api -lX11 -lGL -lGLX -lGLEW -lcairo -ljpeg -lvulkan -lgdbm
export EXLIBS = -lopenvr_api -lX11 -lGL -lGLX -lGLEW -lcairo -ljpeg

prepare clean test RELEASE DEBUG COVERAGE:
	make -r -j -f subsystems/toolbox/makefile $(MAKECMDGOALS)
