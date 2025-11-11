//
// Created by student on 9/15/23.
//

//
// Created by student on 8/3/23.
//

#include <iostream>
#include "fsDisk.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

fsDisk :: fsDisk() {
    sim_disk_fd = fopen( DISK_SIM_FILE , "w+" );
    cout << "the sim is " <<sim_disk_fd << endl ;
    assert(sim_disk_fd);
    for (int i=0; i < DISK_SIZE ; i++) {
        int ret_val = fseek ( sim_disk_fd , i , SEEK_SET );
        ret_val = fwrite( "\0" ,  1 , 1, sim_disk_fd );
        assert(ret_val == 1);
    }
    fflush(sim_disk_fd);

    this->is_formated = false;







}

void fsDisk :: listAll() {
    int i = 0;
    for ( auto it = begin (OpenFileDescriptors); it != end (OpenFileDescriptors); ++it) {
        cout << "index: " << i << ": FileName: " << it->getFileName() <<  " , isInUse: " << it->isInUse() << " file Size: " << it->GetFileSize() << endl;
        i++;
    }
    char bufy;
    cout << "Disk content: '" ;
    for (i=0; i < DISK_SIZE ; i++) {
        int ret_val = fseek ( sim_disk_fd , i , SEEK_SET );
        ret_val = fread(  &bufy , 1 , 1, sim_disk_fd );
        cout << bufy;
    }
    cout << "'" << endl;
}


void fsDisk :: fsFormat(int blockSize ){

    if (blockSize <4){
        cerr << " EER : in block size " << endl;
        return;
    }
    this->block_size = blockSize;
    this->BitVectorSize = DISK_SIZE / blockSize;
    this->BitVector = new int[BitVectorSize];             // if i have to divide  by 32 ?
    memset(BitVector, 0, sizeof(int) * (BitVectorSize ));      //  if  does   not work will do that by loop
    this->MainDir.clear();                      // remove all the filenames and their inode
    this->OpenFileDescriptors.clear();          // remove all the opened file
    is_formated = true;
}

fsDisk :: ~fsDisk(){
    fclose(sim_disk_fd);
    delete[] BitVector;

    for (auto it = MainDir.begin(); it != MainDir.end(); ++it) {
        delete it->second;
    }
    MainDir.clear();
    this->OpenFileDescriptors.clear();
}


int  fsDisk :: CreateFile( string fileName){

    // what if there is a file with same name ?

    if (this->is_formated == false){
        cerr<< "ERR: the disk is not formatted "<< endl;
        return -1;
    }

    auto it = MainDir.find(fileName);
    if (it != MainDir.end()) {
        // File exists
        cerr << "ERR: File '" << fileName << "'  already exists.\n";
        return -1;

    }
    // File doesn't exist

    fsInode* newInode = new fsInode(this->block_size);    // Assuming you have a valid fsInode constructor.
    // which pointer i have to fill     ,    directBlock1;  directBlock2;  directBlock3;
    FileDescriptor newFile (fileName, newInode);

    // to add to the MainDir
    this->MainDir.insert({fileName,newInode});                  //  if does not work {}   replace make_pair

    // do i have to add it to the opened filed descriptor

    int fd = 0;
    bool added = false;

    for (int i = 0; i < this->OpenFileDescriptors.size(); i++) {
        if (!OpenFileDescriptors[i].isInUse()) {
            fd = i;
            this->OpenFileDescriptors[i].setFileName(fileName);
            this->OpenFileDescriptors[i].setInode(newInode);
            this->OpenFileDescriptors[i].setInUse(1);
            // this->OpenFileDescriptors.insert(OpenFileDescriptors.begin() + i, newFile);
            added = true;
            break;
        }
    }

    if (added == false) {
        this->OpenFileDescriptors.push_back(newFile);
        fd = OpenFileDescriptors.size()-1;
    }

    return fd;
}


