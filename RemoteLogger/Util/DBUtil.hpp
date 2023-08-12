#pragma once

#ifndef REMOTELOGGER_UTIL_DBUTIL_HPP
#define REMOTELOGGER_UTIL_DBUTIL_HPP

#include <fstream>
#include <algorithm>
#include <iostream>
#include <vector>
#include <mutex>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>

#include "blockingconcurrentqueue.h"

using cQueue = moodycamel::BlockingConcurrentQueue<std::string>;
using std::string;
using std::vector;
using std::pair;
using std::cout;

class DBUtil {
public:
    void print();
    /// @brief read the logs from the database and insert them inside the queue 
    /// @param q thread-safe and concurrent queue container
    /// @return return number of found elemnts if connected to the database else return -1
    int getLogs(cQueue &q);
    void insertLogs(vector<string> logs);
    static DBUtil *getInstance( string mongoURIStr = "mongodb://localhost:27017",
                                string dbName = "WAF",
                                string collName = "Logs");
    void operator=(const DBUtil &) = delete;
    DBUtil(DBUtil &) = delete;
private:
    void connect();
    DBUtil( string mongoURIStr,
            string dbName, 
            string collName);
    ~DBUtil();
    void createCollection(mongocxx::database& db, const string& collectionName) const;
    vector<string> getDatabases(mongocxx::client &client) const;
    void printCollection(mongocxx::collection& collection);
    void insertDocument(const bsoncxx::document::value& document);
    bsoncxx::document::value createDocument(const vector<pair<string, string>>& keyValues);
private:
    static DBUtil* instance;
    static std::mutex mut;
    string dbName;
    string collName;
    mongocxx::uri mongoURI;
    mongocxx::client conn;
    mongocxx::collection collection;
    mongocxx::database database;
};

#endif