//
// Created by student on 8/3/23.
//

#ifndef FILEDESCRIPTOR_H
#define FILEDESCRIPTOR_H


#include <iostream>
#include "fsInode.h"
using namespace std;


class FileDescriptor {
    pair<string, fsInode*> file;
    bool inUse;

public:
    FileDescriptor(string FileName, fsInode* fsi);
    string getFileName();
    fsInode* getInode();
    int GetFileSize();

    bool isInUse();
    void setInUse(bool _inUse);
    void setFileName(string new_name);
    void setInode(fsInode* ind);
    void  setFileSize( int size);


    // int GetblockSize();
};



#endif //FILEDESCRIPTOR_H
