#include "ofxWaveHandler.h"

ofxWaveHandler::ofxWaveHandler(ofSoundStream* stream, int minimumSec, int width, int height, int ovWidth, int ovHeight) {

	soundStream = stream;
	recBuffer=NULL;
	recPointer = 0;
	isBlocked = false;

	recBufferMin = minimumSec * stream->getNumInputChannels() * stream->getSampleRate();
	recBuffer=(float*)realloc(recBuffer, recBufferMin*sizeof(float));
	if (width == 0 || height == 0) {
		waveFormWidth = ofGetWidth();
		waveFormHeight = ofGetHeight();
	}
	else {
		waveFormWidth = width;
		waveFormHeight = height;
	}
    if (ovHeight == 0) {
        overviewHeight = 100;
    } else {
        overviewHeight = ovHeight;
    }
    
    if (ovWidth == 0) {
        overviewWidth = ofGetWidth();
    } else {
        overviewWidth = ovWidth;
    }

	waveForm.allocate(waveFormWidth, waveFormHeight);
	overView.allocate(overviewWidth, overviewHeight);
}
int ofxWaveHandler::loadBuffer(string fileName, unsigned int startSmpl) {
	if (isBlocked) {
		cout << "!> The buffer is still blocked (loadBuffer)!\n";
		return -1;
	}

	isBlocked = true;
	SndfileHandle	*oFile;
	oFile=new SndfileHandle(ofToDataPath(fileName,true));
	cout << "Opened file: " << fileName << "\n";
	printf ("- Sample rate : %d\n", oFile->samplerate()) ;
	printf ("- Channels    : %d\n", oFile->channels()) ;
	printf ("- Error       : %s\n", oFile->strError());
	printf ("- Frames      : %d\n", int(oFile->frames()));

	// if startSmpl <> 0 then concatenate the file from startSmpl
	recPointer = (startSmpl+oFile->frames())*oFile->channels();
	if (recPointer > recBufferMin) recBuffer=(float*)realloc(recBuffer, recPointer*sizeof(float));
	oFile->readf(recBuffer+(startSmpl*soundStream->getNumInputChannels()), oFile->frames());
	sf_close(oFile->takeOwnership());
	delete oFile;
	isBlocked = false;
	return 0;
}

int ofxWaveHandler::saveBuffer(string fileName, int audioFormat, unsigned int startSmpl, unsigned int endSmpl) {

	if (isBlocked) {
		cout << "!> The buffer is still blocked (saveBuffer)!\n";
		return -1;
	}
	isBlocked = true;

	SndfileHandle	*oFile;
	oFile=new SndfileHandle(ofToDataPath(fileName,true),SFM_WRITE, audioFormat, soundStream->getNumInputChannels() , soundStream->getSampleRate());
	if (!oFile) {
		isBlocked = false;
		cout << "!> SndFileHandl couldn't creat the output file!\n";
		return -1;
	}
	else {
		// calculate and constraint the start and end point of the buffer to write...
		if ((endSmpl*= soundStream->getNumInputChannels())== 0) endSmpl=recPointer;
		else	endSmpl =min((unsigned int)recPointer, endSmpl);
		if ((startSmpl*= soundStream->getNumInputChannels()) >= endSmpl) startSmpl=max((unsigned int)0,endSmpl-soundStream->getNumInputChannels());

		int numSmpl = endSmpl-startSmpl;
		int res=oFile->write(recBuffer+startSmpl, numSmpl);
		if (res < numSmpl) {
			cout << "!> Saving to file had some problems (less data written)!\n" << res << "/" << numSmpl;
		}
		sf_close(oFile->takeOwnership());
		delete oFile;
		isBlocked = false;
	}
	return 0;
}

int ofxWaveHandler::clearBuffer() {

	if (isBlocked) {
		cout << "!> The buffer is still blocked (clearBuffer)!\n";
		return -1;
	}
	isBlocked = true;
	recPointer = 0;
/*	
	if(recBuffer!=NULL) {
		free(recBuffer);
		recBuffer=NULL;
		recPointer = 0;
	}
*/
	isBlocked = false;
	return 0;
}

void ofxWaveHandler::addSamples(float* input, int numSamples){
	if(!isBlocked){
		isBlocked = true;
		if (recPointer+numSamples > recBufferMin) recBuffer=(float*)realloc(recBuffer, (recPointer+numSamples)*sizeof(float));
		memcpy(&recBuffer[recPointer], input, numSamples*sizeof(float));
		recPointer+= numSamples;
		isBlocked = false;
	}
}

float ofxWaveHandler::getSample(int startSmple){
    if (startSmple > getBufferLengthSmpls()) {
        startSmple = getBufferLengthSmpls();
    }
    return recBuffer[startSmple];
}


