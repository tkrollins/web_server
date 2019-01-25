#include <stack>
#include "config_parser.h"



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
    parametersMap.insert(std::make_pair("listen", configParameter::LISTEN_PORT));
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
    while (!tokens.empty())
    {
        parameterValue += tokens.back();
        parameterValue += " ";
        tokens.pop_back();
    }
    parameterValue.pop_back();
}

bool NginxConfigParser::findParameterToSet(std::string context, std::string parameterName,
                        std::string parameterValue, configParameter& parameterToSet)
{
    // If currently within a context
    if (contextsMap.count(context))
    {
        // Loops through all parameters within that context
        for (std::string prmt : contextsMap.at(context))
        {
            // If the prev token seen is a known parameter
            if (prmt == parameterName)
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
        configParameter parameterToSet;

        setParameterNameAndValue(parameterName, parameterValue, tokens);
        bool setParameter = findParameterToSet(context, parameterName, parameterValue, parameterToSet);

        if (setParameter)
        {
            config->parameters.insert(std::make_pair(parameterToSet, parameterValue));
        }
    }
}


NginxConfigParser::TokenType NginxConfigParser::ParseToken(std::istream* input,
                                                           std::string* value) {
    TokenParserState state = TOKEN_STATE_INITIAL_WHITESPACE;
    while (input->good()) {
        const char c = input->get();
        if (!input->good()) {
            break;
        }
        switch (state) {
            case TOKEN_STATE_INITIAL_WHITESPACE:
                switch (c) {
                    case '{':
                        *value = c;
                        return TOKEN_TYPE_START_BLOCK;
                    case '}':
                        *value = c;
                        return TOKEN_TYPE_END_BLOCK;
                    case '#':
                        *value = c;
                        state = TOKEN_STATE_TOKEN_TYPE_COMMENT;
                        continue;
                    case '"':
                        *value = c;
                        state = TOKEN_STATE_DOUBLE_QUOTE;
                        continue;
                    case '\'':
                        *value = c;
                        state = TOKEN_STATE_SINGLE_QUOTE;
                        continue;
                    case ';':
                        *value = c;
                        return TOKEN_TYPE_STATEMENT_END;
                    case ' ':
                    case '\t':
                    case '\n':
                    case '\r':
                        continue;
                    default:
                        *value += c;
                        state = TOKEN_STATE_TOKEN_TYPE_NORMAL;
                        continue;
                }
            case TOKEN_STATE_SINGLE_QUOTE:
                // TODO: the end of a quoted token should be followed by whitespace.
                // TODO: Maybe also define a QUOTED_STRING token type.
                // TODO: Allow for backslash-escaping within strings.
                *value += c;
                if (c == '\'') {
                    return TOKEN_TYPE_NORMAL;
                }
                continue;
            case TOKEN_STATE_DOUBLE_QUOTE:
                *value += c;
                if (c == '"') {
                    return TOKEN_TYPE_NORMAL;
                }
                continue;
            case TOKEN_STATE_TOKEN_TYPE_COMMENT:
                if (c == '\n' || c == '\r') {
                    return TOKEN_TYPE_COMMENT;
                }
                *value += c;
                continue;
            case TOKEN_STATE_TOKEN_TYPE_NORMAL:
                if (c == ' ' || c == '\t' || c == '\n' ||
                    c == ';' || c == '{' || c == '}') {
                    input->unget();
                    return TOKEN_TYPE_NORMAL;
                }
                *value += c;
                continue;
        }
    }

    // If we get here, we reached the end of the file.
    if (state == TOKEN_STATE_SINGLE_QUOTE ||
        state == TOKEN_STATE_DOUBLE_QUOTE) {
        return TOKEN_TYPE_ERROR;
    }

    return TOKEN_TYPE_EOF;
}

bool NginxConfigParser::Parse(std::istream* config_file) {

    delete config;
    config = new NginxConfig;

    std::deque<std::string> tokens;
    TokenType last_token_type = TOKEN_TYPE_START;
    TokenType token_type;
    std::string last_token_str = "";
    std::stack<std::string> context;  // Keeps track of context level
    context.push("");
    while (true)
    {
        std::string token;
        token_type = ParseToken(config_file, &token);
        printf ("%s: %s\n", TokenTypeAsString(token_type), token.c_str());
        if (token_type == TOKEN_TYPE_ERROR)
        {
            break;
        }

        if (token_type == TOKEN_TYPE_COMMENT)
        {
            // Skip comments.
            continue;
        }

        if (token_type == TOKEN_TYPE_START)
        {
            // Error.
            break;
        }
        else if (token_type == TOKEN_TYPE_NORMAL)
        {
            tokens.push_front(token);
            if (!(last_token_type == TOKEN_TYPE_START ||
                  last_token_type == TOKEN_TYPE_STATEMENT_END ||
                  last_token_type == TOKEN_TYPE_START_BLOCK ||
                  last_token_type == TOKEN_TYPE_END_BLOCK ||
                  last_token_type == TOKEN_TYPE_NORMAL))
            {
                //Error
                break;
            }
        }
        else if (token_type == TOKEN_TYPE_STATEMENT_END)
        {
            tokens.clear();
            if (last_token_type != TOKEN_TYPE_NORMAL)
            {
                // Error.
                break;
            }
        }
        else if (token_type == TOKEN_TYPE_START_BLOCK)
        {
            // If a start block is seen then the last token will
            // be the context token.
            context.push(tokens.back());
            tokens.clear();
            if (last_token_type != TOKEN_TYPE_NORMAL)
            {
                // Error.
                break;
            }
        }
        else if (token_type == TOKEN_TYPE_END_BLOCK)
        {
            // If an end block is seen, we are exiting
            context.pop();
            if (last_token_type != TOKEN_TYPE_STATEMENT_END &&
                last_token_type != TOKEN_TYPE_END_BLOCK)
            {
                // This means that the end block can follow a ';'
                // or a '}' and be legal.
                // Error.
                break;
            }
        }
        else if (token_type == TOKEN_TYPE_EOF)
        {
            if (context.top() != "")
            {
                // This means that there is an unequal number of
                // opening and closing brackets
                // Error
                printf("Bracket Count: %u", (unsigned)context.size());
                break;
            }
            if (last_token_type != TOKEN_TYPE_STATEMENT_END &&
                last_token_type != TOKEN_TYPE_END_BLOCK)
            {
                // Error.
                break;
            }
            return true;
        }
        else
        {
            // Error. Unknown token.
            break;
        }
        last_token_type = token_type;
        setConfig(tokens, context.top());
        last_token_str = token;
    }

    printf ("Bad transition from %s to %s\n",
            TokenTypeAsString(last_token_type),
            TokenTypeAsString(token_type));
    delete config;
    return false;
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