int fsDisk :: OpenFile(string FileName ) {

    // firs i have to check if the file name is already exists in main dir

    if (this->is_formated == false){
        cerr<< "ERR: the disk is not formatted "<< endl;
        return -1;
    }

    auto it = MainDir.find(FileName);
    if (it == MainDir.end()) {
        // File doesn't exist     // that mean that i can not open!   why ?   because it is not creat
        cerr << "ERR: the file that named " << FileName<< " does not exists "<<  endl;
        return -1;
    }

    /// if the file already opened ?
    // /  just the exists file we can open

    for (int i = 0; i < this->OpenFileDescriptors.size(); i++) {
        if (OpenFileDescriptors[i].getFileName().compare(FileName) == 0) {
            if (OpenFileDescriptors[i].isInUse() ) {
                cerr << "ERR: the file that named " << FileName << " is already opened !" << endl;
                return -1;
            } else {                   //   inuse == false
                OpenFileDescriptors[i].setInUse(true);
                return i;
            }
        }
    }
    return 1;
}


string fsDisk :: CloseFile(int fd){

    if (this->is_formated == false){
        cerr<< "ERR: the disk is not formatted "<< endl;
        return "-1";
    }

    // to check that are file exists
    if (fd > OpenFileDescriptors.size() || fd < 0){
        cerr << "ERR: the file with fd " <<fd << " not exists " << endl;
    }

        // if the file is opened
    else if (OpenFileDescriptors[fd].isInUse() ){
        this->OpenFileDescriptors[fd].setInUse(false);          // i have to remove the file from OpenFileDescriptors
        ///         make another vector that contain if the file delete or not
        /// i can not use thing to make the file name null
        // return what
        return OpenFileDescriptors[fd].getFileName();
    }

    else if (!OpenFileDescriptors[fd].isInUse()){
        // if the file is closed     not opened
        cerr << "ERR: the file with fd  " << fd << " already closed !" << endl;
    }

    return "-1";
}

int fsDisk :: WriteToFile(int fd, char *buf, int len ) {

    if (this->is_formated == false){
        cerr<< "ERR: the disk is not formatted "<< endl;
        return -1;
    }

    // to check if the fd is legal
    if (fd > OpenFileDescriptors.size() || fd < 0) {
        cerr << "ERR: the file with fd " << fd << " not exists " << endl;
        return -1;
    }

    // to check if the desk formatted
    if (!this->is_formated) {
        cerr << "ERR: the disk is not formatted " << endl;
        return -1;
    }

    // to  check if the opened
    if (!(this->OpenFileDescriptors[fd].isInUse())) {
        cerr << "ERR: the file with fd " << fd << " is not opened " << endl;
        return -1;
    }

    int remainingBlock = 0;
    for (int i = 0; i < this->BitVectorSize; i++) {
        if (this->BitVector[i] == 0)
            remainingBlock++;
    }

    int dataBlock_num = 0;
    int num_block = 0;
    int offset=0;
    fsInode *file_inode = OpenFileDescriptors[fd].getInode();


    if (file_inode->getBlock_in_use() == 0) {    // that mean the file size is 0 ---> there is no blocks in use --> to know the number of block that need

        dataBlock_num = len / block_size;
        if (len % block_size != 0)
            dataBlock_num++;    // Add one more block if there's a partial block

//        if (dataBlock_num > remainingBlock) {
//            cerr << " there is no more places in the disk " << endl;
//            return -1;
//        }
        // to get this block
        for (int i = 0; i < dataBlock_num; i++) {

            num_block = numberBlock(&remainingBlock);
            if (num_block != -1) {
                char inside;
                char in_inside;

                if (file_inode->getBlock_in_use() == 3) {
                    inside = decToBinary(numberBlock(&remainingBlock));
                    fseek(sim_disk_fd, num_block * block_size, SEEK_SET);
                    fwrite(&inside, sizeof(char), 1, sim_disk_fd);
                } else if (file_inode->getBlock_in_use() == 4) {
                    inside = decToBinary(numberBlock(&remainingBlock));
                    in_inside = decToBinary(numberBlock(&remainingBlock));

                    fseek(sim_disk_fd, num_block * block_size, SEEK_SET);
                    fwrite(&inside, sizeof(char), 1, sim_disk_fd);

                    fseek(sim_disk_fd, inside * block_size, SEEK_SET);
                    fwrite(&in_inside, sizeof(char), 1, sim_disk_fd);
                }

                whichBlock(file_inode, num_block);
                writeIfZero(file_inode,&buf, &len ,offset );
            }
        }

        // to write to this block

    } else {
        // file size is not 0 --> there is a block in use
        int remainingSpace =block_size - (file_inode->getFileSize() % block_size);      // check if there space in the last block
        offset = (file_inode->getFileSize() % block_size);
        if (remainingSpace == block_size) {
            remainingSpace = 0;
        }

        dataBlock_num = (len - remainingSpace) / block_size;
        if ((len - remainingSpace) % block_size)
            dataBlock_num++;

        // to write to last block that have space
        if (remainingSpace != 0)
            writeIfZero(file_inode, &buf, &remainingSpace, offset);

        if (buf[0] != '\0'){
            offset=0;
            len = len - remainingSpace;

            for (int i = 0; i < dataBlock_num; i++) {
                num_block = numberBlock(&remainingBlock);

                if (num_block != -1) {
                    char inside;
                    char in_inside;

                    if (file_inode->getBlock_in_use() == 3) {
                        inside = decToBinary(numberBlock(&remainingBlock));
                        fseek(sim_disk_fd, num_block * block_size, SEEK_SET);
                        fwrite(&inside, sizeof(char), 1, sim_disk_fd);
                    } else if (file_inode->getBlock_in_use() == 4) {
                        inside = decToBinary(numberBlock(&remainingBlock));
                        in_inside = decToBinary(numberBlock(&remainingBlock));

                        fseek(sim_disk_fd, num_block * block_size, SEEK_SET);
                        fwrite(&inside, sizeof(char), 1, sim_disk_fd);

                        fseek(sim_disk_fd, inside * block_size, SEEK_SET);
                        fwrite(&in_inside, sizeof(char), 1, sim_disk_fd);
                    }

                    whichBlock(file_inode, num_block);
                    writeIfZero(file_inode, &buf, &len,offset );
                }

            }

        }
    }
    return 1;
}


