
//
// Created by student on 8/3/23.
//

#ifndef FSDISK_H
#define FSDISK_H

#include <iostream>
#include "fsInode.h"
#include "FileDescriptor.h"
#include <vector>
#include <map>
#include <assert.h>
#include <string.h>

#define DISK_SIZE 512

#define DISK_SIM_FILE "DISK_SIM_FILE.txt"
using namespace std;



class fsDisk {
    FILE *sim_disk_fd;

    bool is_formated;

    // BitVector - "bit" (int) vector, indicate which block in the disk is free
    //              or not.  (i.e. if BitVector[0] == 1 , means that the
    //             first block is occupied.
    int BitVectorSize;
    int *BitVector;

    // Unix directories are lists of association structures,
    // each of which contains one filename and one inode number.
    map<string, fsInode*>  MainDir ;

    // OpenFileDescriptors --  when you open a file,
    // the operating system creates an entry to represent that file
    // This entry number is the file descriptor.
    vector< FileDescriptor > OpenFileDescriptors;

    //int direct_enteris;

    int block_size;

public:
    fsDisk();    // constructor
    ~fsDisk();

    void listAll() ;      // to print the list
    void fsFormat( int blockSize =4  );
    int CreateFile(string fileName);
    int OpenFile(string FileName );
    string CloseFile(int fd) ;
    int WriteToFile(int fd, char *buf, int len );
    int ReadFromFile(int fd, char *buf, int len );
    int DelFile( string FileName );
    int CopyFile(string srcFileName, string destFileName);
    int RenameFile(string oldFileName, string newFileName);

   // int MoveFile(string srcFileName, string destFileName);
    int GetFileSize(int fd) ;



    char decToBinary(int n);
    void whichBlock(fsInode* , int);
    int numberBlock (int*);
    void writeIfZero (fsInode* , char** , int* , int);
    string readFromBlocks(fsInode* ind,int , int*);
    void  deleteToCpy (fsInode* ind );

};


#endif //FSDISK_H

