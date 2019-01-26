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
#include "nginx_enums.h"

using namespace NginxEnum;

// The parsed representation of the entire config.
// To be passed to the server for configuration.
class NginxConfig {
public:
    std::unordered_map<ConfigParameter, std::string> parameters;
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

    // Object to be returned to server for configuration.
    // Holds configuration parameters from Nginx conf file.
    NginxConfig* config;

private:

    void setParameterNameAndValue(std::string& parameterName, std::string& parameterValue, std::deque<std::string> tokens);
    bool findParameterToSet(std::string context, std::string parameterName, std::string parameterValue, ConfigParameter& parameterToSet);

    // Holds key words that open new context (new {}).
    // Key=context token, value=vector of parameters within that context.
    std::unordered_map<std::string, std::vector<std::string>> contextsMap;

    // Parameters that are to be stored in the returned config.
    // Key=parameter token, value=enum corresponding to parameter
    std::unordered_map<std::string, ConfigParameter> parametersMap;

    // This parses the current set of tokens, and sets any corresponding
    // parameters in NGinxConfig* config.
    void setConfig(std::deque<std::string> tokens, std::string context);


//    TokenType ParseToken(std::istream* input, std::string* value);
    const char* TokenTypeAsString(TokenType type);
};

#endif //CONFIG_PARSER