int fsDisk :: ReadFromFile(int fd, char *buf, int len ){


    // if it formatted
    if (!this->is_formated ){
        cerr << "ERR: the disk is not formatted "<< endl;
        return -1;
    }

    // if  the file not exist
    if (fd > OpenFileDescriptors.size() || fd < 0 )
    {
        cerr << "ERR: the file with fd " <<fd << " not exists " << endl;
        return -1;
    }

    // if it opened
    if(!(this->OpenFileDescriptors[fd].isInUse())){
        cerr << "ERR: the the file is not opened "<< endl;
        return -1;
    }

    int BlockNum_toRead = 0;
    string str = "";
    memset(buf, '\0', sizeof(buf));
    fsInode *file_inode = OpenFileDescriptors[fd].getInode();

    if(len < file_inode->getFileSize()) {
        BlockNum_toRead = len / block_size;
        if (len % block_size != 0)
            BlockNum_toRead++;
        // to call the function
        for (int i =1 ; i <= BlockNum_toRead ; i++) {
            str = readFromBlocks(file_inode, i, &len);
            strcat(buf, str.c_str());

        }

    }
    else{

        BlockNum_toRead = file_inode->getFileSize() /block_size ;
        if (file_inode->getFileSize() % block_size != 0)
            BlockNum_toRead++;

        for (int i =1 ; i <= BlockNum_toRead ; i++) {
            str = readFromBlocks(file_inode, i, &len);
            strcat(buf, str.c_str());
        }
    }

    return 1;
}


