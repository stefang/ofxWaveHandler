 ofxWaveHandler
 ==============
 
 Wrapper for libsndfile. Also does waveform generation etc... Created by Attila Fodor on 07/05/2014 - kevin@finecutbodies.com
 
 STATUS REPORT:
 
 - As the class can't handle soundstreams with different channel and different sample rate,
 this could be a possible glitch in the system. Can be sorted if there is a plan to
 release it to the public...
 - It's not 100% threadsafe as the audioIn eventhandler writes to the buffer what other processes
 can read. There is a basic blocking mechanism to handle this, but no real semaphors involved
 so not so robust code. Also they are not writing and reading the same memory the same time,
 the only problem can be if the recorded wave buffer overflow the "WAVEBUFFER_MINSEC" parameter.
 Even then there is no problem, just in that case the system is reallocating and moving the buffer to
 another memory position, and rarely it can have threading problems...