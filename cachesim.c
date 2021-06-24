#include <stdio.h> 
#include <stdlib.h>
#include <math.h>
#include <string.h>

// initialize file 
FILE *fr; 

// struct declaration
typedef struct tagToBlock {
    int tag; 
    int count;
    unsigned char *blockByteArray; // block defined as a byte array
} tagToBlock_t; 

// declare helper functions
int log2(int n) {
    int r=0;
    while (n>>=1) r++;
    return r;
}

int ones(int n) {
    return ((1<<n)-1);
}

// main function
int main(int argc, char *argv[]) {
    // read trace file 
    fr = fopen(argv[1], "r");
    // calculate cache properties (atoi converts a string to an int)
    int cacheSize = atoi(argv[2]) * 1024; // convert KB to B 
    int associativity = atoi(argv[3]);
    int blockSize = atoi(argv[4]);
    int setCount = cacheSize / (blockSize * associativity);
    // initialize memory array of 16 M bytes
    unsigned char *memoryByteArray = (unsigned char*) malloc(pow(2, 24) * sizeof(unsigned char));
    // block specifications
    int setIndexBits = log2(setCount); 
    int offsetBits = log2(blockSize);
    int tagBits = 24 - (setIndexBits + offsetBits);
    // initialize cache 
    int cacheEntries = setCount * associativity;
    tagToBlock_t cache[cacheEntries]; 
    for (int i = 0; i < cacheEntries; i++) {
        cache[i].tag = -1; // all cache blocks are initially invalid
        cache[i].count = i; 
        cache[i].blockByteArray = (unsigned char*) malloc(blockSize * sizeof(unsigned char));
    }
    // cache operations
    char instructionArray[10];
    while (fscanf(fr, "%s", instructionArray) != EOF){
        // obtain address 
        int address; 
        fscanf(fr, "%x", &address);
        // obtain access size
        int accessSize;
        fscanf(fr, "%d", &accessSize);
        // compute tag 
        int taglessBits = offsetBits + setIndexBits;
        int tag = address >> taglessBits;
        // compute setIndex
        int offsetlessBits = address >> offsetBits;
        int setIndexMask = ones(24) >> (offsetBits + tagBits);
        int setIndex = setIndexMask & offsetlessBits;
        // compute offSet
        int offsetMask = ones(24) >> (tagBits + setIndexBits);
        int offset = offsetMask & address;
        // define current and next
        int current = setIndex * associativity;
        int next = current + associativity;
        // if store 
        if (strcmp(instructionArray, "store") == 0){
            // paste data to be stored in a character byte array
            char storeData[accessSize*2+1];
            fscanf(fr, "%s", storeData); // scan data hex string
            char *doubleScan = storeData;
            unsigned char byteArray[accessSize]; 
            for (int i = 0; i < accessSize; i++) {
                sscanf(doubleScan, "%2hhx", &byteArray[i]); // scan two hex digits at once
                doubleScan += 2;
            }

            int byteArrayIndex = 0;
            unsigned char loadReturn[accessSize];
            bool hit = false; // initialize hit as false
            bool full = true; 

            // store into cache if present 
            for (int i = current; i < next; i++){
                if (cache[i].tag == tag){ // if the tags match
                    hit = true; // label as a hit 
                    for (int j = offset; j < (offset + accessSize); j++) {
                        cache[i].blockByteArray[j] = byteArray[byteArrayIndex];
                        byteArrayIndex++;
                    }
                    // LRU here
                    for (int l = current; l < next; l++) {
                        if (cache[l].count < cache[i].count) { // increase index of a more recently used block on a store hit or a load. 
                            cache[l].count += 1; 
                        }
                    }
                    cache[i].count = current; 
                    break;
                }
            }
            // if miss because cache isn't full and the block hasn't been brought in yet
            if(!hit) {
                for (int i = current; i < next; i++) {
                    if (cache[i].tag == -1) { // not brought in yet
                        full = false; 
                        cache[i].tag = tag;
                        // load block here
                        int loadReturnIndex = 0; 
                        if (!hit) { // if miss
                            for (int m = 0; m < blockSize; m++) { 
                                cache[i].blockByteArray[m] = memoryByteArray[m + address - offset]; // bring whole block in on a miss 
                            }
                        }
                        for (int n = offset; n < (offset + accessSize); n++) { 
                            loadReturn[loadReturnIndex] = cache[i].blockByteArray[n];
                            loadReturnIndex++;
                        }  

                        for (int j = offset; j < (offset + accessSize); j++) {
                            cache[i].blockByteArray[j] = byteArray[byteArrayIndex];
                            byteArrayIndex++;
                        }
                        // LRU here
                        for (int l = current; l < next; l++) {
                            if (cache[l].count < cache[i].count) { // increase index of a more recently used block on a store hit or a load. 
                                cache[l].count += 1; 
                            }
                        }
                        cache[i].count = current; 
                        break; 
                    }
                }
            }
            // if miss because cache is full, LRU  
            if (!hit && full) {
                for (int i = current; i < next; i++) {
                    if (cache[i].count == next - 1) {
                        cache[i].tag = tag;
                        // load block here
                        int loadReturnIndex = 0; 
                        if (!hit) { // if miss
                            for (int m = 0; m < blockSize; m++) { 
                                cache[i].blockByteArray[m] = memoryByteArray[m + address - offset]; // bring whole block in on a miss 
                            }
                        }
                        for (int n = offset; n < (offset + accessSize); n++) { 
                            loadReturn[loadReturnIndex] = cache[i].blockByteArray[n];
                            loadReturnIndex++;
                        }  

                        for (int j = offset; j < (offset + accessSize); j++) {
                            cache[i].blockByteArray[j] = byteArray[byteArrayIndex];
                            byteArrayIndex++;
                        }
                        // LRU here
                        for (int l = current; l < next; l++) {
                            if (cache[l].count < cache[i].count) { // increase index of a more recently used block on a store hit or a load. 
                                cache[l].count += 1; 
                            }
                        }
                        cache[i].count = current;  
                        break; 
                    }
                }
            } 
            
            // in any case, need to locate position in main memory and write 
            for (int i = 0; i < accessSize; i++) { 
                memoryByteArray[i + address] = byteArray[i];
            }

            // printing 
            printf("store ");
            printf("0x%x ", address);
            if (hit) printf("hit\n");
            if (!hit) printf("miss\n");
        }
        // if load 
        if (strcmp(instructionArray, "load") == 0){
            // do loads. 
            unsigned char loadReturn[accessSize];
            bool hit = false;
            bool full = true; 
            // if tag is found, then hit == true and return val
            for (int i = current; i < next; i++) {
                if (cache[i].tag == tag) { // if the tags match
                    hit = true;
                    // load block here
                    int loadReturnIndex = 0; 
                    if (!hit) { // if miss
                        for (int m = 0; m < blockSize; m++) { 
                            cache[i].blockByteArray[m] = memoryByteArray[m + address - offset]; // bring whole block in on a miss 
                        }
                    }
                    for (int n = offset; n < (offset + accessSize); n++) { 
                        loadReturn[loadReturnIndex] = cache[i].blockByteArray[n];
                        loadReturnIndex++;
                    }   
                    // LRU here
                    for (int l = current; l < next; l++) {
                        if (cache[l].count < cache[i].count) { // increase index of a more recently used block on a store hit or a load. 
                            cache[l].count += 1; 
                        }
                    }
                    cache[i].count = current; 
                    break;
                }
            }
            // if miss because cache isn't full and the block hasn't been brought in yet
            if(!hit) {
                for (int i = current; i < next; i++) {
                    if (cache[i].tag == -1) { // not brought in yet
                        full = false; 
                        cache[i].tag = tag;
                        // load block here
                        int loadReturnIndex = 0; 
                        if (!hit) { // if miss
                            for (int m = 0; m < blockSize; m++) { 
                                cache[i].blockByteArray[m] = memoryByteArray[m + address - offset]; // bring whole block in on a miss 
                            }
                        }
                        for (int n = offset; n < (offset + accessSize); n++) { 
                            loadReturn[loadReturnIndex] = cache[i].blockByteArray[n];
                            loadReturnIndex++;
                        }  
                        // LRU here
                        for (int l = current; l < next; l++) {
                            if (cache[l].count < cache[i].count) { // increase index of a more recently used block on a store hit or a load. 
                                cache[l].count += 1; 
                            }
                        }
                        cache[i].count = current; 
                        break; 
                    }
                }
            }
            // if miss because cache is full, LRU  
            if (!hit && full) {
                for (int i = current; i < next; i++) {
                    if (cache[i].count == next - 1) {
                        cache[i].tag = tag;
                        // load block here
                        int loadReturnIndex = 0; 
                        if (!hit) { // if miss
                            for (int m = 0; m < blockSize; m++) { 
                                cache[i].blockByteArray[m] = memoryByteArray[m + address - offset]; // bring whole block in on a miss 
                            }
                        }
                        for (int n = offset; n < (offset + accessSize); n++) { 
                            loadReturn[loadReturnIndex] = cache[i].blockByteArray[n];
                            loadReturnIndex++;
                        }   
                        // LRU here
                        for (int l = current; l < next; l++) {
                            if (cache[l].count < cache[i].count) { // increase index of a more recently used block on a store hit or a load. 
                                cache[l].count += 1; 
                            }
                        }
                        cache[i].count = current; 
                        break; 
                    }
                }                   
            }    
            // printing
            printf("load ");
            printf("0x%x ", address);
            if (hit) printf("hit ");
            if (!hit) printf("miss ");
            for (int i = 0; i < accessSize; i++) {
                printf("%02x", loadReturn[i]);
            }
            printf("\n");
        }
    }
    fclose(fr); // close file
    free(memoryByteArray); // free memory 
    for (int i = 0; i < setCount * associativity; i++) {
        free(cache[i].blockByteArray);
    }
    return EXIT_SUCCESS; 
}