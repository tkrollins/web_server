#include "gtest/gtest.h"
#include "config_parser.h"


class NginxConfigParserTest : public ::testing::Test {
  protected:
    NginxConfigParser parser;
    NginxConfig out_config;
};

//Basic Ngnix config
TEST_F(NginxConfigParserTest, SimpleConfig) {

  bool success = parser.Parse("example_config", &out_config);

  EXPECT_TRUE(success);
}

//Try to parse non-existent file, this should fail
TEST_F(NginxConfigParserTest, FileDoesNotExist) {

  bool success = parser.Parse("does_not_exist", &out_config);

  EXPECT_TRUE(!success);
}

//Nginx config without any braces
TEST_F(NginxConfigParserTest, NoBraces) {

  bool success = parser.Parse("no_braces", &out_config);

  EXPECT_TRUE(success);
}

//Nginx config file with a missing semi-colon, this should fail
TEST_F(NginxConfigParserTest, MissingSemicolon) {

  bool success = parser.Parse("missing_semicolon", &out_config);

  EXPECT_TRUE(!success);
}

//A one line Nginx config file
TEST_F(NginxConfigParserTest, OneLine) {

  bool success = parser.Parse("one_line", &out_config);

  EXPECT_TRUE(success);
}

//A Nginx config file with just a server block
TEST_F(NginxConfigParserTest, StdServer) {

  bool success = parser.Parse("std_server", &out_config);

  EXPECT_TRUE(success);
}

//Nginx config file with nested block statements
TEST_F(NginxConfigParserTest, Nested) {

  bool success = parser.Parse("nested", &out_config);

  EXPECT_TRUE(success);
}

//Nginx config file with missing close bracket, this should fail
TEST_F(NginxConfigParserTest, BracketsUnequal) {

  bool success = parser.Parse("brackets_unequal", &out_config);

  EXPECT_TRUE(!success);
}