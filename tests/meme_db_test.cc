#include "gtest/gtest.h"
#include "meme_db.h"

TEST(MemeDBTest, addPic)
{
    MemeDB database("/usr/src/projects/bigbear/build/tmp_db");
    ASSERT_TRUE(database.isConnected());
    bool success = database.Put("TestID", "pictureName", MemeDB::IMAGE);
    std::cout << "addPic test: " << database.Get("TestID", MemeDB::IMAGE) << std::endl;

    EXPECT_TRUE(success);
}

TEST(MemeDBTest, getPic)
{
    MemeDB database("/usr/src/projects/bigbear/build/tmp_db");
    std::string pic = database.Get("TestID", MemeDB::IMAGE);

    EXPECT_EQ(pic, "pictureName");
}

TEST(MemeDBTest, deletePic)
{
    MemeDB database("/usr/src/projects/bigbear/build/tmp_db");
    bool success = database.Delete("TestID", MemeDB::IMAGE);

    EXPECT_TRUE(success);
}

