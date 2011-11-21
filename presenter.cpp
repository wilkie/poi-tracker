#include "presenter.h"

Presenter::Presenter(Canvas* c) {
	canvas = c;
}

void Presenter::draw() {
}

void Presenter::run() {
	canvas->loop();
}
