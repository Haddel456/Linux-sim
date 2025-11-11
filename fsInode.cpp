//
// Created by student on 8/3/23.
//

#include "fsInode.h"
#include <assert.h>


fsInode:: fsInode(int _block_size) {
    fileSize = 0;
    block_in_use = 0;
    block_size = _block_size;
    directBlock1 = -1;
    directBlock2 = -1;
    directBlock3 = -1;
    singleInDirect = -1;
    doubleInDirect = -1;

}


int fsInode:: getFileSize() {
    return fileSize;
}


// i added
int fsInode:: getBlockSize(){
    return block_size;
}

int fsInode::getBlock_in_use(){
    return block_in_use;
}


int fsInode:: getDirectBlock1(){
    return directBlock1;
}

int fsInode:: getDirectBlock2(){
    return directBlock2;
}

int fsInode:: getDirectBlock3(){
    return directBlock3;
}

int fsInode:: getSingleInDirect(){
    return singleInDirect;

}
int fsInode:: getDoubleInDirect(){
    return doubleInDirect;
}

// set function


void fsInode:: setFileSize(int sizeFile){
    this->fileSize += sizeFile;
}

void fsInode:: setToDelete(int size){
    this->fileSize=0;
}


void fsInode:: setBlock_in_use(int blockUse){
    this->block_in_use += blockUse;
}

void fsInode:: setDirectBlock1(int block1){
    this->directBlock1 = block1;

}
void fsInode:: setDirectBlock2(int block2){
    this->directBlock2 = block2;
}

void fsInode:: setDirectBlock3(int block3){
    this->directBlock3 = block3;
}

void fsInode:: setSingleInDirect(int dirBlock ){
    this->singleInDirect = dirBlock;
}

void fsInode:: setDoubleInDirect(int douBlock){
     this->doubleInDirect = douBlock;
}

//fsInode:: ~fsInode() {
//    delete directBlocks;
//}
