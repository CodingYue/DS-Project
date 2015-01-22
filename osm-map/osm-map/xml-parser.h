//
//  xml-parser.h
//  osm-map
//
//  Created by yy on 1/3/15.
//  Copyright (c) 2015 yy. All rights reserved.
//


#ifndef osm_map_xml_parser_h
#define osm_map_xml_parser_h

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WPainter>
#include <Wt/WText>
#include <Wt/WPaintedWidget>
#include <Wt/WPainterPath>
using namespace std;

class Node {
public:
	
	double lat, lon;
	long idx;
	Node(){}
	Node(double lat, double lon, long idx) : lat(lat), lon(lon), idx(idx) {}
};

class Way {
public:
	int level;
	vector<long> node_refs;
	Wt::WColor color;
	double width;
	string tag_k, tag_v;
	
	Way(){
		color = Wt::WColor(0, 0, 0);
	}
	Way(const vector<long> &__node_refs, const int &__level, const Wt::WColor &__color, const double &__width,
		char* __tag_k, char *__tag_v
		) {
		node_refs = __node_refs;
		level = __level;
		color = __color;
		width = __width;
		tag_k = __tag_k;
		tag_v = __tag_v;
	}
	
};

typedef pair<string, string> pss;

map<long, Node> map_node;
map<long, Way> map_way;
map<pss, int> level_hash;
map<pss, Wt::WColor> color_hash;
map<pss, double> width_hash;

double minLat, minLon, maxLat, maxLon;

const char* way_type[] = {"natural", "waterway", "highway", "leisure", "landuse", "building", "aeroway", "tourism"};

int load_fliter(char *path) {
	xmlDocPtr doc = xmlParseFile(path);
	assert(doc != NULL);
	
	xmlNodePtr root = xmlDocGetRootElement(doc);
	
	for (xmlNodePtr child = root->children; child; child = child->next) {
		if (xmlStrcasecmp(child->name, BAD_CAST"road") == 0) {
			int level = atof((char*) xmlGetProp(child, BAD_CAST"level"));
			
			for (xmlNodePtr tag = child->children; tag; tag = tag->next) {
				if (xmlStrcasecmp(tag->name, BAD_CAST"tag") != 0) continue;
				char *tag_k = (char *) xmlGetProp(tag, BAD_CAST"k");
				char *tag_v = (char *) xmlGetProp(tag, BAD_CAST"v");
			//	printf("%s %s\n", tag_k, tag_v);
				level_hash[make_pair(tag_k, tag_v)] = level;
				
				
				char *color_str = (char *) xmlGetProp(tag, BAD_CAST"rgb");
				char *width_str = (char *) xmlGetProp(tag, BAD_CAST"width");
				
				int r = 0, g = 0, b = 0;
				if (color_str != NULL) sscanf(color_str, "%d %d %d", &r, &g, &b);
				double width = 1.0;
				if (width_str != NULL) width = atof(width_str) / 7.0;
			//	printf("%.10f\n", width);
				color_hash[make_pair(tag_k, tag_v)] = Wt::WColor(r, g, b);
				width_hash[make_pair(tag_k, tag_v)] = width;
			}
			
		}
	}
	
	xmlFreeDoc(doc);
	return 0;
	
}

int parse_file(char *path) {
	
	xmlDocPtr doc = xmlParseFile(path);
	if (doc == NULL) return -1;
	
	xmlNodePtr root = xmlDocGetRootElement(doc);
	
	for (xmlNodePtr child = root->children; child; child = child->next) {
		
		if (xmlStrcasecmp(child->name, BAD_CAST"Node") == 0) {
			
			long name = atol((char*) xmlGetProp(child, BAD_CAST"id"));
			double lon = atof((char*) xmlGetProp(child, BAD_CAST"lon"));
			double lat = atof((char*) xmlGetProp(child, BAD_CAST"lat"));
			
			if (map_node.count(name)) continue;
			map_node[name] = Node(lat, lon, name);
			
		}
		
		if (xmlStrcasecmp(child->name, BAD_CAST"Way") == 0) {
			
			long name = atol((char*) xmlGetProp(child, BAD_CAST"id"));
			vector<long> refs;
			int level = 0;
			double width = 0;
			Wt::WColor color;
			char *tag_k = "", *tag_v = "";
			bool fuck = false;
			for (xmlNodePtr way_ch = child->children; way_ch; way_ch = way_ch->next) {
				if (xmlStrcasecmp(way_ch->name, BAD_CAST"nd") == 0) {
					refs.push_back(atol((char*) xmlGetProp(way_ch, BAD_CAST"ref")));
				}
				if (xmlStrcasecmp(way_ch->name, BAD_CAST"tag") == 0) {

					for (int i = 0; i < 8; ++i) {
						tag_k = (char *) xmlGetProp(way_ch, BAD_CAST"k");
						if (strcmp(tag_k, way_type[i]) != 0) continue;

						if (i < 5) {
							tag_v = (char* ) xmlGetProp(way_ch, BAD_CAST"v");
						}
						if (!level_hash.count(make_pair(tag_k, tag_v))) {
							continue;
						}
						fuck = true;
						level = level_hash[make_pair(tag_k, tag_v)];
						color = color_hash[make_pair(tag_k, tag_v)];
						width = width_hash[make_pair(tag_k, tag_v)];
					}
				}
			}
		//	printf("%.10f\n", width);
			if (!fuck) continue;
			map_way[name] = Way(refs, level, color, width, tag_k, tag_v);
			
		}
		
		if (xmlStrcasecmp(child->name, BAD_CAST"bounds") == 0) {
			
			minLat = atof((char*) xmlGetProp(child, BAD_CAST"minlat"));
			maxLat = atof((char*) xmlGetProp(child, BAD_CAST"maxlat"));
			minLon = atof((char*) xmlGetProp(child, BAD_CAST"minlon"));
			maxLon = atof((char*) xmlGetProp(child, BAD_CAST"maxlon"));
			
		}
		
	}
	xmlFreeDoc(doc);
	return 0;
}


#endif
