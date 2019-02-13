#include "nginx_config.h"

std::unordered_map<string, string> NginxConfigg::getFlatParameters()
{
    return flatParams;
}

std::unordered_map<string, NginxConfigg *> NginxConfigg::getNestedParameters()
{
    return nestedParams;
}

std::vector<string> NginxConfigg::getOrderOfParams()
{
    return orderOfParams;
}

string NginxConfigg::getName()
{
    return name;
}

unsigned NginxConfigg::getDepth()
{
    return depth;
}

void NginxConfigg::addFlatParam(string key, string value)
{
    flatParams.insert( {key, value} );
    orderOfParams.push_back(key);
}

void NginxConfigg::addNestedParam(string handler, NginxConfigg* config)
{
    nestedParams.insert( {handler, config} );
    orderOfParams.push_back(handler);
    depth++;
}

string NginxConfigg::compareParamOrder(string param1, string param2)
{
    for(auto name : orderOfParams)
    {
        if(name == param1)
        {
            return param1;
        }
        else if(name == param2)
        {
            return param2;
        }
    }
    // neither param was found in config
    return "";
}

NginxConfigg::~NginxConfigg()
{
    std::unordered_map<string, NginxConfigg*>::iterator it;
    for(it = nestedParams.begin(); it != nestedParams.end(); it++)
    {
        // deletes each nested config
        delete it->second;
    }
}
