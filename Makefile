poi-tracker: main.cpp presenter.cpp kinect.cpp canvas.cpp
	g++ -o poi-tracker $+ -I /usr/include/ni -I /usr/include/nite -lOpenNI -lXnVNite_1_4_2 -lglut -lGL
