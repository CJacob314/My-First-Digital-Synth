#include <cctype>
#include <string.h>
#include <math.h>
#include <cstdio>
#include <iostream>
#include <stdio.h>
#include <cmath>
#include <fstream>
#include <string>


std::string combineArgv(int argc, char *argv[]);
// void sineWave(std::string fname, unsigned int freq, unsigned int duration, double volume);
void sineWave(std::string fname, double freq, double duration, double volume, int append = 0, int harmonics = 0, double harmVolume = 0.0);
void squareWave(std::string fname, double freq, unsigned int duration, double volume, int append = 0);
void triangleWave(std::string fname, unsigned int freq, unsigned int duration, double volume);
void putError(const char* s, int exitCode);
void startInteractive(int argc, char *argv[]);
void initNewWavFile(FILE* fptr);

class NoteNumLookup{
    private:
    
    int arr[12] = {0, 2, 3, 5, 7, 8, 10};

    public:

    int& operator[](char index);

    NoteNumLookup();
};


class Octave{
    private:

    unsigned int octave;

    NoteNumLookup noteLookup = NoteNumLookup();

    // Freqencies, in order starting from A, for the first octave.
    // Will multiply by powers of 2 to get other octaves.
    double freqArr[12] = {
        27.5, 29.1352, 30.8677, 32.7032, 34.6478, 36.7081, 38.8909, 41.2034, 43.6535, 46.2493, 48.9994, 51.9131
    };

    public:

    // Constructor, takes integer value of octave number, starting from 0
    Octave(unsigned int octave);

    Octave(signed int octave);

    double getFreq(const char* note);

    void setOctave(unsigned int octave);
};
