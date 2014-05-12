#pragma once

#include "ofMain.h"
#include "ofxWaveHandler.h"

#define NUM_CHANNELS 1
#define SAMPLE_RATE 44100
#define STREAM_BUFFER_SIZE 1024
#define WAVEBUFFER_MINSEC 60

class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
        void exit();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        // sound input related declarations
        void audioIn(float * input, int bufferSize, int nChannels);
        ofSoundStream	soundStream;
        ofxWaveHandler* waveObject;
        bool			isRecording;
        int				actSlot, waveStart,waveLength, meshDetail;

};
