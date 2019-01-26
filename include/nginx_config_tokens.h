#ifndef NGINX_CONFIG_TOKENS_H
#define NGINX_CONFIG_TOKENS_H

#include <string>
#include <vector>
#include "nginx_enums.h"

using namespace NginxEnum;

struct Token {
public:
    std::string value;
    TokenType type;
};

class NginxConfigTokens
{
public:
    std::vector<Token*> tokens;
    // will return a new NginxConfigTokens object if input config is valid
    // otherwise will return nullptr
    static NginxConfigTokens* makeNginxConfigTokens(std::istream* input);

private:
    NginxConfigTokens() {}
    ~NginxConfigTokens();

    TokenType handleStateChangeChars(TokenParserState &state, TokenParserState newState, std::string& value, char c, std::istream *input);
    TokenType handleStatementEndChars(TokenParserState state, TokenType TYPE, std::string& value, char c, std::istream *input);
    TokenType handleWhitespaceChars(TokenParserState state, std::string& value, char c, std::istream *input);
    void handleNormalChars(TokenParserState& state, std::string& value, char c);
    void addToken(std::string& value, TokenType& type, TokenParserState& state);

    bool tokenizer(std::istream *input);

};


#endif //NGINX_CONFIG_TOKENS_H