int fsDisk :: DelFile( string FileName ) {
    // to delete from main dir
    // to delete  from  open file dic
    // to delete the inode also
    // check that the file is exists

    if (this->is_formated == false){
        cerr<< "ERR: the disk is not formatted "<< endl;
        return -1;
    }

    int fd_index=0;
    auto it = MainDir.find(FileName);
    if (it == MainDir.end()) {
        // File doesn't exist     // that mean that i can not open!   why ?   because it is not creat
        cerr << "ERR: the file that named " << FileName << " does not exists " << endl;
        return -1;
    }

    ////

    // if the filed is open -> print error  // the close delete it from open file
    for (int i = 0; i < this->OpenFileDescriptors.size(); i++) {
        if (OpenFileDescriptors[i].getFileName().compare(FileName) == 0) {
            if (OpenFileDescriptors[i].isInUse() ) {
                cerr << "Error: the file is opened you have to close it  first" << endl;
                return -1;
            }
            else {
                fd_index = i;
                break;
            }
        }
    }

    int in = '\0';
    int num =0;
    fsInode *file_inode = OpenFileDescriptors[fd_index].getInode();
    file_inode->setToDelete(0);

    /// to delete direct  block



    if (file_inode->getDirectBlock1() != -1){
        BitVector[file_inode->getDirectBlock1()]= 0;
        file_inode->setDirectBlock1(-1);
        file_inode->setBlock_in_use(-1);
    }

    if (file_inode->getDirectBlock2() != -1){
        BitVector[file_inode->getDirectBlock2()]= 0;
        file_inode->setDirectBlock2(-1);
        file_inode->setBlock_in_use(-1);
    }

    if (file_inode->getDirectBlock3() != -1){
        BitVector[file_inode->getDirectBlock3()]= 0;
        file_inode->setDirectBlock3(-1);
        file_inode->setBlock_in_use(-1);
    }
    /// SingleInDirect
    if (file_inode->getSingleInDirect() != -1){

        fseek(sim_disk_fd,  file_inode->getSingleInDirect()* block_size, SEEK_SET);
        fread(&in,1, 1, sim_disk_fd);

        num = static_cast<int>(in);
        BitVector[num] = 0;
        BitVector[file_inode->getSingleInDirect()]= 0;
        file_inode->setSingleInDirect(-1);
        file_inode->setBlock_in_use(-1);
    }
    //// doubleInDirect
    if (file_inode->getDoubleInDirect() != -1){

        fseek(sim_disk_fd,  file_inode->getDoubleInDirect()* block_size, SEEK_SET);
        fread(&in,1, 1, sim_disk_fd);

        num = static_cast<int>(in);
        in ='\0';

        fseek(sim_disk_fd, num * block_size, SEEK_SET);
        fread(&in,1, 1, sim_disk_fd);
       int num_single =  static_cast<int>(in);

        BitVector[num_single] = 0;
        BitVector[num] = 0;
        BitVector[file_inode->getDoubleInDirect()]= 0;
        file_inode->setDoubleInDirect(-1);

        file_inode->setBlock_in_use(-1);
    }


    OpenFileDescriptors[fd_index].setInUse(false);
    OpenFileDescriptors[fd_index].setFileName(" ");
    OpenFileDescriptors[fd_index].getInode()->setToDelete(0);
    MainDir.erase(it);


    /// it will contain the index o
    return 1;
}

int fsDisk :: RenameFile(string oldFileName, string newFileName) {

    if (this->is_formated == false){
        cerr<< "ERR: the disk is not formatted "<< endl;
        return -1;
    }

    auto it = MainDir.find(oldFileName);
    if (it == MainDir.end()) {
        // File doesn't exist     // that mean that i can not open!   why ?   because it is not creat
        cerr << " the file that named " << oldFileName<< " does not exists "<<  endl;
        return -1;
    }

    // if the file is open
    for (int i=0 ; i<OpenFileDescriptors.size() ; i++){
        if (OpenFileDescriptors[i].getFileName().compare(oldFileName) == 0){
            if(OpenFileDescriptors[i].isInUse()){
                cerr << " the file " <<  oldFileName<< " is open "<<  endl;
                return -1;
            } else {
                OpenFileDescriptors[i].setFileName(newFileName);
                fsInode* inodePtr = it->second;
                MainDir.erase(it);
                MainDir[newFileName] = inodePtr;
                break;
            }
        }
    }
    return 1;
}


