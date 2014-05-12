#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	currentSlot = 0;
	waveStart = 0;
	waveLength = 0;
	meshDetail = 0;
    
    isRecording=false;
    isPlaying=false;
    soundStream.listDevices();
    soundStream.setDeviceID(4);
    soundStream.setup(this, NUM_CHANNELS, NUM_CHANNELS, SAMPLE_RATE, STREAM_BUFFER_SIZE, 4);
    waveObject = new ofxWaveHandler(&soundStream, WAVEBUFFER_MINSEC, ofGetWidth()-30, 400);
    
}

//--------------------------------------------------------------
void ofApp::update(){
    waveObject->updateWaveBuffer(waveStart, waveLength);
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(255,255, 255);
	ofSetColor(20,20,20);
	ofDrawBitmapString("PRESS SPACE to start/pause recording to the end of the actual slot...",20,20);
	ofDrawBitmapString("PRESS P to play back current slot...",20,40);
	ofDrawBitmapString("PRESS S to save current slot as in a file...",20,60);
	ofDrawBitmapString("PRESS C to clear current slot...",20,80);
	ofDrawBitmapString("PRESS 0-9 to select new slot and load sample if there were any saved...",20,100);
	ofDrawBitmapString("PRESS K/L to set the drawn wave's starting position...",20,120);
	ofDrawBitmapString("PRESS M/N to set the drawn wave's length...",20,140);
    
	waveObject->drawWaveBuffer(15,255);
    
    if (isPlaying) {
        float playDiv = ((float)ofGetWidth()-30) / (float)(waveCurrent);
        ofRect((playPosition * playDiv) - (waveStart * playDiv) + 15, 255, 3, 395);
        playDiv = ((float)ofGetWidth()-30) / (float)waveObject->getBufferLengthSmpls();
        ofRect((playPosition * playDiv) + 15, 655, 3, 395);
    }
    
	ofDrawBitmapString("SLOT: "+ofToString(currentSlot),25,275);
	ofDrawBitmapString("Current buffer size in samples... "+ofToString(waveObject->getBufferLengthSmpls()),25,560);
	ofDrawBitmapString("Current buffer size in seconds... "+ofToString(waveObject->getBufferLengthSec(),2),25,580);
	ofDrawBitmapString("Wave starting sample... "+ofToString(waveStart),25,600);
	ofDrawBitmapString("Wave length (0 = full)... "+ofToString(waveLength),25,620);
    
	if(isRecording){
		ofSetCircleResolution(50);
		ofSetColor(255,0,0);
		ofCircle(ofGetWidth()/2,455,30);
	}

}

//--------------------------------------------------------------
void ofApp::audioIn(float* input, int bufferSize, int nChannels){
	if(isRecording) {
        waveObject->addSamples(input, bufferSize*nChannels);
    }
}

//--------------------------------------------------------------
void ofApp::audioOut (float* output, int bufferSize, int nChannels){
    if (isPlaying) {
        for (int i = 0; i < bufferSize; i++){
            output[i*nChannels    ] = waveObject->getSample(playPosition);
            playPosition++;
            waveCurrent = waveLength;
            if (waveCurrent == 0 || waveCurrent > waveObject->getBufferLengthSmpls()) {
                waveCurrent = waveObject->getBufferLengthSmpls();
            }
            if (playPosition > waveCurrent + waveStart) {
                playPosition = waveStart;
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key=='s') {
		string currentRecording=ofToString(currentSlot,2,'0')+".wav";
		string currentDatafile=ofToString(currentSlot,2,'0')+".txt";
		cout<<"Saving: " << currentRecording << "\n";
		waveObject->saveBuffer(currentRecording, SF_FORMAT_WAV | SF_FORMAT_PCM_16, 0, 0);
	}
	if (key==' '){
		if(isRecording) {
			cout<<"Stop recording...\n";
			isRecording = false;
		}
		else {
			cout<<"Start recording...\n";
			isRecording = true;
            isPlaying = false;
		}
        
	}
	if (key>='0' && key <= '9') {
        isRecording = false;
        isPlaying = false;
		currentSlot = key-'0';
		string fileNameToLoad="0"+ofToString(currentSlot)+string(".wav");
		string dataNameToLoad="0"+ofToString(currentSlot)+string(".txt");
		waveObject->loadBuffer(fileNameToLoad);
	}
	if (key=='c') {
		waveObject->clearBuffer();
	}
	if (key=='k') {
		waveStart +=22050;
	}
	if (key=='l') {
		waveStart= max(0,waveStart-44100);
	}
	if (key=='n') {
		waveLength +=11025;
	}
	if (key=='m') {
		waveLength= max(0,waveLength-11025);
	}
	if (key=='g') {
		meshDetail +=10;
	}
	if (key=='h') {
		meshDetail= max(0,waveLength-10);
	}
	if (key=='p') {
        if(!isRecording) {
            playPosition = waveStart;
            isPlaying = true;
        }
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void ofApp::exit(){
    soundStream.close();
	delete waveObject;
}
