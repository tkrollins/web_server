#include "meme_db.h"
#include <assert.h>
#include <iostream>
#include <boost/filesystem.hpp>
#include <map>
#include <sstream>

leveldb::DB* MemeDB::db = nullptr;
std::string MemeDB::dir = "";

// Inits only if first instance
MemeDB::MemeDB(std::string dir)
{
    if(MemeDB::db == nullptr)
    {
        init(dir);
    }
}

// Creates connection to leveldb, using dir specified
void MemeDB::init(std::string dir)
{
    MemeDB::dir = dir;
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, dir, &MemeDB::db);
    // TODO: add logging
    assert(status.ok());
}

// appends value type to key
void MemeDB::appendTypeToID(std::string &id, MemeDB::ValueType type)
{
    switch (type)
    {
        case IMAGE:
            id = id + "_id";
            break;
        case TOP_TEXT:
            id = id + "_top";
            break;
        case BOTTOM_TEXT:
            id = id + "_bottom";
            break;
        case URL:
            id = id + "_url";
            break;
        case TIMESTAMP:
            id = id + "_time";
            break;
        case ACCESS_TOKEN:
            id = id + "_tok";
            break;
        default:
            break;
    }
}

// Removes value type from id
void MemeDB::removeTypeFromID(std::string &id)
{
    const int ID_LEN = 3;
    const int TOP_LEN = 4;
    const int BOTTOM_LEN = 7;
    const int URL_LEN = 4;
    const int TIME_LEN = 5;
    const int TOK_LEN = 4;

    if(id.find("_id") != std::string::npos)
    {
        id = id.substr(0, (id.size()-ID_LEN));
    }
    else if(id.find("_top") != std::string::npos)
    {
        id = id.substr(0, (id.size()-TOP_LEN));
    }
    else if(id.find("_bottom") != std::string::npos)
    {
        id = id.substr(0, (id.size()-BOTTOM_LEN));
    }
    else if(id.find("_url") != std::string::npos)
    {
        id = id.substr(0, (id.size()-URL_LEN));
    }
    else if(id.find("_time") != std::string::npos)
    {
        id = id.substr(0, (id.size()-TIME_LEN));
    }
    else if(id.find("_tok") != std::string::npos)
    {
        id = id.substr(0, (id.size()-TOK_LEN));
    }
}

// Closes connection to leveldb
// Database info still stored
void MemeDB::close()
{
    delete MemeDB::db;
    MemeDB::db = nullptr;
}

// Removes dir used by leveldb
// This will clear the database info
void MemeDB::clear()
{
    close();
    boost::filesystem::remove_all(MemeDB::dir);
    MemeDB::dir = "";
}

// Adds id:parameter pair to db. Must specify type
bool MemeDB::Put(std::string key, std::string value, ValueType type)
{
    appendTypeToID(key, type);
    leveldb::Status status = MemeDB::db->Put(leveldb::WriteOptions(), key, value);
    // TODO: add logging
    return status.ok();
}

// Removes id:parameter pair from db.  Must specify type
bool MemeDB::Delete(std::string key, ValueType type)
{
    appendTypeToID(key, type);
    leveldb::Status status = MemeDB::db->Delete(leveldb::WriteOptions(), key);
    // TODO: add logging
    return status.ok();
}

// Returns parameter given a id. Must specify type.
// Returns "" if Get() unsuccessful
std::string MemeDB::Get(std::string key, ValueType type)
{
    appendTypeToID(key, type);
    std::string value;
    leveldb::Status status = MemeDB::db->Get(leveldb::ReadOptions(), key, &value);
    // TODO: add logging
    if(status.ok())
    {
        return value;
    }
    return "";
}

// Returns true if database has connection.
bool MemeDB::isConnected()
{
    // TODO: add better connection test
    return (MemeDB::db != nullptr);
}


// Returns set of all IDs in db
std::set<std::string> MemeDB::getIDs()
{
    std::set<std::string> IDs;
    leveldb::Iterator* it = MemeDB::db->NewIterator(leveldb::ReadOptions());
    for(it->SeekToFirst(); it->Valid(); it->Next())
    {
        std::string id = it->key().ToString();
        removeTypeFromID(id);
        IDs.insert(id);
    }
    delete it;
    return IDs;
}

std::vector<std::string> MemeDB::getSortedIDs()
{
    std::set<std::string> IDs = this->getIDs();
    std::map<std::string, std::string> mapIDTimestamp;
    std::vector<std::string> sortedIDs;

    // Create a comparator function to sort memes based on timestamp
    std::function<bool(std::pair<std::string,std::string>, std::pair<std::string, std::string>)> comparator =
            [](std::pair<std::string,std::string> meme1 ,std::pair<std::string,std::string> meme2)
            {
                int time1;
                int time2;
                std::istringstream(meme1.second) >> time1;
                std::istringstream(meme2.second) >> time2;
                return time1 > time2;
            };

    // Get all id : timestamp pairs from database
    for(const auto& id : IDs)
    {
        mapIDTimestamp.insert({id, this->Get(id, TIMESTAMP)});
    }

    // Create a set that sorts the pairs based on the comparator function
    std::set<std::pair<std::string,std::string>, std::function<bool(std::pair<std::string,std::string>, std::pair<std::string, std::string>)>>
        sortedMemes(mapIDTimestamp.begin(), mapIDTimestamp.end(), comparator);

    // Added sorted IDs to a vector
    for(const auto& meme : sortedMemes)
    {
        sortedIDs.push_back(meme.first);
    }
    return sortedIDs;
}