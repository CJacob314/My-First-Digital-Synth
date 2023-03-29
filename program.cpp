#include "synth.h"

using std::cout;

Octave* freqGetter = nullptr;

int main(int argc, char *argv[])
{
    if(argc < 3){
        putError("Pass, at least, the filename to generate to.", 1);
    }

    signed int strcmp_res;
    if(!(strcmp_res = strcmp("-i", argv[1])) || strcmp_res == 60){
        startInteractive(argc - 1, argv + 1);
        exit(0);
    }

    unsigned short octave = 4;
    if(argc > 3){
        try {
            octave = std::stoi(argv[1]);
        } catch (const std::invalid_argument& ia) {
            putError("Could not parse octave number as integer.", 2);
        }        
    }

    std::string fname = combineArgv(argc - 1, argv + 1);

    triangleWave(fname, 440, 5, 0.5);

    return 0;
}

void startInteractive(int argc, char *argv[]){
    static unsigned int harmonics = 0;
    static double volume = 0.25;
    static double dd = 0.75; // Default Duration
    static double harmVolume = 0.25;

    unsigned short octave = 4;
    if(argc > 3){
        try {
            octave = std::stoi(argv[1]);
        } catch (const std::invalid_argument& ia) {
            putError("Could not parse octave number as integer.", 2);
        }
    }

    freqGetter = new Octave(octave);

    std::string fname = combineArgv(argc - 1, argv + 1);
    
    // std::vector<double> freqs;
    
    // Check if the file already exists
    std::ifstream fstream(fname);
    int needInit = !fstream.good();
    fstream.close(); // Just in case, I'd like to make sure the file is closed.

    if(needInit){
        std::ofstream outfile(fname);
    }

    for(std::string line; std::getline(std::cin, line);){

        for(size_t i = 0; i < line.length();){
            unsigned int first;
            // std::string note = line.substr(i, !(first = line.find_first_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ", i)) ? 1 : first - i);
            std::string note = line.substr(i, (first = line.find_first_of("ABCDEFGOVH() ", i+1)) - i);
            i += note.length();

            if(note == "O"){
                // Read an integer and change the octave with freqGetter->setOctave(int)
                std::string oct = line.substr(i+1, (first = line.find_first_of(")", i+1)) - i - 1);
                i += oct.length() + 2;
                try {
                    unsigned int octInt = std::stoi(oct);
                    freqGetter->setOctave(octInt);
                    std::cout << "Octave set to: " << octInt << "\n";
                } catch (const std::invalid_argument& ia) {
                    putError("Could not parse octave as integer.", 2);
                }
                continue;
            } else if (note == "H"){
                // Changing/adding harmonics
                std::string harm = line.substr(i+1, (first = line.find_first_of("),", i+1)) - i - 1);
                
                i += harm.length() + 2;
                try {
                    unsigned int harmInt = std::stoi(harm);
                    harmonics = harmInt;
                    std::cout << "Harmonics set to: " << harmInt << "\n";
                } catch (const std::invalid_argument& ia) {
                    putError("Could not parse harmonics to add as an integer.", 5);
                }

                if(line[i-1] == ','){
                    std::string vol = line.substr(i+1, (first = line.find_first_of(")", i+1)) - i - 1);
                    i += vol.length() + 2;
                    try {
                        double volDouble = std::stod(vol);
                        if(volDouble > 1.0 || volDouble < 0.0){
                            putError("Volume must be between 0 and 1.", 4);
                        }
                        harmVolume = volDouble;
                        std::cout << "Volume set to: " << volDouble << "\n";
                    } catch (const std::invalid_argument& ia) {
                        putError("Could not parse volume as double.", 11);
                    }
                } else {
                    harmVolume = 0.25;

                    i += harm.length() + 1;
                }

                continue;
            } else if (note == "V"){
                // Changing volume parameter
                std::string vol = line.substr(i+1, (first = line.find_first_of(")", i+1)) - i - 1);
                i += vol.length() + 2;
                try {
                    double volDouble = std::stod(vol);
                    if(volDouble > 1.0 || volDouble < 0.0){
                        putError("Volume must be between 0 and 1.", 4);
                    }
                    volume = volDouble;
                    std::cout << "Volume set to: " << volDouble << "\n";
                } catch (const std::invalid_argument& ia) {
                    putError("Could not parse volume as double.", 11);
                }
                continue;
            } else if(note == "D" && line[i] == 'd'){
                // Double D means set default duration

                // Get double inside parenthesis
                std::string dur = line.substr(i+2, (first = line.find_first_of(")", i+2)) - i - 2);
                i += dur.length() + 3;
                try {
                    double durDouble = std::stod(dur);
                    if(durDouble < 0.0){
                        putError("Duration must be positive.", 20);
                    }
                    std::cout << "Default duration set to: " << durDouble << "\n";
                    dd = durDouble;
                } catch (const std::invalid_argument& ia) {
                    putError("Could not parse duration as double.", 11);
                }

                continue;
            }

            double second = dd;
            // Test if an opening parenthesis exists right after the note in line, if so, parse out the integer between the parenthesis. If a closing parenthesis doesn't exist, or if the string could not parse to a positive integer, then putError and exit. Note that putError takes the const char* first, then an exit code.
            if(line[i] == '('){
                
                std::string dur = line.substr(i+1, (second = line.find_first_of(")", i+1)) - i - 1);
                i += dur.length() + 2;
                try {
                    double durDouble = std::stod(dur);
                    std::cout << "Note: " << note << ", Duration: " << durDouble << "\n";
                    second = durDouble;
                } catch (const std::invalid_argument& ia) {
                    putError("Could not parse duration as double.", 2);
                }
            } else {
                std::cout << "Note: " << note << ", Duration: " << dd << "\n";
            }
            
            // For debug purposes, I will use a square wave
            sineWave(fname, freqGetter->getFreq(note.c_str()), second, 0.3, !needInit, harmonics, harmVolume);
            if(needInit) needInit = 0;
        }
        
        cout << "Done...\n";
    }
}

