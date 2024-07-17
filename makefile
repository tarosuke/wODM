.PHONY : clean test RELEASE DEBUG COVERAGE FULLTEST

# -lopenvr_api -lX11 -lGL -lGLX -lGLEW -lcairo -ljpeg -lvulkan -lgdbm
export EXLIBS = -lopenvr_api -lX11 -lGL -lGLX -lGLEW -lcairo -ljpeg

FULLTEST:
	make -r -j -f subsystems/toolbox/makefile DEBUG
	make -r -j -f subsystems/toolbox/makefile $(MAKECMDGOALS)

clean test RELEASE DEBUG COVERAGE:
	make -r -j -f subsystems/toolbox/makefile $(MAKECMDGOALS)
