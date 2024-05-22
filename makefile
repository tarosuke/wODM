.PHONY : clean test RELEASE DEBUG COVERAGE

prepare clean test RELEASE DEBUG COVERAGE:
	make -r -j -f subsystems/toolbox/makefile $(MAKECMDGOALS)