void putError(const char* s, int exitCode = -1){
    fprintf(stderr, "%s%s%s", "\033[1;4;31m", s, "\033[0;0m");

    if(exitCode > -1){
        if(!freqGetter){
            delete freqGetter;
        }
        
        exit(exitCode);
    }
}

void initNewWavFile(FILE* fptr, unsigned int fileSize){
    // Write "RIFF"
    fwrite("RIFF", 1, 4, fptr);

    // Write file size
    fwrite(&fileSize, 4, 1, fptr);

    // Write "WAVEfmt "
    fwrite("WAVEfmt ", 1, 8, fptr);

    // Write the length of the format as a 4 byte integer
    unsigned int formatLength = 16;
    fwrite(&formatLength, 4, 1, fptr);

    // Write the format type as a 2 byte integer (PCM)
    unsigned short formatType = 1;
    fwrite(&formatType, 2, 1, fptr);

    // Write the number of channels (1)
    unsigned short channels = 1;
    fwrite(&channels, 2, 1, fptr);


    // Write the sample rate (44100) as a 4 byte integer
    unsigned int sampleRate = 44100;
    fwrite(&sampleRate, 4, 1, fptr);

    // (Sample Rate * BitsPerSample * Channels) / 8
    unsigned int byteRate = 44100 * 16 * 1 / 8;
    fwrite(&byteRate, 4, 1, fptr);

    // (BitsPerSample * Channels) / 8
    unsigned short blockAlign = 16 * 1 / 8;
    fwrite(&blockAlign, 2, 1, fptr);

    // Write the bits per sample (16)
    unsigned short bitsPerSample = 16;
    fwrite(&bitsPerSample, 2, 1, fptr);

    // Write the data chunk header
    fwrite("data", 1, 4, fptr);
}

