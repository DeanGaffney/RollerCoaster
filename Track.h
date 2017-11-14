#pragma once
#include "ofMain.h"
#include "Node.h"

class Track {

	public:
		Track();
		const float RANGE = 16;
		const float HEIGHT = 3;
		const float NUM_OF_NODES = 200;

		//line properties
		float angle;

		//circle properties
		float radius, pitch, yaw;

		//loop properties
		float xFactor, yFactor, zFactor;

		enum class Type {
			LINE,
			CIRCLE,
			LOOP
		};

		Type type;				//track type

		std::vector<Node::Ref> nodes;	//nodes for the path

		ofPolyline path;		//track path

		void generate();
		void draw();

		void addNode(float x, float y, float z, float scale, int pathIndex);
		void setNodesScaleFactor();
		ofVec3f getBeadPositionFromScale(float beadPositionScale);
		ofVec3f getTangentAtPosition(float beadPositionScale);
};
