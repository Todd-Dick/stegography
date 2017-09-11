#include <string>
#include <bitset>
#include <iostream>
#include <vector>
#include <cstring>
#include <ctime>
#include <cstdint>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>


using namespace std;

string getMessage ();
void encodeMessage(string message, const char* fileName);
void decodeMessage(const char* fileName);

struct WAVE_HEADER {
    uint8_t RIFF[4];   //should be "RIFF"
    uint32_t chunkSize;     //RIFF chunk size
    uint8_t WAVE[4];   //should be "WAVE"
    uint8_t fmt[4];    //should be "fmt "
    uint32_t SubChunkSize;  //fmt chunk size
    uint16_t format;        //should be 1
    uint16_t numChannels;   //should be 1
    uint32_t frequency;     //should be 22050
    uint32_t bytesPerSec;
    uint16_t blockalign;
    uint16_t bitsPerSample;
    uint8_t data[4];    //should be "data"
    uint32_t dataSize;
};


int main(int argc, char ** argv) {
    //check file name
    const char* fileName;
    if (argc !=2) {
        cout << "Usage: ./\"executable name\" \"name of .wav file\"" << endl;
        cout << "Restart the program\n";
        exit(1);
    }
    else {
        fileName = argv[1];
    }
    cout << "Option 1: encode a message in your .wav file.\n";
    cout << "Option 2: decode a message from your .wav file.\n";
    cout << "Choose an option (1 or 2): ";
    int choice;
    cin >> choice;
    if(choice == 1) {
        string message = getMessage();
        encodeMessage(message, fileName);
    }
    else if(choice ==2) {
        decodeMessage(fileName);
    }
    else{
        cout << "Invalid choice: restart the program\n";
        exit(1);
    }
    //cout << message << endl;
    return 0;
}

string getMessage () {
    string message;
    cout << "Enter message to encode: ";
    cin.ignore();
    getline(cin, message);
    return message;
}


void encodeMessage (string message, const char* fileName) {
    int bits[8] = {0,0,1,1,0,0,0,1};
    unsigned char a = '0';
    for (int i = 0; i < 8; ++i) {
        a=a<<1;
        a+=bits[i];
        //cout << "a = " << a << endl;
    }
    //cout << "a = " << a << endl;
    // encode the message
    vector<int> encoded;
    cout << "You chose to encode: " << message << endl;
    cout << "This message will be encoded into: " << fileName << endl;
    const char *temp = message.c_str();
    int bit_index;
    for (bit_index = sizeof(*temp)*8 - 1; bit_index >= 0; --bit_index) {
        unsigned int bit = *temp >> bit_index & 1;
        encoded.push_back(bit);
    }
    char newLine [] = {0,0,0,0,1,0,1,0};
    for(int i = 0; i < 8; i++) {
        encoded.push_back(newLine[i]);
    }
    for(int i = 0; i < sizeof(encoded); i++) {
        //cout << encoded[i] << endl;
    }
    //cout << "size of encoded is " << sizeof(encoded) << endl;
    
    // set up header, open the file, read past the file, also check the file for my parameters
    //some useful variables
    WAVE_HEADER header;
    FILE *theFile;
    //fileShit = fileName.c_str();
    uint32_t headerSize;
    
    //open the file
    //theFile = fopen(fileName, "rb" "wb")
    theFile = fopen(fileName, "rb+");
    if(theFile == NULL) {
        cout << "Put your file in this folder and restart the program\n";
        exit(1);
    }
    
    headerSize = sizeof(header);
    size_t bytesRead = fread (&header,1,headerSize,theFile);
    
    // test to see we can handle the input file
    if (header.bitsPerSample != 16 || header.numChannels != 1){
        printf("This program won't properly process your file.\n Please use a file with 16 bit samples recorded in mono.\n");
        exit(1);
    }
    //fclose(theFile);
    
    uint32_t sampcount = 0;
    uint16_t sample;
    //theFile = fopen(fileName, "wb");
    fseek(theFile,headerSize,SEEK_SET);
    // read and modify
    while (sampcount < sizeof(encoded)) {
        size_t readStuff = fread(&sample,sizeof(uint16_t),1,theFile);
        //cout << "sample = " << sample << endl;
        unsigned int b = encoded[sampcount];
        //cout << "b = " << b << endl;
        sample = ((sample & ~1) | b);
        //cout << "Sample = " << sample << endl;
        fwrite(&sample,sizeof(uint16_t),1,theFile);
        sampcount++;
    }
    //fseek(theFile,0,SEEK_END);
    fclose(theFile);
    cout << "done" << endl;
}

void decodeMessage (const char* fileName) {
    cout << "You chose to decode: " << fileName << endl;
    cout << "The message is: \n";
    vector<int> temp;
    WAVE_HEADER header;
    FILE *theFile;
    //fileName = fileName.c_str();
    uint32_t headerSize;
    
    theFile = fopen(fileName, "rb");
    if(!theFile) {
        cout << "Error opening the file. Make sure it's in this folder." << endl;
        exit(1);
    }
    bool endLineFound = false;
    uint32_t sampcount = 0;
    headerSize = sizeof(header);
    fseek(theFile,headerSize,SEEK_SET);
    uint16_t sample;
    while(!endLineFound) {
        size_t readStuff = fread(&sample,sizeof(uint16_t),1,theFile);
        //cout << "sample = " << sample << endl;
        int LSB = sample & 1;
        //cout << "LSB = " << LSB << endl;
        temp.push_back(LSB);
        sampcount++;
        int checkNum = sampcount%8;
        if(checkNum == 0) {
            //cout << "if triggered" << endl;
            //cout << temp[j];
            unsigned char c = '0';
            int bits [8];
            int j = sampcount -8;
            for(int i = 0; i < 8; ++i) {
                bits[i] = (unsigned int)temp[j];
                
                //cout << "i = " << i << ". temp[j] = " << temp[j] << ". j = " << j << ". bits[i] = " << bits[i] << "." << endl;
                //cout << "second for entered" << endl;
                c=c<<1;
                c+=bits[i];
                
                if(c=='\n') {
                    endLineFound = true;
                    //cout << "End found" << endl;
                }
                ++j;
            }
            cout << c;
        }
    }
    cout << "closing file" << endl;
    fclose(theFile);
    cout << endl;
}

