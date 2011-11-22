// Local
#include "kinect.h"
#include "window.h"

int main(int argc, char* argv[]) {
	Kinect* k = new Kinect();

	if (!k->initialize()) {
		printf("There were errors initializing the Kinect device.\n");
		return -1;
	}

	Task* t = new Task(k);

	Window* w = new Window(k->width(), k->height(), t);
	Presenter* p = w->windowPresenter();

	p->run();

	k->cleanup();

	return 0;
}
