// Local
#include "kinect.h"
#include "window.h"

int main(int argc, char* argv[]) {
	Kinect* k = new Kinect();

	char* xml_path = "poi-tracker.xml";
	if (argc > 1) {
		xml_path = argv[1];
	}

	if (!k->initialize(xml_path)) {
		printf("There were errors initializing the Kinect device.\n");
		return -1;
	}

	Window* w = new Window(k->width(), k->height(), k);
	Presenter* p = w->windowPresenter();

	p->run();

	k->cleanup();

	return 0;
}
