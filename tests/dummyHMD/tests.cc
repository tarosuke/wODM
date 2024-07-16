#include <tb/test.h>
#include <unistd.h>


int main() {
	assert(0 <= execl("DEBUG/wODM", "wODM", "--GLDummyHMD=true"));
	return -1;
}
