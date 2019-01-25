#include "gtest/gtest.h"
#include "config_parser.h"


class NginxConfigParserTest : public ::testing::Test {
  protected:
    NginxConfigParser parser;
};

// Basic Ngnix config
TEST_F(NginxConfigParserTest, SimpleConfig) {

  bool success = parser.Parse("example_config");

  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, ParseListenPort) {

    bool success = parser.Parse("example_config");
    std::string port = parser.config->parameters[configParameter::LISTEN_PORT];

    EXPECT_TRUE(success);
    EXPECT_TRUE(port == "80");
}

// Try to parse non-existent file, this should fail
TEST_F(NginxConfigParserTest, FileDoesNotExist) {

  bool success = parser.Parse("does_not_exist");

  EXPECT_FALSE(success);
}

// Nginx config without any braces
TEST_F(NginxConfigParserTest, NoBraces) {

  bool success = parser.Parse("no_braces");

  EXPECT_TRUE(success);
}

// Nginx config file with a missing semi-colon, this should fail
TEST_F(NginxConfigParserTest, MissingSemicolon) {

  bool success = parser.Parse("missing_semicolon");

  EXPECT_FALSE(success);
}

// A one line Nginx config file
TEST_F(NginxConfigParserTest, OneLine) {

  bool success = parser.Parse("one_line");

  EXPECT_TRUE(success);
}

// A Nginx config file with just a server block
TEST_F(NginxConfigParserTest, StdServer) {

  bool success = parser.Parse("std_server");

  EXPECT_TRUE(success);
}

// Nginx config file with nested block statements
TEST_F(NginxConfigParserTest, Nested) {

  bool success = parser.Parse("nested");

  EXPECT_TRUE(success);
}

// Nginx config file with missing close bracket, this should fail
TEST_F(NginxConfigParserTest, BracketsUnequal) {

  bool success = parser.Parse("brackets_unequal");

  EXPECT_FALSE(success);
}