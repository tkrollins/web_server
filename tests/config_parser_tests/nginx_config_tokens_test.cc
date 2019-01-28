#include "nginx_config_tokens.h"
#include <assert.h>
#include "gtest/gtest.h"
#include <fstream>

bool compareTokens(Token* tkn1, Token* tkn2)
{
    assert(tkn1 != nullptr && tkn2 != nullptr);
    if(tkn1->value != tkn2->value)
    {
        return false;
    }
    if(tkn1->type != tkn2->type)
    {
        return false;
    }
    return true;
}

bool compareTokenLists(std::vector<Token*> list1, std::vector<Token*> list2)
{
    if(list1.size() != list2.size())
    {
        return false;
    }
    for(int i = 0; i < list1.size(); i++)
    {
        if(!compareTokens(list1[i], list2[i]))
        {
            return false;
        }
    }
    return true;
}

class NginxConfigTokensTest : public ::testing::Test {
protected:
     std::ifstream input;
     NginxConfigTokens* tokenList;
};

// Compares generated token list from "example_config"
// to expected list
TEST_F(NginxConfigTokensTest, ValidConfig)
{
    input.open("example_config");
    tokenList = NginxConfigTokens::makeNginxConfigTokens(&input);
    ASSERT_TRUE(tokenList != nullptr);
    std::vector<Token*> expectedTokens = {new Token("foo", TOKEN_TYPE_NORMAL),
                                          new Token("\"bar\"", TOKEN_TYPE_NORMAL),
                                          new Token(";", TOKEN_TYPE_STATEMENT_END),
                                          new Token("server", TOKEN_TYPE_NORMAL),
                                          new Token("{", TOKEN_TYPE_START_BLOCK),
                                          new Token("listen", TOKEN_TYPE_NORMAL),
                                          new Token("80", TOKEN_TYPE_NORMAL),
                                          new Token("test", TOKEN_TYPE_NORMAL),
                                          new Token(";", TOKEN_TYPE_STATEMENT_END),
                                          new Token("server_name", TOKEN_TYPE_NORMAL),
                                          new Token("foo.com", TOKEN_TYPE_NORMAL),
                                          new Token(";", TOKEN_TYPE_STATEMENT_END),
                                          new Token("root", TOKEN_TYPE_NORMAL),
                                          new Token("/home/ubuntu/sites/foo/", TOKEN_TYPE_NORMAL),
                                          new Token(";", TOKEN_TYPE_STATEMENT_END),
                                          new Token("}", TOKEN_TYPE_END_BLOCK)};

    EXPECT_TRUE(compareTokenLists(tokenList->tokens, expectedTokens));

    for (Token* tkn : expectedTokens)
    {
        delete tkn;
    }
};

TEST_F(NginxConfigTokensTest, InValidConfig)
{
    input.open("unclosed_quotes");
    tokenList = NginxConfigTokens::makeNginxConfigTokens(&input);

    EXPECT_TRUE(tokenList == nullptr);
}