int fsDisk :: CopyFile(string srcFileName, string destFileName) {

    if (this->is_formated == false){
        cerr<< "ERR: the disk is not formatted "<< endl;
        return -1;
    }

    int fd_src;
    if (srcFileName.compare(destFileName) == 0) {
        cerr << "ERR : its the same name !" << endl;
    }

    auto it = MainDir.find(srcFileName);
    if (it == MainDir.end()) {
        // File doesn't exist     // that mean that i can not open!   why ?   because it is not creat
        cerr << "ERR : the file that named " << srcFileName << " does not exists " << endl;
        return -1;
    }

    for (int i = 0; i < OpenFileDescriptors.size(); i++) {
        if (OpenFileDescriptors[i].getFileName().compare(srcFileName) == 0) {
            if (OpenFileDescriptors[i].isInUse()) {
                cerr << "ERR: the file " << srcFileName << " is open " << endl;
                return -1;
            } else {
                fd_src=i;
                break;}
        }
    }

    char str[it->second->getFileSize()];
    int fd_dest;

    auto lt = MainDir.find(destFileName);
    if (lt != MainDir.end()) {
        // the file  exists  --->> to check if it opened
        for (int i = 0; i < OpenFileDescriptors.size(); i++) {
            if (OpenFileDescriptors[i].getFileName().compare(destFileName) == 0) {
                if (OpenFileDescriptors[i].isInUse()) {
                    cerr << "ERR: the file " << destFileName << " is open " << endl;
                    return -1;
                } else {
                    /// to copy the inode
                    fd_dest = i;
                    break;
                }}
        }
        OpenFileDescriptors[fd_src].setInUse(true);
        ReadFromFile(fd_src,str,it->second->getFileSize());
        OpenFileDescriptors[fd_src].setInUse(false);

        OpenFileDescriptors[fd_dest].setInUse(true);
        deleteToCpy (OpenFileDescriptors[fd_dest].getInode());
        WriteToFile(fd_dest,str,it->second->getFileSize());
        OpenFileDescriptors[fd_dest].setInUse(false);


    }
    else {
        fsInode* newInode = new fsInode(this->block_size);
        FileDescriptor newFile (destFileName, newInode);

        this->MainDir.insert({destFileName,newInode});

        OpenFileDescriptors[fd_src].setInUse(true);
        ReadFromFile(fd_src,str,it->second->getFileSize());

        bool added = false;

        for (int i = 0; i < this->OpenFileDescriptors.size(); i++) {
            if (!OpenFileDescriptors[i].isInUse()) {
                fd_dest = i;
                this->OpenFileDescriptors[i].setFileName(destFileName);
                this->OpenFileDescriptors[i].setInode(newInode);
                added = true;
                break;
            }
        }

        if (added == false) {
            this->OpenFileDescriptors.push_back(newFile);
            fd_dest = OpenFileDescriptors.size()-1;
        }

        OpenFileDescriptors[fd_src].setInUse(false);

        OpenFileDescriptors[fd_dest].setInUse(true);
        WriteToFile(fd_dest,str,it->second->getFileSize());
        OpenFileDescriptors[fd_dest].setInUse(false);

    }
    return 1;
}


void fsDisk :: whichBlock( fsInode* ind , int i){     // to sent i also

    if( ind->getBlock_in_use() < 3){
        if ( ind->getBlock_in_use()==0 ) {        // block_in_use = 0
            ind->setDirectBlock1(i);
            ind->setBlock_in_use(1);
        }
        else if (ind->getBlock_in_use()==1){       // ind->getDirectBlock1() == -1 i think no need
            ind->setDirectBlock2(i);
            ind->setBlock_in_use(1);
        }
        else {
            ind->setDirectBlock3(i);
            ind->setBlock_in_use(1);
        }
    } else if ( ind->getBlock_in_use() == 3){
        // set signal in directed
        ind->setSingleInDirect(i);
        ind->setBlock_in_use(1);

    } else if (ind->getBlock_in_use() == 4 ){
        // set double in directer
        ind->setDoubleInDirect(i);
        ind->setBlock_in_use(1);

    } else {
        cerr << "all the block filed used "<< endl;
    }
}


char fsDisk :: decToBinary(int n) {
    return static_cast<char>(n);
}

int  fsDisk :: numberBlock(int* not_use_block) {

    for (int i = 0; i < BitVectorSize; i++) {
        if (BitVector[i] == 0) {
            BitVector[i] = 1;
            (*(not_use_block))--;
            return i;
        }
    }
    return -1;
}


