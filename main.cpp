// Local
#include "kinect.h"
#include "window.h"
#include "player.h"

int main(int argc, char* argv[]) {
	Kinect* k = new Kinect();

	char* xml_path = "poi-tracker.xml";
	char* play_path = NULL;
	if (argc > 1) {
		if (strlen(argv[1]) > 4) {
			if (strcmp(&argv[1][strlen(argv[1])-4], ".poi") == 0) {
				play_path = argv[1];
			}
		}
		xml_path = argv[1];
	}

	Window* w = NULL;
	if (play_path != NULL) {
		Player* player = new Player(play_path);
		w = new Window(player->width(), player->height(), player);
	}
	else if (k->initialize(xml_path)) {
		w = new Window(k->width(), k->height(), k);
	}
	else {
		printf("There were errors initializing the Kinect device.\n");
		return -1;
	}

	Presenter* p = w->windowPresenter();

	p->run();

	k->cleanup();

	return 0;
}
