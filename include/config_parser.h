#ifndef CONFIG_PARSER
#define CONFIG_PARSER

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <deque>

// This is a public enum that will be used to access
// values with in the config parameters object
enum class configParameter
{
    LISTEN_PORT
};

// The parsed representation of the entire config.
// To be passed to the server for configuration.
class NginxConfig {
public:
    std::unordered_map<configParameter, std::string> parameters;
};

// The driver that parses a config file and generates an NginxConfig.
// Returns a NginxConfig* with parameters for server.
class NginxConfigParser {
public:
    NginxConfigParser();
    ~NginxConfigParser();

    // Take a opened config file or file name (respectively) and store the
    // parsed config in the provided NginxConfig out-param.  Returns true
    // if the input config file is valid.
    // Also fills in parameters of NginxConfig* config to be returned by getConfig().
    bool Parse(std::istream* config_file);
    bool Parse(const char* file_name);

    // Returns NginxConfig object with parameters for server.
    // Must be run after a successful Parse()
    NginxConfig* getConfig();

    // Object to be returned to server for configuration.
    // Holds configuration parameters from Nginx conf file.
    NginxConfig* config;

private:
    enum TokenType {
        TOKEN_TYPE_START = 0,
        TOKEN_TYPE_NORMAL = 1,
        TOKEN_TYPE_START_BLOCK = 2,
        TOKEN_TYPE_END_BLOCK = 3,
        TOKEN_TYPE_COMMENT = 4,
        TOKEN_TYPE_STATEMENT_END = 5,
        TOKEN_TYPE_EOF = 6,
        TOKEN_TYPE_ERROR = 7
    };

    enum TokenParserState {
        TOKEN_STATE_INITIAL_WHITESPACE = 0,
        TOKEN_STATE_SINGLE_QUOTE = 1,
        TOKEN_STATE_DOUBLE_QUOTE = 2,
        TOKEN_STATE_TOKEN_TYPE_COMMENT = 3,
        TOKEN_STATE_TOKEN_TYPE_NORMAL = 4
    };

    void setParameterNameAndValue(std::string& parameterName, std::string& parameterValue, std::deque<std::string> tokens);
    bool findParameterToSet(std::string context, std::string parameterName,
            std::string parameterValue, configParameter& parameterToSet);

    // Holds key words that open new context (new {}).
    // Key=context token, value=vector of parameters within that context.
    std::unordered_map<std::string, std::vector<std::string>> contextsMap;

    // Parameters that are to be stored in the returned config.
    // Key=parameter token, value=enum corresponding to parameter
    std::unordered_map<std::string, configParameter> parametersMap;

    // This parses the current set of tokens, and sets any corresponding
    // parameters in NGinxConfig* config.
    void setConfig(std::deque<std::string> tokens, std::string context);


    TokenType ParseToken(std::istream* input, std::string* value);
    const char* TokenTypeAsString(TokenType type);
};

#endif //CONFIG_PARSER