void squareWave(std::string fname, double freq, unsigned int duration, double volume, int append) {
    // Open file with fopen
    FILE *f;
    if(append){
        f = fopen(fname.c_str(), "r+b");
    } else {
        f = fopen(fname.c_str(), "wb");
    }

    if(!f){
        putError("Could not open file for writing.", 3);
    }

    if(!append){
        unsigned int fileSize = 36 + (duration * 44100 * 2);
        initNewWavFile(f, fileSize);

        // Write the size of the data chunk
        unsigned int dataSize = duration * 44100 * 2;
        fwrite(&dataSize, 4, 1, f);
    } else {
        // First update the file size in the header
        fseek(f, 4, SEEK_SET);
        unsigned int fileSize;
        fread(&fileSize, 4, 1, f);
        fileSize += duration * 44100 * 2;
        fseek(f, 4, SEEK_SET);
        fwrite(&fileSize, 4, 1, f);

        // Seek to just after the data chunk header to change data size
        fseek(f, 40, SEEK_SET);
        unsigned int dataSize;
        fread(&dataSize, 4, 1, f);
        dataSize += duration * 44100 * 2;
        fseek(f, 40, SEEK_SET);
        fwrite(&dataSize, 4, 1, f);

        // Seek to the end of the file now
        fseek(f, 0, SEEK_END);
    }

    
    // Write the actual data
    for (size_t i = 0; i < duration * 44100; i++)
    {
        short sample = ((short)(volume * 32767 * ((i * freq * 2 / 44100)) ) % 2 == 0 ? 1 : -1);
        fwrite(&sample, 2, 1, f);
    }

    // Close the file
    fclose(f);
}


// TODO: Change harmVolume to be a vector or array for different harmonic volumes
void sineWave(std::string fname, double freq, double duration, double volume, int append, int harmonics, double harmVolume) {
    // Open file with fopen
    FILE *f;
    if(append){
        f = fopen(fname.c_str(), "r+b");
        if(!f){
            putError("Could not open file for appending.", 3);
        }
        // Seek to the end of the file to append data
        fseek(f, 0, SEEK_END);
    } else {
        f = fopen(fname.c_str(), "wb");
        if(!f){
            putError("Could not open file for writing.", 3);
        }
        // Write the file header for a new file
        unsigned int fileSize = ceil(36.0 + (duration * 44100.0 * 2.0));
        initNewWavFile(f, fileSize);

        // Write the size of the data chunk
        unsigned int dataSize = ceil(duration * 44100.0 * 2.0);
        fwrite(&dataSize, 4, 1, f);
    }
    
    // Write the actual data
    double t =0; 
    // for (size_t i = 0; i < ceil(duration * 44100.0); i++)
    // {
    //     double floatingSample = sin(2 * M_PI * freq * t);

    //     // Add harmonics
    //     for (size_t j = 0; j < harmonics; j++)
    //     {
    //         floatingSample += sin(2 * M_PI * freq * (j + 2) * t);
    //     }
        
    //     floatingSample *= (32767 * volume);

    //     // Clamp the sample within the range that data can be in a .wav
    //     if(floatingSample > 32767) floatingSample = 32767;
    //     else if (floatingSample < -32767) floatingSample = -32767;

    //     short sample = (short)floatingSample;

    //     fwrite(&sample, 2, 1, f);
    //     t += 1.0/44100.0;
    // }
    for (size_t i = 0; i < ceil(duration * 44100.0); i++)
    {
        double floatingSample = sin(2 * M_PI * freq * t);

        // Add harmonics
        for (size_t j = 0; j < harmonics; j++)
        {
            floatingSample += sin(2 * M_PI * freq * (j + 2) * t) * (j + 2) * harmVolume;
        }

        floatingSample *= (32767 * volume);

        // Clamp the sample within the range that data can be in a .wav
        if(floatingSample > 32767) floatingSample = 32767;
        else if (floatingSample < -32767) floatingSample = -32767;

        short sample = (short)floatingSample;

        fwrite(&sample, 2, 1, f);
        t += 1.0/44100.0;
    }

    // Update the file size in the header if appending
    if (append) {
        // First update the file size in the header
        fseek(f, 4, SEEK_SET);
        unsigned int fileSize;
        fread(&fileSize, 4, 1, f);
        fileSize += ceil(duration * 44100.0 * 2.0);
        fseek(f, 4, SEEK_SET);
        fwrite(&fileSize, 4, 1, f);

        // Seek to just after the data chunk header to change data size
        fseek(f, 40, SEEK_SET);
        unsigned int dataSize;
        fread(&dataSize, 4, 1, f);
        dataSize += ceil(duration * 44100.0 * 2.0);
        fseek(f, 40, SEEK_SET);
        fwrite(&dataSize, 4, 1, f);

        // Seek to the end of the file now
        fseek(f, 0, SEEK_END);
    }

    // Close the file
    fclose(f);
}


