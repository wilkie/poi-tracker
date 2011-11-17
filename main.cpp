// Local
#include "kinect.h"
#include "presenter.h"

int main(int argc, char* argv[]) {
	Presenter* p = new Presenter();

	Kinect* k = new Kinect();

	if (!k->initialize()) {
		printf("BAH\n");
		return -1;
	}

	p->run();

	k->cleanup();

	return 0;
}
