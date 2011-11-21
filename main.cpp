// Local
#include "kinect.h"
#include "window.h"

int main(int argc, char* argv[]) {
	Window* w = new Window();
	Presenter* p = w->windowPresenter();

	Kinect* k = new Kinect();

	if (!k->initialize()) {
		printf("There were errors initializing the Kinect device.\n");
		return -1;
	}

	p->run();

	k->cleanup();

	return 0;
}
