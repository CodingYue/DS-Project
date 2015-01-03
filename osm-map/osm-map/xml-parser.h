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
#include <vector>
#include <map>
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
	vector<long> node_refs;
	string tag;
	
	Way(){}
	Way(const vector<long> &__node_refs, const string &__tag) {
		node_refs = __node_refs;
		tag = __tag;
	}
	
};

map<long, Node> map_node;
map<long, Way> map_way;

double minLat, minLon, maxLat, maxLon;

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
			string tag;
			
			for (xmlNodePtr way_ch = child->children; way_ch; way_ch = way_ch->next) {
				if (xmlStrcasecmp(way_ch->name, BAD_CAST"nd") == 0) {
					refs.push_back(atol((char*) xmlGetProp(way_ch, BAD_CAST"ref")));
				}
				if (xmlStrcasecmp(way_ch->name, BAD_CAST"tag") == 0) {
					if (xmlStrcasecmp(xmlGetProp(way_ch, BAD_CAST"k"), BAD_CAST"highway") == 0) {
						tag = (char*) xmlGetProp(way_ch, BAD_CAST"v");
					}
				}
			}
			
			map_way[name] = Way(refs, tag);
			
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
