#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofBackground(0,0,0);
	// in this example actSlot is 0-9 and represents the current audio we work with.
	// in the ofxWaveHandler we only have one buffer, so if we press 0-9, then it will load an audio file 00.wav-09.wav
	// to its buffer. If there is no audio file with tha name, then it will just clear the actual buffer.
	actSlot = 0;
	waveStart = 0;
	waveLength = 0;
	meshDetail = 0;
    
	// when it's true the incoming audio will be appended to the current buffer...
	isRecording=false;
    
	// this method just prints the available sound devices on the consol...
	soundStream.listDevices();
	
	// you can change the deviceID to 0 if you only have one soundcard...
	// I have to use 1 because 0 is the integrated soundboard and 1 is my USB card...
	// Check the console for the deviceIDs...
	soundStream.setDeviceID(0);
    
	// you can use soundStream.start() and soundStream.stop() to start/stop the audiostream
	// but soundStream.setup() automatically starts it and as we check the "recording" variable
	// in the audioIn() event (Last function in this file, where we append new samples), it's not
	// an essential thing to toggle it when we press space, we just toggle the "recording" variable
	// as we can't skip that due to lack of a "soundStream.isOn()" function...
	soundStream.setup(this, 0, NUM_CHANNELS, SAMPLE_RATE, STREAM_BUFFER_SIZE, 4);
    
	// waveHandler' parameters are a pointer to the soundStream and the waveForm dimensions
	// WAVEBUFFER_MINSEC defines the buffesr size to start with in seconds. The highest,the more meory optimized (less memcpy and reallocation)
	// so if you don't use longer sounds than 60sec, then use 60 there. Won't be a problem if you record longer, just then the processor
	// will have to do some extra memory handling work...
	waveObject = new ofxWaveHandler(&soundStream, WAVEBUFFER_MINSEC, ofGetWidth()-30, 400);
}

//--------------------------------------------------------------
void ofApp::update(){
	// updating the FBO every fram from the actual audio buffer...
	// there is a 2 parameters version, where you can define a window to be drawn with a start sample and length in samples...
	// updateWaveBuffer(unsigned int startSmpl, int length)
	waveObject->updateWaveBuffer(waveStart, waveLength);
	waveObject->updateWaveMesh(meshDetail,waveStart, waveLength);
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(255,255, 255);
	ofSetColor(20,20,20);
	ofDrawBitmapString("PRESS SPACE to start/pause recording to the end of the actual slot...",20,20);
	ofDrawBitmapString("PRESS S to save actual slot as in a file...",20,40);
	ofDrawBitmapString("PRESS C to clear actual slot...",20,60);
	ofDrawBitmapString("PRESS 0-9 to select new slot and load sample if there were any saved...",20,80);
	ofDrawBitmapString("PRESS K/L to set the drawn wave's starting position...",20,100);
	ofDrawBitmapString("PRESS M/N to set the drawn wave's length...",20,120);
	ofDrawBitmapString("PRESS G/H to set the mesh detail...",20,140);
    
	// without parameters it will draw the fbo to (0,0):
	// waveObject->drawWaveBuffer();
	waveObject->drawWaveBuffer(15,255);
	// parameters can translate the mesh
	waveObject->drawWaveMesh(0,0);
    
	ofDrawBitmapString("SLOT: "+ofToString(actSlot),25,275);
	ofDrawBitmapString("Current buffer size in samples... "+ofToString(waveObject->getBufferLengthSmpls()),25,560);
	ofDrawBitmapString("Current buffer size in seconds... "+ofToString(waveObject->getBufferLengthSec(),2),25,580);
	ofDrawBitmapString("Wave starting sample... "+ofToString(waveStart),25,600);
	ofDrawBitmapString("Wave length (0 = full)... "+ofToString(waveLength),25,620);
	ofDrawBitmapString("MeshDetail (0 = full)... "+ofToString(meshDetail),25,640);
    
	if(isRecording){
		ofSetCircleResolution(50);
		ofSetColor(255,0,0);
		ofCircle(ofGetWidth()/2,455,30);
	}
}

//--------------------------------------------------------------
void ofApp::exit(){
	// clearing up the soundStream and waveHandler objects
	soundStream.close();
	delete waveObject;
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key=='s') {
		string currentRecording=ofToString(actSlot,2,'0')+".wav";
		cout<<"Saving: " << currentRecording << "\n";
        
		// FORMAT EXAMPLES:
		// SF_FORMAT_AIFF | SF_FORMAT_PCM_16;
		// SF_FORMAT_RAW | SF_FORMAT_PCM_16;
		// SF_FORMAT_AU | SF_FORMAT_FLOAT;
		// (see sndfile.h for more information)
        
		// you can use the saveBuffer method with 2 parameters only, then it will save the whole buffer
		// int saveBuffer(string fileName, int audioFormat= SF_FORMAT_WAV|SF_FORMAT_PCM_16)
		// retun value >> -1=ERROR / 0 = OK
		// if you use the four parameter version, the last 2 numbers are the starting and ending sample number to save from the buffer
		// if both is 0, then it will save the whole buffer.
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
		}
        
	}
	if (key>='0' && key <= '9') {
		actSlot = key-'0';
		string fileNameToLoad="0"+ofToString(actSlot)+string(".wav");
        
		// here you can overwrite the buffer in memory from a file... if you use a second argument that gives an offset where to start
		// filling up the buffer (and extending if necessary)... If no second argument, then the starting sample = 0, so fully overwrite the buffer.
		//
		// this code will append a file to the end of the current buffer:
		// waveObject->loadBuffer(fileNameToLoad, waveObject->getBufferLengthSmpls());
		//
		// retun value >> -1=ERROR / 0 = OK
		waveObject->loadBuffer(fileNameToLoad);
	}
	if (key=='c') {
		// this just clears the actual buffer
		// retun value >> -1=ERROR / 0 = OK
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
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

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
void ofApp::audioIn(float* input, int bufferSize, int nChannels){
	if(isRecording) waveObject->addSamples(input,bufferSize*nChannels);
}
