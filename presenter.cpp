#include "presenter.h"

Presenter::Presenter() {
	canvas = new Canvas();
}

void Presenter::draw() {
}

void Presenter::run() {
	canvas->loop();
}
