//
// Created by student on 8/3/23.
//

#include "FileDescriptor.h"

FileDescriptor:: FileDescriptor(string FileName, fsInode* fsi) {
    file.first = FileName;
    file.second = fsi;
    inUse = true;
}


string FileDescriptor :: getFileName(){
    return file.first;
}

fsInode* FileDescriptor :: getInode() {
    return file.second;
}


bool FileDescriptor :: isInUse() {
    return (inUse);
}
void FileDescriptor :: setInUse(bool _inUse) {
    inUse = _inUse ;
}

void  FileDescriptor :: setFileName(string new_name){
    file.first = new_name;
}


int  FileDescriptor :: GetFileSize() {
    return file.second->getFileSize();
}

void  FileDescriptor :: setFileSize( int size) {
    file.second->setToDelete(0);
}



void FileDescriptor :: setInode(fsInode* ind){
    file.second = ind;
}

