#include "Track.h"

Track::Track() {
	type = Type::CIRCLE;
	nodes.reserve(NUM_OF_NODES);
	angle = 0.0f;
	radius = 5.0f;
	pitch = 0.0f;
	yaw = 0.0f;
	xFactor = yFactor = zFactor = 0.0f;
}

void Track::generate() {

	//path.clear needed here to redraw everytime a property changes
	path.clear();
	nodes.clear();
	switch (type) {
		case(Type::LINE): {
			float currentDistance = 0.0f;
			for (int n = 0; n < NUM_OF_NODES; n++) {
				double s{ 1.0 / (NUM_OF_NODES - 1) * n };
				//get unrotated points
				float x = (s - 0.5) * RANGE;
				float y{ 0.0 };
				float z{ 0.0 };

				//rotate points
				ofMatrix4x4 rotationMatrix = ofMatrix4x4();
				rotationMatrix.makeRotationMatrix(0, ofVec3f(1, 0, 0), 0, ofVec3f(0, 1, 0), angle, ofVec3f(0, 0, 1));
				ofVec3f rotatedVec = rotationMatrix.postMult(ofVec3f(x, y, z));
				path.addVertex(rotatedVec.x, rotatedVec.y + HEIGHT, rotatedVec.z);
				if (n != 0){ 
					currentDistance += rotatedVec.distance(nodes[n - 1]->getPos());
				}
				addNode(rotatedVec.x, rotatedVec.y + HEIGHT, rotatedVec.z, currentDistance, n);
			}
			setNodesScaleFactor();
			break;
		}
		case(Type::CIRCLE): {
			float currentDistance = 0.0f;
			for (int n = 0; n < NUM_OF_NODES; n++) {
				double s{ 1.0 / (NUM_OF_NODES - 1) * n };
				//get unrotated x,z,y cordinates
				float x{ radius * cosf(M_TWO_PI * s) };
				float y{ radius * sinf(M_TWO_PI * s)};
				float z{ 0.0 };

				//rotate with rotation matrix
				ofMatrix4x4 rotationMatrix = ofMatrix4x4();
				rotationMatrix.makeRotationMatrix(pitch, ofVec3f(1,0,0), yaw, ofVec3f(0,1,0), 0, ofVec3f(0,0,1));
				ofVec3f rotatedVec = rotationMatrix.postMult(ofVec3f(x, y, z));
				path.addVertex(rotatedVec.x, rotatedVec.y + HEIGHT, rotatedVec.z);
				if (n != 0) {
					currentDistance += rotatedVec.distance(nodes[n - 1]->getPos());
				}
				addNode(rotatedVec.x, rotatedVec.y + HEIGHT, rotatedVec.z, currentDistance, n);
			}
			setNodesScaleFactor();
			break;
		}
		case(Type::LOOP): {
			float currentDistance = 0.0f;
			for (int n = 0; n <NUM_OF_NODES; n++) {
				double s{ 1.0 / (NUM_OF_NODES - 1) * n };
				float x{ radius * cosf(xFactor * M_TWO_PI * s) };
				float z{ radius * sinf(zFactor * M_TWO_PI * s)  };
				float y{ (radius / 4) * sinf(yFactor * M_TWO_PI * s)};
				path.addVertex(x, y + HEIGHT, z);
				if (n != 0) {
					currentDistance += ofVec3f(x, y, z).distance(nodes[n - 1]->getPos());
				}
				addNode(x, y + HEIGHT, z, currentDistance, n);
			}
		}
		 setNodesScaleFactor();
		break;
	}
}

/**
* Creates and adds a node to the nodes vector at the specified path index
**/
void Track::addNode(float x, float y, float z, float scale, int pathIndex) {
	ofVec3f pos = ofVec3f(x, y, z);
	ofVec3f tangent = path.getTangentAtIndex(pathIndex);
	ofVec3f normal = path.getNormalAtIndex(pathIndex);
	Node::Ref node = Node::Ref(new Node(pos, tangent, normal, scale));
	
	nodes.push_back(node);
}

/*
* Sets all nodes on the tracks scale to be that of the scale factor
*/
void Track::setNodesScaleFactor() {
	float totalDistance = nodes[nodes.size() - 1]->getScale();
	float scaleFactor = 1.0f / totalDistance;
	for (int i = 0; i < nodes.size(); ++i) {
		nodes[i]->setScale(nodes[i]->getScale() * scaleFactor);
	}
}

/**
* Returns a vector 3 position on the track from the current bead position scale
*/
ofVec3f Track::getBeadPositionFromScale(float beadPositionScale){
	for (int i = 0; i < nodes.size(); i++) {
		float currentScale = nodes[i]->getScale();
		if (currentScale >= beadPositionScale) {
			return nodes[i]->getPos();
		}
	}
	return ofVec3f::zero();
}

void Track::draw() {
	path.draw();

	for (int i = 0; i < nodes.size(); i++) {
		ofVec3f pos = nodes[i]->getPos();
		ofVec3f norm = nodes[i]->getNormal();
		ofVec3f tangent = nodes[i]->getTangent();

		ofSetLineWidth(1.0f);

		ofSetColor(255, 0, 0);
		ofDrawLine(pos.x, pos.y, pos.z, pos.x + norm.x, pos.y + norm.y, pos.z + norm.z);

		ofSetColor(0, 255, 0);
		ofDrawLine(pos.x, pos.y, pos.z, pos.x + tangent.x, pos.y + tangent.y, pos.z + tangent.z);
	}
}
