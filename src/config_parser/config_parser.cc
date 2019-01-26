#include <stack>
#include "config_parser.h"
#include "nginx_config_tokens.h"



// Constructor for NGginxConfigParser.  Inits the member variables
// With hard-coded values.
NginxConfigParser::NginxConfigParser()
{
    config = nullptr;

    // Each context key word will have a vector of parameters
    std::vector<std::string> serverVector = {"listen"};

    // Key=context token, Value=vector of parameter tokens
    contextsMap.insert(std::make_pair("server", serverVector));

    // Key=parameter token, Value=Parameter enum
    parametersMap.insert(std::make_pair("listen", ConfigParameter::LISTEN_PORT));
}

NginxConfigParser::~NginxConfigParser()
{
    delete config;
}

void NginxConfigParser::setParameterNameAndValue(std::string &parameterName, std::string &parameterValue,
                                                 std::deque<std::string> tokens)
{
    parameterName = tokens.back();
    tokens.pop_back();
    parameterValue = "";
    // For if a parameter value is multi-word
    while (!tokens.empty())
    {
        parameterValue += tokens.back();
        parameterValue += " ";
        tokens.pop_back();
    }
    parameterValue.pop_back();
}

bool NginxConfigParser::findParameterToSet(std::string context, std::string parameterName,
                                           std::string parameterValue, ConfigParameter& parameterToSet)
{
    // If currently within a context
    if (contextsMap.count(context))
    {
        // Loops through all parameters within that context
        for (std::string prmtr : contextsMap.at(context))
        {
            // If the prev token seen is a known parameter
            if (prmtr == parameterName)
            {
                parameterToSet = parametersMap.at(parameterName);
                return true;
            }
        }
    }
    else if (parametersMap.count(parameterName))
    {
        parameterToSet = parametersMap.at(parameterName);
        return true;
    }
    return false;
}

// TODO: make this work for nested contexts
void NginxConfigParser::setConfig(std::deque<std::string> tokens, std::string context)
{
    if (tokens.size() >= 2)
    {
        std::string parameterName, parameterValue;
        ConfigParameter parameterToSet;

        setParameterNameAndValue(parameterName, parameterValue, tokens);
        bool setParameter = findParameterToSet(context, parameterName, parameterValue, parameterToSet);

        if (setParameter)
        {
            config->parameters.insert(std::make_pair(parameterToSet, parameterValue));
        }
    }
}

// TODO: refactor this into a more manageable state
bool NginxConfigParser::Parse(std::istream* config_file)
{
    NginxConfigTokens* tokenList = NginxConfigTokens::makeNginxConfigTokens(config_file);
    if(tokenList == nullptr)
    {
        return false;
    }
    delete config;
    config = new NginxConfig;

    bool isError = false;
    std::deque<std::string> tokens;
    TokenType last_token_type = TOKEN_TYPE_START;
    TokenType tokenType;
    std::string last_token_str = "";
    std::stack<std::string> context;  // Keeps track of context level
    context.push("");

    for (Token* token : tokenList->tokens)
    {
        std::string tokenValue = token->value;
        tokenType = token->type;

        printf ("%s: %s\n", TokenTypeAsString(tokenType), tokenValue.c_str());
        if (tokenType == TOKEN_TYPE_ERROR)
        {
            isError = true;
            break;
        }

        if (tokenType == TOKEN_TYPE_COMMENT)
        {
            // Skip comments.
            continue;
        }

        if (tokenType == TOKEN_TYPE_START)
        {
            isError = true;
            break;
        }
        else if (tokenType == TOKEN_TYPE_NORMAL)
        {
            tokens.push_front(tokenValue);
            if (!(last_token_type == TOKEN_TYPE_START ||
                  last_token_type == TOKEN_TYPE_STATEMENT_END ||
                  last_token_type == TOKEN_TYPE_START_BLOCK ||
                  last_token_type == TOKEN_TYPE_END_BLOCK ||
                  last_token_type == TOKEN_TYPE_NORMAL))
            {
                isError = true;
                break;
            }
        }
        else if (tokenType == TOKEN_TYPE_STATEMENT_END)
        {
            tokens.clear();
            if (last_token_type != TOKEN_TYPE_NORMAL)
            {
                isError = true;
                break;
            }
        }
        else if (tokenType == TOKEN_TYPE_START_BLOCK)
        {
            // If a start block is seen then the last token will
            // be the context token.
            context.push(tokens.back());
            tokens.clear();
            if (last_token_type != TOKEN_TYPE_NORMAL)
            {
                isError = true;
                break;
            }
        }
        else if (tokenType == TOKEN_TYPE_END_BLOCK)
        {
            // If an end block is seen, we are exiting
            context.pop();
            if (last_token_type != TOKEN_TYPE_STATEMENT_END &&
                last_token_type != TOKEN_TYPE_END_BLOCK)
            {
                // This means that the end block can follow a ';'
                // or a '}' and be legal.
                isError = true;
                break;
            }
        }
        else
        {
            isError = true;
            break;
        }
        last_token_type = tokenType;
        setConfig(tokens, context.top());
        last_token_str = tokenValue;
    }
    if (context.top() != "")
    {
        // This means that there is an unequal number of
        // opening and closing brackets
        // Error
        printf("Bracket Count: %u", (unsigned)context.size());
        isError = true;
    }
    else if (last_token_type != TOKEN_TYPE_STATEMENT_END && last_token_type != TOKEN_TYPE_END_BLOCK)
    {
        isError = true;
    }
    if(isError)
    {
        // TODO: need to get rid of this after refactor
        printf("Bad transition from %s to %s\n",
               TokenTypeAsString(last_token_type),
               TokenTypeAsString(tokenType));
        delete config;
    }
    return !isError;
}

bool NginxConfigParser::Parse(const char* file_name) {
    std::ifstream config_file;
    config_file.open(file_name);
    if (!config_file.good()) {
        printf ("Failed to open config file: %s\n", file_name);
        return false;
    }

    const bool return_value =
            Parse(dynamic_cast<std::istream*>(&config_file));
    config_file.close();
    return return_value;
}

const char* NginxConfigParser::TokenTypeAsString(TokenType type) {
    switch (type) {
        case TOKEN_TYPE_START:         return "TOKEN_TYPE_START";
        case TOKEN_TYPE_NORMAL:        return "TOKEN_TYPE_NORMAL";
        case TOKEN_TYPE_START_BLOCK:   return "TOKEN_TYPE_START_BLOCK";
        case TOKEN_TYPE_END_BLOCK:     return "TOKEN_TYPE_END_BLOCK";
        case TOKEN_TYPE_COMMENT:       return "TOKEN_TYPE_COMMENT";
        case TOKEN_TYPE_STATEMENT_END: return "TOKEN_TYPE_STATEMENT_END";
        case TOKEN_TYPE_EOF:           return "TOKEN_TYPE_EOF";
        case TOKEN_TYPE_ERROR:         return "TOKEN_TYPE_ERROR";
        default:                       return "Unknown token type";
    }
}