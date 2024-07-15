#include <unistd.h>


int main() {
	execl("DEBUG/wODM", "wODM", "--GLDummyHMD=true");
	return -1;
}
