#include "synth.h"

int& NoteNumLookup::operator[](char index){
    return arr[toupper(index) - 65];
}

NoteNumLookup::NoteNumLookup(){
    // Do nothing
}