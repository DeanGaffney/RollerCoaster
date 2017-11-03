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
				addNode(rotatedVec.x, rotatedVec.y + HEIGHT, rotatedVec.z, n);
			}
			break;
		}
		case(Type::CIRCLE): {
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
				addNode(rotatedVec.x, rotatedVec.y + HEIGHT, rotatedVec.z, n);
			}
			break;
		}
		case(Type::LOOP): {
			for (int n = 0; n <NUM_OF_NODES; n++) {
				double s{ 1.0 / (NUM_OF_NODES - 1) * n };
				float x{ radius * cosf(xFactor * M_TWO_PI * s) };
				float z{ radius * sinf(zFactor * M_TWO_PI * s)  };
				float y{ (radius / 4) * sinf(yFactor * M_TWO_PI * s)};
				path.addVertex(x, y + HEIGHT, z);
			    addNode(x, y + HEIGHT, z, n);
			}
		}
		break;
	}
}


void Track::addNode(float x, float y, float z, int pathIndex) {
	ofVec3f pos = ofVec3f(x, y, z);
	ofVec3f tangent = path.getTangentAtIndex(pathIndex);
	ofVec3f normal = path.getNormalAtIndex(pathIndex);
	Node::Ref node = Node::Ref(new Node(pos, tangent, normal));
	nodes.push_back(node);
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
