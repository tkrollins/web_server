#include "gtest/gtest.h"
#include "meme_db.h"
#include <boost/filesystem.hpp>
#include <set>
#include <vector>

TEST(MemeDBTest, addIDs)
{
    MemeDB database("/usr/src/projects/bigbear/build/tmp_db");
    ASSERT_TRUE(database.isConnected());
    bool successPic = database.Put("TestID", "pictureName", MemeDB::IMAGE);
    bool successTop = database.Put("TestID", "topText", MemeDB::TOP_TEXT);
    bool successBottom = database.Put("TestID", "bottomText", MemeDB::BOTTOM_TEXT);
    bool successTime = database.Put("TestID", "time", MemeDB::TIMESTAMP);
    bool successToken = database.Put("TestID", "token", MemeDB::ACCESS_TOKEN);

    EXPECT_TRUE(successPic);
    EXPECT_TRUE(successTop);
    EXPECT_TRUE(successBottom);
    EXPECT_TRUE(successTime);
    EXPECT_TRUE(successToken);
}

TEST(MemeDBTest, getValuesValid)
{
    MemeDB database("/usr/src/projects/bigbear/build/tmp_db");
    std::string pic = database.Get("TestID", MemeDB::IMAGE);
    std::string top = database.Get("TestID", MemeDB::TOP_TEXT);
    std::string bottom = database.Get("TestID", MemeDB::BOTTOM_TEXT);
    std::string timestamp = database.Get("TestID", MemeDB::TIMESTAMP);
    std::string token = database.Get("TestID", MemeDB::ACCESS_TOKEN);

    EXPECT_EQ(pic, "pictureName");
    EXPECT_EQ(top, "topText");
    EXPECT_EQ(bottom, "bottomText");
    EXPECT_EQ(timestamp, "time");
    EXPECT_EQ(token, "token");
}

TEST(MemeDBTest, multipleDBInstances)
{
    MemeDB database1("/usr/src/projects/bigbear/build/tmp_db");
    std::string pic1 = database1.Get("TestID", MemeDB::IMAGE);

    MemeDB database2;
    std::string pic2 = database2.Get("TestID", MemeDB::IMAGE);

    EXPECT_TRUE((pic1 == pic2) && (pic2 == "pictureName"));
}

TEST(MemeDBTest, getIDs)
{
    MemeDB database("/usr/src/projects/bigbear/build/tmp_db");
    std::set<std::string> expectedIDs = { "TestID" };
    std::set<std::string> actualIDs = database.getIDs();

    EXPECT_EQ(actualIDs, expectedIDs);
}

TEST(MemeDBTest, deleteValues)
{
    MemeDB database("/usr/src/projects/bigbear/build/tmp_db");
    bool successPic = database.Delete("TestID", MemeDB::IMAGE);
    bool successTop = database.Delete("TestID", MemeDB::TOP_TEXT);
    bool successBottom = database.Delete("TestID", MemeDB::BOTTOM_TEXT);
    bool successTime = database.Delete("TestID", MemeDB::TIMESTAMP);
    bool successToken = database.Delete("TestID", MemeDB::ACCESS_TOKEN);

    EXPECT_TRUE(successPic);
    EXPECT_TRUE(successTop);
    EXPECT_TRUE(successBottom);
    EXPECT_TRUE(successTime);
    EXPECT_TRUE(successToken);
}

TEST(MemeDBTest, getValuesInValid)
{
    MemeDB database("/usr/src/projects/bigbear/build/tmp_db");
    std::string pic = database.Get("TestID", MemeDB::IMAGE);
    std::string top = database.Get("TestID", MemeDB::TOP_TEXT);
    std::string bottom = database.Get("TestID", MemeDB::BOTTOM_TEXT);
    std::string timestamp = database.Get("TestID", MemeDB::TIMESTAMP);
    std::string token = database.Get("TestID", MemeDB::ACCESS_TOKEN);

    EXPECT_EQ(pic, "");
    EXPECT_EQ(top, "");
    EXPECT_EQ(bottom, "");
    EXPECT_EQ(timestamp, "");
    EXPECT_EQ(token, "");
    database.clear();
}

TEST(MemeDBTest, sortedIDs)
{
    MemeDB database("/usr/src/projects/bigbear/build/tmp_db");

    ASSERT_TRUE(database.getIDs().empty());

    database.Put("id1", "5", MemeDB::TIMESTAMP);
    database.Put("id2", "3", MemeDB::TIMESTAMP);
    database.Put("id3", "4", MemeDB::TIMESTAMP);
    database.Put("id4", "2", MemeDB::TIMESTAMP);
    database.Put("id5", "1", MemeDB::TIMESTAMP);

    std::vector<std::string> expectedIDOrder = {"id1", "id3", "id2", "id4", "id5"};

    std::vector<std::string> actualIDOrder = database.getSortedIDs();

    EXPECT_EQ(actualIDOrder, expectedIDOrder);
    database.clear();
}

TEST(MemeDBTest, clearDB)
{
    MemeDB database("/usr/src/projects/bigbear/build/tmp_db");
    bool success = boost::filesystem::exists("/usr/src/projects/bigbear/build/tmp_db");

    EXPECT_TRUE(success);

    database.clear();
    success = boost::filesystem::exists("/usr/src/projects/bigbear/build/tmp_db");

    EXPECT_FALSE(success);
    EXPECT_FALSE(database.isConnected());
}

