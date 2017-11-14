#include "ofApp.h"

using namespace YAMPE; using namespace P;

//--------------------------------------------------------------
void ofApp::setup() {
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    // repatable randomness
    ofSeedRandom(10);
    
    // simulation specific stuff goes here

    gui.setup();
    ImGui::GetIO().MouseDrawCursor = false;

    // load parameters from file
    // loadFromFile("settings.xml");
    
    // instantiate the ground
    ground.set(RANGE, RANGE);
    ground.rotate(90, 1,0,0);
    
    // lift camera to 'eye' level
    easyCam.setDistance(RANGE);
    float d = easyCam.getDistance();
    easyCam.setPosition(ofVec3f(0, cameraHeightRatio*d, d*sqrt(1.0f-cameraHeightRatio*cameraHeightRatio))+easyCamTarget);
    easyCam.setTarget(easyCamTarget);
    
    
    // TODO - simulation specific stuff goes here

    isForceVisible = false;
        
    ForceGenerator::Ref fg;
    
    ForceGenerator::Ref gravity(new GravityForceGenerator(ofVec3f(0, -1, 0)));

	bead = Particle::Ref(new Particle());
	ofVec3f scaledPos = track.getBeadPositionFromScale(beadSliderPos);
	bead->setPosition(scaledPos);

	particles.push_back(bead);

	forceGenerators.add(bead, gravity);

	track.generate();
    
    // finally start everything off by resetting the simulation
    reset();
    
}

void ofApp::reset() {

    t = 0.0f;

	scalePos = beadSliderPos;
	acc = 0;
	vel = 0;
	ofVec3f scaledPos = track.getBeadPositionFromScale(beadSliderPos);
	bead->setPosition(scaledPos);
    
}

void ofApp::update() {

    float dt = ofClamp(ofGetLastFrameTime(), 0.0, 0.02);
    if (!isRunning || dt<=0.0f) return;
    t += dt;

	ofVec3f nodeTangent = track.getTangentAtPosition(beadSliderPos);
	float theta = nodeTangent.angle(ofVec3f(0, 1, 0)) - 90.0f;
	
	cout << "Theta:" << theta << "\n";
	acc = 9.81f * sin(ofDegToRad(theta));
	cout << "Acc: " << acc << "\n";
	ofVec3f acceleration = nodeTangent * acc;
	if (acc > 0) {
		vel += dt * acceleration.length();
		cout << "Vel Increase: " << vel << "\n";
	}
	else {
		vel -= dt * acceleration.length();
		cout << "Vel Decrease: " << vel << "\n";
	}
	beadSliderPos += dt * vel;
	while (beadSliderPos > 1) {
		beadSliderPos -= 1;
	}
	while (beadSliderPos < 0) {
		beadSliderPos += 1;
	}
	cout << "Scale Pos:" << scalePos << "\n";
	bead->setPosition(track.getBeadPositionFromScale(beadSliderPos));
}

void ofApp::draw() {
    
    ofEnableDepthTest();
    ofBackgroundGradient(ofColor(128), ofColor(0), OF_GRADIENT_BAR);
    
    ofPushStyle();
    easyCam.begin();
    
    ofDrawGrid(RANGE/(2*8), 8, false, isXGridVisible, isYGridVisible, isZGridVisible);
    
    if (isAxisVisible) ofDrawAxis(1);
    
    if (isGroundVisible) {
        ofPushStyle();
        ofSetHexColor(0xB87333);
        ground.draw();
        ofPopStyle();
    }
    
    
    // render all particles
    for(auto p: particles) (*p).draw();

	track.draw();
	
    easyCam.end();
    ofPopStyle();

    // draw gui elements
    gui.begin();
    drawAppMenuBar();
    drawMainWindow();
    drawLoggingWindow();
    gui.end();
    
}