void fsDisk :: writeIfZero (fsInode* ind , char** text , int* len , int off){

    char block;

    if (ind->getBlock_in_use() == 1 ) {        // block_in_use = 0
        fseek(sim_disk_fd, (ind->getDirectBlock1() * block_size )+ off, SEEK_SET);


        if (*len >block_size ) {
            fwrite((*text),sizeof (char), block_size, sim_disk_fd);
            ind->setFileSize(block_size);
            (*text)= (*text)+block_size ;
            *len = *len-block_size;
        }

        else {
            fwrite((*text),sizeof (char), (*len), sim_disk_fd);
            ind->setFileSize(*len);
            (*text) = *text+(*len) ;
        }

    }
    else if (ind->getBlock_in_use() == 2) {

        fseek(sim_disk_fd, (ind->getDirectBlock2() * block_size) + off, SEEK_SET);

        if (*len > block_size) {
            fwrite((*text), sizeof(char), block_size, sim_disk_fd);
            ind->setFileSize(block_size);
            (*text) = (*text) + block_size;
            (*len) = (*len) - block_size;
        } else {
            fwrite((*text),sizeof (char), (*len), sim_disk_fd);
            ind->setFileSize(*len);
            (*text) = (*text) + (*len);
        }
    }
    else if (ind->getBlock_in_use() == 3) {

        fseek(sim_disk_fd, ( ind->getDirectBlock3() * block_size) + off, SEEK_SET);

        if (*len > block_size) {
            fwrite((*text),sizeof (char), block_size, sim_disk_fd);
            ind->setFileSize(block_size);
            (*text) = (*text) + block_size;
            (*len) = (*len) - block_size;
        } else {
            fwrite((*text),sizeof (char), (*len), sim_disk_fd);
            ind->setFileSize(*len);
            (*text) = (*text) + (*len);
        }
    }
    else if(ind->getBlock_in_use() == 4) {

        int num;
        fseek(sim_disk_fd, ind->getSingleInDirect() * block_size, SEEK_SET);
        fread(&block,1, 1, sim_disk_fd);
        num = static_cast<int>(block);

        fseek(sim_disk_fd, (num * block_size) + off, SEEK_SET);

        if (*len > block_size) {
            fwrite((*text) ,sizeof (char), block_size, sim_disk_fd);
            ind->setFileSize(block_size);
            (*text) = (*text) + block_size;
            (*len) = (*len) - block_size;
        } else {
            fwrite((*text) ,sizeof (char), (*len), sim_disk_fd);
            ind->setFileSize(*len);
            (*text) = (*text) + (*len);
        }
    }
    else if(ind->getBlock_in_use() == 5) {
        int num1;
        int num2;
        fseek(sim_disk_fd, ind->getDoubleInDirect() * block_size, SEEK_SET);
        fread(&block, 1, 1, sim_disk_fd);
        num1 = static_cast<int>(block);

        fseek(sim_disk_fd, num1 * block_size, SEEK_SET);
        fread(&block, 1, 1, sim_disk_fd);
        num2 = static_cast<int>(block);

        fseek(sim_disk_fd, num2 * block_size, SEEK_SET);

        if (*len > block_size) {
            fwrite((*text), sizeof(char), block_size, sim_disk_fd);
            ind->setFileSize(block_size);
            (*text) = (*text) + block_size;
            (*len) = (*len) - block_size;
        } else {
            fwrite((*text), sizeof(char), (*len), sim_disk_fd);
            ind->setFileSize(*len);
            (*text) = (*text) + (*len);
        }
    }

}


