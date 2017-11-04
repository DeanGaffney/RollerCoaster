#pragma once
#include "ofMain.h"

class Node {

	ofVec3f pos;
	ofVec3f tangent;
	ofVec3f normal;
	float scale;

public:
	typedef ofPtr<Node> Ref;

	Node();
	Node(ofVec3f pos, ofVec3f tangent, ofVec3f normal, float scale);
	ofVec3f getPos();
	ofVec3f getTangent();
	ofVec3f getNormal();
	float getScale();
	Node& setScale(const float scale);
};