/*
void sineWave(std::string fname, unsigned int freq, unsigned int duration, double volume) {
    // Open file with fopen in binary mode
    FILE *f = fopen(fname.c_str(), "w+b");

    // Write "RIFF"
    fwrite("RIFF", 1, 4, f);

    // Write file size
    unsigned int fileSize = 36 + (duration * 44100 * 2);
    fwrite(&fileSize, 4, 1, f);

    // Write "WAVEfmt "
    fwrite("WAVEfmt ", 1, 8, f);

    // Write the length of the format as a 4 byte integer
    unsigned int formatLength = 16;
    fwrite(&formatLength, 4, 1, f);

    // Write the format type as a 2 byte integer (PCM)
    unsigned short formatType = 1;
    fwrite(&formatType, 2, 1, f);

    // Write the number of channels (1)
    unsigned short channels = 1;
    fwrite(&channels, 2, 1, f);


    // Write the sample rate (44100) as a 4 byte integer
    unsigned int sampleRate = 44100;
    fwrite(&sampleRate, 4, 1, f);

    // (Sample Rate * BitsPerSample * Channels) / 8
    unsigned int byteRate = 44100 * 16 * 1 / 8;
    fwrite(&byteRate, 4, 1, f);

    // (BitsPerSample * Channels) / 8
    unsigned short blockAlign = 16 * 1 / 8;
    fwrite(&blockAlign, 2, 1, f);

    // Write the bits per sample (16)
    unsigned short bitsPerSample = 16;
    fwrite(&bitsPerSample, 2, 1, f);

    // Write the data chunk header
    fwrite("data", 1, 4, f);

    // Write the size of the data chunk
    unsigned int dataSize = duration * 44100 * 2;
    fwrite(&dataSize, 4, 1, f);

    // Write the actual data
    double t = 0;
    for (size_t i = 0; i < duration * 44100; i++)
    {
        short sample = 32767 * volume * sin(2 * M_PI * freq * t);
        fwrite(&sample, 2, 1, f);
        t += 1.0/44100;
    }
    
    // Close the file
    fclose(f);
}

*/

void triangleWave(std::string fname, unsigned int freq, unsigned int duration, double volume) {
    // Open file with fopen
    FILE *f = fopen(fname.c_str(), "w+b");

    // Write "RIFF"
    fwrite("RIFF", 1, 4, f);

    // Write file size
    unsigned int fileSize = 36 + (duration * 44100 * 2);
    fwrite(&fileSize, 4, 1, f);

    // Write "WAVEfmt "
    fwrite("WAVEfmt ", 1, 8, f);

    // Write the length of the format as a 4 byte integer
    unsigned int formatLength = 16;
    fwrite(&formatLength, 4, 1, f);

    // Write the format type as a 2 byte integer (PCM)
    unsigned short formatType = 1;
    fwrite(&formatType, 2, 1, f);

    // Write the number of channels (1)
    unsigned short channels = 1;
    fwrite(&channels, 2, 1, f);

    // Write the sample rate (44100) as a 4 byte integer
    unsigned int sampleRate = 44100;
    fwrite(&sampleRate, 4, 1, f);

    // (Sample Rate * BitsPerSample * Channels) / 8
    unsigned int byteRate = 44100 * 16 * 1 / 8;
    fwrite(&byteRate, 4, 1, f);

    // (BitsPerSample * Channels) / 8
    unsigned short blockAlign = 16 * 1 / 8;
    fwrite(&blockAlign, 2, 1, f);

    // Write the bits per sample (16)
    unsigned short bitsPerSample = 16;
    fwrite(&bitsPerSample, 2, 1, f);

    // Write the data chunk header
    fwrite("data", 1, 4, f);

    // Write the size of the data chunk
    unsigned int dataSize = duration * 44100 * 2;
    fwrite(&dataSize, 4, 1, f);

    // Write the actual data
    for (size_t i = 0; i < duration * 44100; i++)
    {
        double t = (double)i / 44100.0;
        double value = (2.0 * fabs(2.0 * (t * freq - floor(t * freq + 0.5)))) - 1.0;
        short sample = value * 32767.0 * volume;
        fwrite(&sample, 2, 1, f);
    }

    // Close the file
    fclose(f);
}



// TODO: Improve speed
std::string combineArgv(int argc, char *argv[]) {
    std::string fname;
    for (size_t i = 1; i < argc - 1; i++)
    {
        fname += argv[i];
        fname += " ";
    }

    fname += argv[argc - 1];
    return fname;
}