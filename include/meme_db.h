#include "leveldb/db.h"
#include <set>

class MemeDB
{
public:
    explicit MemeDB(std::string dir = "./leveldb_dir");

    // Types of parameters to add to db
    enum ValueType
    {
        IMAGE = 0,
        TOP_TEXT,
        BOTTOM_TEXT,
        URL,
        NONE
    };

    bool Put(std::string key, std::string value, ValueType type);
    bool Delete(std::string key, ValueType type);
    std::string Get(std::string key, ValueType type);
    std::set<std::string> getIDs();
    bool isConnected();
    void close();
    void clear();

private:
    static std::string dir;
    static leveldb::DB* db;

    void init(std::string dir);
    void appendTypeToID(std::string& id, ValueType type);
    void removeTypeFromID(std::string& id);
};