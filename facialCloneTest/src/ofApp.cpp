#include "ofApp.h"

using namespace ofxCv;

void ofApp::setup() {
#ifdef TARGET_OSX
    //ofSetDataPathRoot("../data/");
#endif
    ofSetVerticalSync(true);
    cloneReady = false;
    cam.initGrabber(2560, 1600);
    clone.setup(cam.getWidth(), cam.getHeight());
    ofFbo::Settings settings;
    settings.width = cam.getWidth();
    settings.height = cam.getHeight();
    maskFbo.allocate(settings);
    srcFbo.allocate(settings);
    camTracker.setup();
    srcTracker.setup();
    srcTracker.setIterations(25);
    srcTracker.setAttempts(4);
    
    faces.allowExt("jpg");
    faces.allowExt("png");
    faces.listDir("faces");
    currentFace = 0;
    if(faces.size()!=0){
        loadFace(faces.getPath(currentFace));
    }
}

void ofApp::update() {
    cam.update();
    if(cam.isFrameNew()) {
        camTracker.update(toCv(cam));
        
        cloneReady = camTracker.getFound();
        if(cloneReady) {
            ofMesh camMesh = camTracker.getImageMesh();
            camMesh.clearTexCoords();
            camMesh.addTexCoords(srcPoints);
            
            maskFbo.begin();
            ofClear(0, 255);  //second number used to be 255
            camMesh.draw();
            maskFbo.end();
            
            srcFbo.begin();
            ofClear(0, 255);//second number used to be 255
            src.bind();
            camMesh.draw();
            src.unbind();
            srcFbo.end();
            
            clone.setStrength(16);//EVB you changed this!!! 16);
            clone.update(srcFbo.getTextureReference(), cam.getTextureReference(), maskFbo.getTextureReference());
        }
    }
}

void ofApp::draw() {
    ofSetColor(255);

    if(src.getWidth() > 0 && cloneReady) {
//        ofBackground(0,0,0,255);

        clone.draw(0, 0);

    } else {
        ofBackground(0,0,0,255);
        
        //where it's drawing the camera if it can't find the clone
//        cam.draw(0, 0);

    }
    
    if(!camTracker.getFound()) {
        cout<<"you're in here crap: could not find the camera";
        //drawHighlightString("camera face not found", 10, 10);
    }
    if(src.getWidth() == 0) {
        //cout<<"you're in here crap: drag an image onto the screen";
      //  drawHighlightString("drag an image here", 10, 30);
    } else if(!srcTracker.getFound()) {
        cout<<"you're in here crap: couldn't find the tracker";
    //    drawHighlightString("image face not found", 10, 30);
    }
}

void ofApp::loadFace(string face){
    src.loadImage(face);
    if(src.getWidth() > 0) {
        srcTracker.update(toCv(src));
        srcPoints = srcTracker.getImagePoints();
    }
}

void ofApp::dragEvent(ofDragInfo dragInfo) {
    loadFace(dragInfo.files[0]);
}

void ofApp::keyPressed(int key){
    switch(key){
        case OF_KEY_UP:
            currentFace++;
            break;
        case OF_KEY_DOWN:
            currentFace--;
            break;
    }
    currentFace = ofClamp(currentFace,0,faces.size());
    if(faces.size()!=0){
        loadFace(faces.getPath(currentFace));
    }
}
