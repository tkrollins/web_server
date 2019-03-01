#include "gtest/gtest.h"
#include "meme_db.h"
#include <boost/filesystem.hpp>
#include <set>

TEST(MemeDBTest, addIDs)
{
    MemeDB database("/usr/src/projects/bigbear/build/tmp_db");
    ASSERT_TRUE(database.isConnected());
    bool successPic = database.Put("TestID", "pictureName", MemeDB::IMAGE);
    bool successTop = database.Put("TestID", "topText", MemeDB::TOP_TEXT);
    bool successBottom = database.Put("TestID", "bottomText", MemeDB::BOTTOM_TEXT);

    EXPECT_TRUE(successPic);
    EXPECT_TRUE(successTop);
    EXPECT_TRUE(successBottom);
}

TEST(MemeDBTest, getValuesValid)
{
    MemeDB database("/usr/src/projects/bigbear/build/tmp_db");
    std::string pic = database.Get("TestID", MemeDB::IMAGE);
    std::string top = database.Get("TestID", MemeDB::TOP_TEXT);
    std::string bottom = database.Get("TestID", MemeDB::BOTTOM_TEXT);

    EXPECT_EQ(pic, "pictureName");
    EXPECT_EQ(top, "topText");
    EXPECT_EQ(bottom, "bottomText");
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

    EXPECT_TRUE(successPic);
    EXPECT_TRUE(successTop);
    EXPECT_TRUE(successBottom);
}

TEST(MemeDBTest, getValuesInValid)
{
    MemeDB database("/usr/src/projects/bigbear/build/tmp_db");
    std::string pic = database.Get("TestID", MemeDB::IMAGE);
    std::string top = database.Get("TestID", MemeDB::TOP_TEXT);
    std::string bottom = database.Get("TestID", MemeDB::BOTTOM_TEXT);

    EXPECT_EQ(pic, "");
    EXPECT_EQ(top, "");
    EXPECT_EQ(bottom, "");
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

