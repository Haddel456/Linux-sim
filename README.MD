# Linux File System Simulation in C++

## Overview

This project implements a Linux-style file system simulation, demonstrating how operating systems manage files and storage internally. It provides a block-based disk simulation with inodes, file descriptors, and core file system operations.

The project is primarily designed for educational purposes to explore file system concepts, including disk block allocation, file metadata management, and basic file operations.

## Goal

The goal of this project is to provide a hands-on simulation of low-level file system operations similar to how Linux handles files internally.

## Features

**File Management**

- Create files with unique inodes.
- Open / Close files with different modes (r, w, rw, a).
- Read / Write data with automatic block allocation.
- Append to existing files.
- Delete files safely, handling open file descriptors correctly.
- Copy & Rename files with full data integrity.

**Disk & Block Management**

- Simulated disk divided into fixed-size blocks (configurable, e.g., 512 bytes per block).
- Free block management using a bit vector.
- Automatic allocation/deallocation of disk blocks when files are written or deleted.
- Disk formatting to reset all files, inodes, and blocks.
- Inode & File Descriptor System
- fsInode class: stores file metadata including size, allocated blocks, and links.
- FileDescriptor class: tracks open file handles, mode, and current read/write position.
- fsDisk class: manages disk operations, directory structure, inodes, and file descriptors.

**Error Handling**

- Throws exceptions for:
- File not found
- File already exists
- Invalid file descriptor
- Disk full
- Invalid read/write operations

## Project Structure

- FileDescriptor — Represents open files with mode and position.
- fsInode — Tracks metadata, block allocations, and size.
- fsDisk — Core class for file operations and block management.
- main.cpp — Demonstrates creating, reading, writing, copying, renaming, and deleting files.