string fsDisk :: readFromBlocks(fsInode* ind,int numBlock , int* len){

    char* buffer = new char [block_size];
    char block ='\0';

    string str="";
    memset(buffer, '\0', sizeof(buffer));

    if (numBlock == 1){
        fseek(sim_disk_fd, ind->getDirectBlock1() * block_size, SEEK_SET);

        if(*len < block_size){
            fread(buffer, *len, 1, sim_disk_fd);}
        else {
            fread(buffer, block_size, 1, sim_disk_fd);
            (*len) = (*len) - block_size;
        }
    }

    else if (numBlock == 2){

        fseek(sim_disk_fd, ind->getDirectBlock2() * block_size, SEEK_SET);

        if ((*len) < block_size)
            fread(buffer,(*len), 1, sim_disk_fd);          /// if read have to change

        else{

            fread(buffer, block_size, 1, sim_disk_fd);
            (*len) = (*len) - block_size;
        }
    }
    else if (numBlock == 3) {

        fseek(sim_disk_fd, ind->getDirectBlock3() * block_size, SEEK_SET);

        if ((*len) < block_size)
            fread(buffer,(*len), 1, sim_disk_fd);          /// if read have to change

        else{
            fread(buffer, block_size, 1, sim_disk_fd);
            (*len) = (*len) - block_size;
        }
    }

    else if (numBlock == 4){

        int num ;

        fseek(sim_disk_fd,  ind->getSingleInDirect()* block_size, SEEK_SET);
        fread(&block,1, 1, sim_disk_fd);

        num = static_cast<int>(block);

        fseek(sim_disk_fd, num * block_size, SEEK_SET);

        if ((*len) < sizeof (buffer))
            fread(buffer,(*len), 1, sim_disk_fd);          /// if read have to change

        else{
            fread(buffer, block_size, 1, sim_disk_fd);
            (*len) = (*len) - block_size;
        }
    }

    else if (numBlock == 5){

        int num;
        int num_single;

        fseek(sim_disk_fd,  ind->getDoubleInDirect()* block_size, SEEK_SET);
        fread(&block,1, 1, sim_disk_fd);

        num = static_cast<int>(block);
        block ='\0';

        fseek(sim_disk_fd, num * block_size, SEEK_SET);
        fread(&block,1, 1, sim_disk_fd);
        num_single =  static_cast<int>(block);

        fseek(sim_disk_fd, num_single * block_size, SEEK_SET);

        if ((*len) < block_size)
            fread(buffer,(*len), 1, sim_disk_fd);          /// if read have to change

        else{
            fread(buffer, block_size, 1, sim_disk_fd);
            (*len) = (*len) - block_size;
        }
    }

    str+=buffer;
    delete [] buffer;
    return str;

}


void fsDisk :: deleteToCpy (fsInode* ind ){


    int in = '\0';
    int num =0;
    ind->setToDelete(0);

    /// to delete direct  block
    if (ind->getDirectBlock1() != -1){
        BitVector[ind->getDirectBlock1()]= 0;
        ind->setDirectBlock1(-1);
        ind->setBlock_in_use(-1);
    }

    if (ind->getDirectBlock2() != -1){
        BitVector[ind->getDirectBlock2()]= 0;
        ind->setDirectBlock2(-1);
        ind->setBlock_in_use(-1);
    }

    if (ind->getDirectBlock3() != -1){
        BitVector[ind->getDirectBlock3()]= 0;
        ind->setDirectBlock3(-1);
        ind->setBlock_in_use(-1);
    }
    /// SingleInDirect
    if (ind->getSingleInDirect() != -1){

        fseek(sim_disk_fd,  ind->getSingleInDirect()* block_size, SEEK_SET);
        fread(&in,1, 1, sim_disk_fd);

        num = static_cast<int>(in);
        BitVector[num] = 0;
        BitVector[ind->getSingleInDirect()]= 0;
        ind->setSingleInDirect(-1);
        ind->setBlock_in_use(-1);
    }
    //// doubleInDirect
    if (ind->getDoubleInDirect() != -1){

        fseek(sim_disk_fd,  ind->getDoubleInDirect()* block_size, SEEK_SET);
        fread(&in,1, 1, sim_disk_fd);

        num = static_cast<int>(in);
        in ='\0';

        fseek(sim_disk_fd, num * block_size, SEEK_SET);
        fread(&in,1, 1, sim_disk_fd);
        int num_single =  static_cast<int>(in);

        BitVector[num_single] = 0;
        BitVector[num] = 0;
        BitVector[ind->getDoubleInDirect()]= 0;
        ind->setDoubleInDirect(-1);

        ind->setBlock_in_use(-1);
    }
}


int fsDisk :: GetFileSize(int fd){
    return OpenFileDescriptors[fd].GetFileSize();
}