void ofApp::drawAppMenuBar() {
    
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            ImGui::Separator();
            if (ImGui::MenuItem("Quit", "")) quit();
            ImGui::EndMenu();
        }
        
        float d = easyCam.getDistance();
        
        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem(isAxisVisible?"Hide Unit Axis":"Show Unit Axis", "")) isAxisVisible = !isAxisVisible;
            if (ImGui::MenuItem(isGroundVisible?"Hide Ground":"Show Ground", "")) isGroundVisible = !isGroundVisible;
            if (ImGui::MenuItem(isXGridVisible?"Hide Grid (X)":"Show Grid (X)", "")) isXGridVisible = !isXGridVisible;
            if (ImGui::MenuItem(isYGridVisible?"Hide Grid (Y)":"Show Grid (Y)", "")) isYGridVisible = !isYGridVisible;
            if (ImGui::MenuItem(isZGridVisible?"Hide Grid (Z)":"Show Grid (Z)", "")) isZGridVisible = !isZGridVisible;
            ImGui::Separator();
            if (ImGui::MenuItem("Align camera above X axis ", "")) {
                easyCam.setPosition(ofVec3f(d*sqrt(1.0f-cameraHeightRatio*cameraHeightRatio), cameraHeightRatio*d, 0)+easyCamTarget);
                easyCam.setTarget(easyCamTarget);
            }
            if (ImGui::MenuItem("Align camera above Z axis ", "")) {
                easyCam.setPosition(ofVec3f(0, cameraHeightRatio*d, d*sqrt(1.0f-cameraHeightRatio*cameraHeightRatio))+easyCamTarget);
                easyCam.setTarget(easyCamTarget);
                cout <<"here";
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Align camera along X axis ", "")) {
                easyCam.setPosition(ofVec3f(d, 0, 0)+easyCamTarget);
                easyCam.setTarget(easyCamTarget);
            }
            if (ImGui::MenuItem("Align camera along Y axis ", "")) {
                easyCam.setPosition(ofVec3f(0.001, d, 0)+easyCamTarget);
                easyCam.setTarget(easyCamTarget);
            }
            if (ImGui::MenuItem("Align camera along Z axis ", "")) {
                easyCam.setPosition(ofVec3f(0, 0, d)+easyCamTarget);
                easyCam.setTarget(easyCamTarget);
            }
            
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Application")) {
            if (ImGui::MenuItem("Add application specific menu items here", "")) {
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}


