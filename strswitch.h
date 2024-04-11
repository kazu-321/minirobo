#ifndef __STRSWITCH_H__
#define __STRSWITCH_H__

#include <vector>
#include <string>
#include <iostream>
#include <sstream>

bool startswith(const std::string& str, const std::string& prefix) {
    if (str.size() < prefix.size()) return false;
    return str.compare(0, prefix.size(), prefix) == 0;
}

std::vector<std::string> split(const std::string &str, char delimiter) {
    std::vector<std::string> internal;
    std::stringstream ss(str);  
    std::string tok;
    while(getline(ss, tok, delimiter)) {
        internal.push_back(tok);
    }
    return internal;
}
#define strswitch(s) {const char* __s__=s;if(0){
#define strstart(t) }else if(startswith(__s__,t)){
#define strcase(t) }else if(strcmp(t,__s__)==0){
#define strdefault }else
#define strend }}
#endif