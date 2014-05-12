#include "ofMain.h"
#include "sndfile.hh"

#ifndef waveHandler
#define waveHandler
#pragma once

class ofxWaveHandler {
	public :
		ofxWaveHandler(ofSoundStream* soundStream=NULL, int minimumSec=1, int width=0, int height=0, int overviewHeight = 0);
		~ofxWaveHandler() {free(recBuffer);};

		void addSamples(float* input, int numSamples);
        float getSample(int startSmpl);
		int clearBuffer();
		void drawWaveBuffer(float xPos=0, float yPos=0);
        void drawOverviewBuffer(float xPos=0, float yPos=0);
		void drawWaveMesh(float xPos=0, float yPos=0);
		void updateWaveBuffer(unsigned int startSmpl=0, int length=0);
        void updateOverviewBuffer();
		void updateWaveMesh(int detail=0, unsigned int startSmpl=0, int length=0);
		int getBufferLengthSmpls();
		float getBufferLengthSec();
		int loadBuffer(string fileName, unsigned int startSmpl=0);
		int saveBuffer(string fileName, int audioFormat= SF_FORMAT_WAV|SF_FORMAT_PCM_16, unsigned int startSmpl=0, unsigned int endSmpl=0);

		ofFbo			waveForm, overView;
		ofMesh			waveMesh;
		ofEasyCam		cam;
			
	private:
		ofSoundStream*	soundStream;
		float*			recBuffer;
		unsigned int	recBufferMin;
		int				recPointer;
		bool			isBlocked;

		int				waveFormWidth;
		int				waveFormHeight;
        int             overviewHeight;
};

#endif