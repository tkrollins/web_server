#ifndef NGINX_CONFIG_H
#define NGINX_CONFIG_H

#include <unordered_map>
#include <vector>
#include <string>

using std::string;

// TODO: fix name later once other class removed
class NginxConfigg
{
public:
    NginxConfigg() { depth = 1; }
    explicit NginxConfigg(string n) { depth = 1; name = n;  }

    std::unordered_map<string,string> getFlatParameters();
    std::unordered_map<string,NginxConfigg*> getNestedParameters();

    // vector with param names in order found of context
    std::vector<string> getOrderOfParams();

    // will compare param names, returns name that was ordered first
    string compareParamOrder(string param1, string param2);

    // name of config context (will be 'server' or name of handler)
    string getName();
    unsigned getDepth();

    void addFlatParam(string key, string value);
    void addNestedParam(string handler, NginxConfigg* config);

private:
    ~NginxConfigg();

    // all single line params for a given context
    std::unordered_map<string, string> flatParams;
    // all nested params for a given context
    std::unordered_map<string, NginxConfigg*> nestedParams;
    // stores param names in order found of context
    std::vector<string> orderOfParams;

    unsigned depth;
    string name;
};


#endif //NGINX_CONFIG_H
