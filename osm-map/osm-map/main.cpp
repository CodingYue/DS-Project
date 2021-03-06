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
#include <Wt/WSvgImage>
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

typedef tuple<int, int, int> tdd;

double my_height, my_width;
void draw_image(WPainter &painter, double width, double height, int level) {

	painter.fillRect(0, 0, width, height, WColor(247, 245, 240));
	for (auto &way : map_way) {
		
		int way_level = way.second.level;
		if (way_level - 4 > level) continue;
		
		bool set_first = false;
		WPainterPath path = WPainterPath(WPointF(0, 0));
		
		WPointF begin = WPointF(0, 0);
		WPointF end = WPointF(0, 0);
		
		for (auto &node : way.second.node_refs) {
			if (!map_node.count(node)) continue;
			
			double x = (map_node[node].lon - minLon) / (maxLon - minLon) * width;
			double y = (maxLat - map_node[node].lat) / (maxLat - minLat) * height;
			
			//printf("%.f %.f\n", x, y);
			
			if (!set_first) {
				path.moveTo(WPointF(x, y));
				begin = WPointF(x, y);
				set_first = true;
			} else {
				path.lineTo(WPointF(x, y));
			}
			end = WPointF(x, y);
			
		}
		WPen pen;
		pen.setColor(way.second.color);
		double c = 1-level*0.015;
		pen.setWidth(way.second.width*c);
		
	//	printf("%d %d %d\n", way.second.color.red(), way.second.color.green(), way.second.color.blue());
		
		if (way.second.color.red() == 0 && way.second.color.green() == 0 && way.second.color.blue() == 0) {
			cout << way.second.tag_k << " " << way.second.tag_v << endl;
		}
		/*
		if (way.second.color == WColor(0, 0, 0)) {
			cout << way.second.tag_k << " " << way.second.tag_v << endl;
		}*/
		
		//printf("%d %d %d\n", way.second.color.red(), way.second.color.green(), way.second.color.blue());
		
		WBrush brush;
		brush.setStyle(NoBrush);
		
		if (begin == end) {
			brush.setStyle(SolidPattern);
			brush.setColor(way.second.color);
		}
		painter.setBrush(brush);
		painter.setPen(pen);
		painter.drawPath(path);

	}
	//painter.end();
}

Coordinates operator - (const Coordinates&a, const Coordinates&b) {
	return Coordinates(a.x-b.x, a.y-b.y);
}

class MyPaintedWidget : public WPaintedWidget {
public:
	MyPaintedWidget(WContainerWidget *parent = 0, int __level = 1) : WPaintedWidget(parent) {
		resize(my_width, my_height);
		
		mouseWheel().connect(this, &MyPaintedWidget::mWheel);
		mouseWentDown().connect(this, &MyPaintedWidget::mouseDown);
		mouseDragged().connect(this, &MyPaintedWidget::mouseDrag);
		
		level = __level;
		double c = 1+0.15*(level-1);
		width_lv = c * my_width;
		height_lv = c * my_height;
	}
	
protected:
	void paintEvent(Wt::WPaintDevice *paintDevice) {
		WPainter painter(paintDevice);
		//painter.drawLine(0, 0, 100, 100);
		//draw_image(painter, my_width*level, my_height*level, level);
		char str[30];
		sprintf(str, "./Resources/images/level_%02d.svg", level);
		
		
		if (ox > width_lv - my_width) ox = width_lv - my_width;
		if (oy > height_lv - my_height) oy = height_lv - my_height;
		if (ox < 0) ox = 0;
		if (oy < 0) oy = 0;

		
		paintDevice->drawImage(WRectF(0, 0, paintDevice->width().toPixels(), paintDevice->width().toPixels()), str, width_lv+10, height_lv+10, WRectF(ox, oy, my_width, my_height));
	}
private :
	int level;
	int count = 1;
	int delta = 0;
	double width_lv, height_lv;
	double ox = 0, oy = 0;
	Coordinates pre = Coordinates(0, 0);
	
	void mWheel(const WMouseEvent &e) {
		delta = e.wheelDelta();
		
		level += (delta > 0 ? 1 : -1);
		if (level > 20) {
			level = 20;
			return;
		}
		if (level < 1) {
			level = 1;
			return;
		}
		double c = 1+0.15*(level-1);
		width_lv = c * my_width;
		height_lv = c * my_height;
//		printf("%d\n", level);
		update();
		
	}
	
	void moveTo(const Coordinates &d) {
		ox += d.x;
		oy += d.y;
		printf("%d %d\n", d.x, d.y);

		
		
		printf("%.3f %.3f\n", width_lv, height_lv);
		printf("%.3f %.3f\n", ox, oy);
		
		update();
	}
	
	void mouseDrag(const Wt::WMouseEvent& e) {
		Wt::Coordinates c = e.widget();
		
		Coordinates d = (pre - c);
		
		//printf("%d %d\n", d.x, d.y);
		
		moveTo(d);
		
		pre = c;
	} void mouseDown(const Wt::WMouseEvent& e) {
		pre = e.widget();
	}

	
};

class MapApplicatoin : public WApplication {
public:
	MapApplicatoin(const WEnvironment& env);
	
};

MapApplicatoin::MapApplicatoin(const WEnvironment& env) : WApplication(env) {
	
	setTitle("Osm Map");
	
	root()->addWidget(new WText("Your name, please ? "));
	WContainerWidget *containter = new WContainerWidget(root());
	
	
	WPaintedWidget *painting = new MyPaintedWidget(containter, 1);
	
	
	
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
	
	
	load_fliter("./Resources/level_fliter.xml");
	parse_file("./Resources/map2.osm");
	my_height = 900.0;
	my_width = (maxLon - minLon) / (maxLat - minLat) * my_height;
	/*
	for (auto &way : map_way) {
		for (auto &node : way.second.node_refs) {
			if (!map_node.count(node)) continue;
			printf("%f %f\n", map_node[node].lon, map_node[node].lat);
		}
		puts("");
	}
	*/
	
	for (int level = 1; level <= 20; ++level) {
		
		
		double c = 1+0.15*(level-1);
		WSvgImage svgImage(c*my_width+10, c*my_height+10);
		WPainter painter(&svgImage);
		draw_image(painter, c*my_width, c*my_height, level);
	//	painter.end();
		
		char str[30];
		sprintf(str, "./Resources/images/level_%02d.svg", level);
		
		ofstream f(str);
		svgImage.write(f);
	}
	
	return WRun(argc, argv, &createApplication);
}
