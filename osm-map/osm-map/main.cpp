//
//  main.cpp
//  osm-map
//
//  Created by yy on 1/3/15.
//  Copyright (c) 2015 yy. All rights reserved.
//
#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WPainter>
#include <Wt/WText>
#include <Wt/WPaintedWidget>
#include "xml-parser.h"
#include <Wt/WPainterPath>

// c++0x only, for std::bind
// #include <functional>

using namespace Wt;

/*
 * A simple hello world application class which demonstrates how to react
 * to events, read input, and give feed-back.
 */

void draw_image(WPainter &painter, double width, double height, int level) {
	
	for (auto &way : map_way) {
		
		
		bool set_first = false;
		WPainterPath path = WPainterPath(WPointF(0, 0));
		
		for (auto &node : way.second.node_refs) {
			if (!map_node.count(node)) continue;
			
			double x = (map_node[node].lon - minLon) / (maxLon - minLon) * width;
			double y = (maxLat - map_node[node].lat) / (maxLat - minLat) * height;
			
			printf("%.f %.f\n", x, y);
			
			if (!set_first) {
				path.moveTo(WPointF(x, y));
				set_first = true;
			} else {
				path.lineTo(WPointF(x, y));
			}
			
		}
		
		painter.drawPath(path);
	}
	//painter.end();
}

double my_height, my_width;

class MyPaintedWidget : public WPaintedWidget {
public:
	MyPaintedWidget(WContainerWidget *parent = 0) : WPaintedWidget(parent) {
		resize(my_width, my_height);
	}
protected:
	void paintEvent(Wt::WPaintDevice *paintDevice) {
		WPainter painter(paintDevice);
		painter.drawLine(0, 0, 100, 100);
		draw_image(painter, my_width, my_height, level);
	}
private :
	int level;
};

class MapApplicatoin : public WApplication {
public:
	MapApplicatoin(const WEnvironment& env);
	
};

MapApplicatoin::MapApplicatoin(const WEnvironment& env) : WApplication(env) {
	
	setTitle("Osm Map");
	
	root()->addWidget(new WText("Your name, please ? "));
	WContainerWidget *containter = new WContainerWidget(root());
	
	WPaintedWidget *painting = new MyPaintedWidget(containter);
	
	
}


/*
 * The env argument contains information about the new session, and
 * the initial request. It must be passed to the WApplication
 * constructor so it is typically also an argument for your custom
 * application constructor.
 */


WApplication *createApplication(const WEnvironment& env)
{
	/*
	 * You could read information from the environment to decide whether
	 * the user has permission to start a new application
	 */
	return new MapApplicatoin(env);
}

int main(int argc, char **argv)
{
	/*
	 * Your main method may set up some shared resources, but should then
	 * start the server application (FastCGI or httpd) that starts listening
	 * for requests, and handles all of the application life cycles.
	 *
	 * The last argument to WRun specifies the function that will instantiate
	 * new application objects. That function is executed when a new user surfs
	 * to the Wt application, and after the library has negotiated browser
	 * support. The function should return a newly instantiated application
	 * object.
	 */
	parse_file("./Resources/map2.osm");
	my_height = 900.0;
	my_width = (maxLon - minLon) / (maxLat - minLat) * my_height;
	
	for (auto &way : map_way) {
		for (auto &node : way.second.node_refs) printf("%ld ", node);
		puts("");
	}
	
	return WRun(argc, argv, &createApplication);
}
