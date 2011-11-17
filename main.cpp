// Local
#include "kinect.h"
#include "presenter.h"

int main(int argc, char* argv[]) {
	Kinect* k = new Kinect();

	if (!k->initialize()) {
		return -1;
	}

	Presenter* p = new Presenter();

	k->cleanup();

	return 0;
}
