//
// Created by student on 8/3/23.
//

#ifndef FSINODE_H
#define FSINODE_H

class fsInode {
    int fileSize;
    int block_in_use;

    int directBlock1;
    int directBlock2;
    int directBlock3;

    int singleInDirect;
    int doubleInDirect;
    int block_size;

public:
    fsInode(int _block_size );
   // ~fsInode();

    int getFileSize();
    int getBlockSize();
    int getBlock_in_use();

    int getDirectBlock1();
    int getDirectBlock2();
    int getDirectBlock3();

    int getSingleInDirect();
    int getDoubleInDirect();


    void setFileSize(int sizeFile);

    // i add
    void setBlock_in_use(int blockUse);
    void setDirectBlock1(int block1);
    void setDirectBlock2(int block2);
    void setDirectBlock3(int block3);
    void setSingleInDirect(int dirBlock );
    void setDoubleInDirect(int douBlock);
    void setToDelete(int size);
};


#endif //FSINODE_H