void ofApp::drawMainWindow() {
    

    ImGui::SetNextWindowSize(ImVec2(200,300), ImGuiSetCond_FirstUseEver);
    if (ImGui::Begin("Main")) {

        if (ImGui::CollapsingHeader("3D")) {
            if(ImGui::Button("Reset##CameraTarget")) {
                easyCamTarget = ofVec3f(0,5,0);
                easyCam.setTarget(easyCamTarget);
            }

            ImGui::SameLine();
            if (ImGui::InputFloat3("Camera Target", &easyCamTarget.x)) {
                easyCam.setTarget(easyCamTarget);
            }
            if (ImGui::SliderFloat("Camera Height Ratio", &cameraHeightRatio, 0.0f, 1.0f))
                cameraHeightRatioChanged(cameraHeightRatio);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }

        if(ImGui::Button("Reset")) reset();
        ImGui::SameLine();
        if(ImGui::Button(isRunning?"Stop":" Go ")) isRunning = !isRunning;
        ImGui::SameLine();
        ImGui::Text("   Time = %8.1f", t);
        if(ImGui::Button("Quit")) quit();

		//track radio buttons
		if (ImGui::CollapsingHeader("Tracks")) {
			if (ImGui::RadioButton("Line", (track.type == Track::Type::LINE))) {
				track.type = Track::Type::LINE;
				track.generate();
				reset();
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Circle", (track.type == Track::Type::CIRCLE))) {
				track.type = Track::Type::CIRCLE;
				track.generate();
				reset();
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Loops", (track.type == Track::Type::LOOP))) {
				track.type = Track::Type::LOOP;
				track.generate();
				reset();
			}

		}
		std::string degFormat = "%3.0f (deg)";
		//track params
		if (ImGui::CollapsingHeader("Track Parameters")) {
			switch (track.type) {
				case (Track::Type::LINE): {
					if (ImGui::SliderFloat("Angle", &track.angle, -90.0f, 90.0f, degFormat.c_str(), 1)) { 
						track.generate(); 
						reset();
					}
					break;
				}
				case (Track::Type::CIRCLE): {
					if (ImGui::SliderFloat("Radius", &track.radius, 0.0f, 8.0f, degFormat.c_str(), 1)) { 
						track.generate(); 
						reset();
					}
					if (ImGui::SliderFloat("Pitch", &track.pitch, 0.0f, 360.0f, degFormat.c_str(), 1)) { 
						track.generate();
						reset();
					}
					if (ImGui::SliderFloat("Yaw", &track.yaw, 0.0f, 360.0f, degFormat.c_str(), 1)) { 
						track.generate();
						reset();
					}
					break;
				}
				case(Track::Type::LOOP): {
					if (ImGui::SliderFloat("Radius", &track.radius, 0.0f, 8.0f, degFormat.c_str(), 1)) { 
						track.generate(); 
						reset();
					}
					if (ImGui::SliderFloat("XFactor", &track.xFactor, 0.0f, 10.0f, degFormat.c_str(), 1)) { 
						track.generate(); 
						reset();
					}
					if (ImGui::SliderFloat("ZFactor", &track.zFactor, 0.0f, 10.0f, degFormat.c_str(), 1)) { 
						track.generate(); 
						reset();
					}
					if (ImGui::SliderFloat("YFactor", &track.yFactor, 0.0f, 10.0f, degFormat.c_str(), 1)) { 
						track.generate(); 
						reset();
					}
					break;
				}
			}
		}

		if (ImGui::SliderFloat("Bead Position", &beadSliderPos, 0.0f, 1.0f, "%.2f")) {
			bead->setPosition(track.getBeadPositionFromScale(beadSliderPos));
		}
        
        // TODO - numeric output goes here
        if (ImGui::CollapsingHeader("Numerical Output")) {

		}
    }
    
    // store window size so that camera can ignore mouse clicks
    mainWindowRectangle.setPosition(ImGui::GetWindowPos().x,ImGui::GetWindowPos().y);
    mainWindowRectangle.setSize(ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
    ImGui::End();

}


void ofApp::drawLoggingWindow() {
    ImGui::SetNextWindowSize(ImVec2(290,300), ImGuiSetCond_Always);
    ImGui::SetNextWindowPos(ImVec2(ofGetWindowWidth()-300,20), ImGuiSetCond_Always);
    
    if (ImGui::Begin("Logging")) {
    }
    // store window size so that camera can ignore mouse clicks
    loggingWindowRectangle.setPosition(ImGui::GetWindowPos().x,ImGui::GetWindowPos().y);
    loggingWindowRectangle.setSize(ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
    ImGui::End();
}


//--------------------------------------------------------------
// GUI events and listeners
//--------------------------------------------------------------

void ofApp::keyPressed(int key) {
    
    float d = easyCam.getDistance();
    
    switch (key) {
        
//        case 'h':                               // toggle GUI/HUD
//           isGuiVisible = !isGuiVisible;
//            break;
//        case 'b':                               // toggle debug
//            isDebugVisible = !isDebugVisible;
//            break;
//        case 'a':                               // toggle axis unit vectors
//            isAxisVisible = !isAxisVisible;
//            break;
//        case 'g':                               // toggle ground
//            isGroundVisible = !isGroundVisible;
//            break;
//        case 'u':                               // set the up vecetor to be up (ground to be level)
//            easyCam.setTarget(ofVec3f::zero());
//            break;
//
//        case 'S' :                              // save gui parameters to file
//            gui.saveToFile("settings.xml");
//
//            break;
//        case 'L' :                              // load gui parameters
//            gui.loadFromFile("settings.xml");
//            break;
//
        case 'z':
            easyCam.setPosition(ofVec3f(0, cameraHeightRatio*d, d*sqrt(1.0f-cameraHeightRatio*cameraHeightRatio))+easyCamTarget);
            easyCam.setTarget(easyCamTarget);
            break;
        case 'Z':
            easyCam.setPosition(0, 0, d);
            easyCam.setTarget(ofVec3f::zero());
            break;
        case 'x':
            easyCam.setPosition(ofVec3f(d*sqrt(1.0f-cameraHeightRatio*cameraHeightRatio), cameraHeightRatio*d, 0)+easyCamTarget);
            easyCam.setTarget(easyCamTarget);
            break;
        case 'X':
            easyCam.setPosition(ofVec3f(d, 0, 0)+easyCamTarget);
            easyCam.setTarget(easyCamTarget);
            break;
        case 'Y':
            easyCam.setPosition(ofVec3f(0.001, d, 0)+easyCamTarget);
            easyCam.setTarget(easyCamTarget);
            break;
            
        case 'f':                               // toggle fullscreen
            // BUG: window size is not preserved
            isFullScreen = !isFullScreen;
            if (isFullScreen) {
                ofSetFullscreen(false);
            } else {
                ofSetFullscreen(true);
            }
            break;

        // simulation specific stuff goes here

    }
}


void ofApp::cameraHeightRatioChanged(float & cameraHeightRatio) {
    
    float d = easyCam.getDistance();
    easyCam.setPosition(ofVec3f(0, cameraHeightRatio*d, d*sqrt(1.0f-cameraHeightRatio*cameraHeightRatio))+easyCamTarget);
    easyCam.setTarget(easyCamTarget);
}


void ofApp::quit() {
    ofExit();
}

//--------------------------------------------------------------
// Unused
//--------------------------------------------------------------
void ofApp::keyReleased(int key) {}
void ofApp::mouseMoved(int x, int y ) {}
void ofApp::mouseDragged(int x, int y, int button) {}
void ofApp::mousePressed(int x, int y, int button) {
    // easy camera should ignore GUI mouse clicks
    if (mainWindowRectangle.inside(x,y)||loggingWindowRectangle.inside(x,y))
        easyCam.disableMouseInput();
    else
        easyCam.enableMouseInput();
}
void ofApp::mouseReleased(int x, int y, int button) {}
void ofApp::mouseEntered(int x, int y) {}
void ofApp::mouseExited(int x, int y) {}
void ofApp::windowResized(int w, int h) {}
void ofApp::gotMessage(ofMessage msg) {}
void ofApp::dragEvent(ofDragInfo dragInfo) {}
