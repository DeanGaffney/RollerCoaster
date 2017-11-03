#include "Node.h"

Node::Node() {
	pos = tangent = normal = ofVec3f::zero();
}

Node::Node(ofVec3f pos, ofVec3f tangent, ofVec3f normal) {
	this->pos = pos;
	this->tangent = tangent;
	this->normal = normal;
}

ofVec3f Node::getPos() {
	return this->pos;
}

ofVec3f Node::getTangent() {
	return this->tangent;
}

ofVec3f Node::getNormal() {
	return this->normal;
}

float Node::getScale() {
	return this->scale;
}

Node& Node::setScale(const float scale) {
	this->scale = scale;
	return *this;
}