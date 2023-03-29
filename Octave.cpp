#include "synth.h"

// double Octave::getFreq(const char* note){
//     if(strlen(note) == 0){
//         return 0; // Improper note
//     }

//     if(strlen(note) < 2){
//         // No flats or sharps
        
//         return freqArr[toupper(note[0]) - 65] * pow(2, octave);
//     }

//     // Handle sharps as # and flats with b
//     if(note[1] == '#'){
//         return freqArr[toupper(note[0]) - 65 + 1] * pow(2, octave);
//     } else if(note[1] == 'b'){
//         if(note[0] == 'A'){
//             // A flat is the same as G sharp, prevent negative indexing
//             if(!octave){
//                 // Return next g sharp divided by 2
//                 return freqArr[11] / 2;
//             }

//             // Otherwise, we can just step down the octave
//             return freqArr[11] * pow(2, octave - 1);
//         }

//         // Otherwise, we can just index the array as normal
//         return freqArr[toupper(note[0]) - 65 - 1] * pow(2, octave);

//     } else {
//         return 0; // Improper note
//     }
// }

// Formula taken from this wikipedia page on equal temperment: https://en.wikipedia.org/wiki/Equal_temperament
double Octave::getFreq(const char* note){
    if(strlen(note) == 0){
        return 0; // Improper note
    }

    
    char upperNote = toupper(note[0]);
    signed int noteNum = noteLookup[upperNote];

    noteNum += 12 * ((signed int)octave - (signed int)5);

    if(strlen(note) > 1){
        // May have a sharp or a flat

        if(note[1] == '#'){
            noteNum++;
        } else if(note[1] == 'b'){
            noteNum--;
        } else {
            return 0; // Improper note
        }
    }

    static double rootConst = pow(2.0, 1.0 / 12.0);

    double freq = 440.0 * pow(rootConst, noteNum);
    return freq;


    // if(strlen(note) < 2){
    //     // No flats or sharps
        
    //     return freqArr[toupper(note[0]) - 65] * pow(2, octave);
    // }

    // // Handle sharps as # and flats with b
    // if(note[1] == '#'){
    //     return freqArr[toupper(note[0]) - 65 + 1] * pow(2, octave);
    // } else if(note[1] == 'b'){
    //     if(note[0] == 'A'){
    //         // A flat is the same as G sharp, prevent negative indexing
    //         if(!octave){
    //             // Return next g sharp divided by 2
    //             return freqArr[11] / 2;
    //         }

    //         // Otherwise, we can just step down the octave
    //         return freqArr[11] * pow(2, octave - 1);
    //     }

    //     // Otherwise, we can just index the array as normal
    //     return freqArr[toupper(note[0]) - 65 - 1] * pow(2, octave);

    // } else {
    //     return 0; // Improper note
    // }
}

Octave::Octave(unsigned int octave){
    this->octave = octave;
}

Octave::Octave(signed int octave){
    if(octave < 0){
        throw "Octave count cannot be negative!";
    } else {
        this->octave = octave;
    }
}

void Octave::setOctave(unsigned int octave){
    this->octave = octave;
}