void ofxWaveHandler::updateWaveMesh(int detail, unsigned int startSmpl, int length) {
	waveMesh.clear();
	waveMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	waveMesh.setupIndicesAuto();
	//waveMesh.disableIndices();

	if (detail==0) detail= waveFormWidth;
	else detail= min(detail, waveFormWidth);

	int channels = soundStream->getNumInputChannels();
	
	// calculate and constraint the start and end point of the buffer to draw...
	if (length==0) length = (recPointer/channels);
	if (startSmpl*channels >= recPointer) startSmpl=recPointer-channels;
	if ((startSmpl+length)*channels>recPointer) length = (recPointer/channels)-startSmpl;

    float per = length / detail;
	float lastIdx = 0;
    for (int i = 0; i < detail; ++i) {
		// V1 - averaging
			int nextIdx = int((i*per)+startSmpl);
			float summa = 0;
			for (int j=lastIdx;j <=nextIdx;++j) summa += recBuffer[j*channels];
			summa/= (1+nextIdx-lastIdx);
			lastIdx= nextIdx;
			summa*= waveFormHeight;
		
		// V2 - sampling (less CPU heavy, but maybe nearly the same output)
        //	float summa = ((recBuffer[int((i*per)+startSmpl)*channels])*waveFormHeight);

		waveMesh.addColor(ofColor(0,0,0,255));
		waveMesh.addVertex(ofPoint((2*float(i)/detail-1)*waveFormWidth,0, 0));
		waveMesh.addColor(ofColor(120,120,120,255));
		waveMesh.addVertex(ofPoint((2*float(i)/detail-1)*waveFormWidth,summa, 0));
	}
}

void ofxWaveHandler::updateWaveBuffer(unsigned int startSmpl, int length) {
	waveForm.begin();
    ofSetColor(225);
	ofRect(0, 0, waveFormWidth, waveFormHeight);
	
	if (recBuffer == NULL || isBlocked) {
		waveForm.end();
		return;
	}
	isBlocked = true;
    ofSetColor(150);
    
	int channels = soundStream->getNumInputChannels();
	
	// calculate and constraint the start and end point of the buffer to draw...
	if (length==0) length = (recPointer/channels);
	if (startSmpl*channels >= recPointer) startSmpl=recPointer-channels;
	if ((startSmpl+length)*channels>recPointer) length = (recPointer/channels)-startSmpl;

    float per = length / waveFormWidth;

    for (int i = 0; i < waveFormWidth; ++i) {
        float h = abs((recBuffer[int((i*per)+startSmpl)*channels] * waveFormHeight));
        ofRect(i, waveFormHeight/2 - h, 1, h*2);
	}
	isBlocked = false;
    waveForm.end();
}

void ofxWaveHandler::updateOverviewBuffer() {
    
    unsigned int startSmpl = 0;
    int length = getBufferLengthSmpls();
	overView.begin();
    ofSetColor(225);
	ofRect(0, 0, overviewWidth, overviewHeight);
	
	if (recBuffer == NULL || isBlocked) {
		waveForm.end();
		return;
	}
	isBlocked = true;
    ofSetColor(150);
    
	int channels = soundStream->getNumInputChannels();
	
	// calculate and constraint the start and end point of the buffer to draw...
	if (length==0) length = (recPointer/channels);
	if (startSmpl*channels >= recPointer) startSmpl=recPointer-channels;
	if ((startSmpl+length)*channels>recPointer) length = (recPointer/channels)-startSmpl;
    
    float per = length / overviewWidth;
    
    for (int i = 0; i < overviewWidth; ++i) {
        float h = abs((recBuffer[int((i*per)+startSmpl)*channels] * overviewHeight));
        ofRect(i, overviewHeight/2 - h, 1, h*2);
	}
	isBlocked = false;
    overView.end();
}

void ofxWaveHandler::drawWaveMesh(float xPos, float yPos) {
	if (recPointer == 0) return;
	cam.begin();
	ofScale(0.5, 0.5, 0.5);
	ofTranslate(xPos, yPos,0);
	waveMesh.draw();
	cam.end();
}

void ofxWaveHandler::drawWaveBuffer(float xPos, float yPos) {
	ofSetColor(255);
	if (recPointer > 0) {
        waveForm.draw(xPos,yPos);
    };
}

void ofxWaveHandler::drawOverviewBuffer(float xPos, float yPos) {
	ofSetColor(255);
	if (recPointer > 0) {
        overView.draw(xPos,yPos);
    };
}

int ofxWaveHandler::getBufferLengthSmpls() {
	return recPointer/soundStream->getNumInputChannels();
}

float ofxWaveHandler::getBufferLengthSmplsf() {
	return (float)(recPointer/soundStream->getNumInputChannels());
}

float ofxWaveHandler::getBufferLengthSec() {
	return float(recPointer)/(soundStream->getNumInputChannels()*soundStream->getSampleRate());
}