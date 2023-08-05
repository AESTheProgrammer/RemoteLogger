#pragma once

#ifndef REMOTELOGGER_FILEUTIL_HPP
#define REMOTELOGGER_FILEUTIL_HPP

#include <fstream>
#include <iostream>
#include "blockingconcurrentqueue.h"


using cQueue = moodycamel::BlockingConcurrentQueue<std::string>;

/// @brief read the file line by line and insert strings inside the queue (each line one string)
/// @param fileName must be text file (no binary)
/// @param q thread-safe and concurrent queue container
/// @return number of inserted lines if file was found else -1 
int readFileToQueue(const char (&fileName)[], cQueue &q);

#endif 