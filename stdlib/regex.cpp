#include "regex.h"
#include <regex>
namespace mantra::stdlib {
MantraValue builtinRegexMatch(const std::vector<MantraValue>& a){if(a.size()<2)return MantraValue::boolean(false);try{return MantraValue::boolean(std::regex_search(a[1].string_value,std::regex(a[0].string_value)));}catch(...){return MantraValue::boolean(false);}}
MantraValue builtinRegexReplace(const std::vector<MantraValue>& a){if(a.size()<3)return MantraValue::string("");try{return MantraValue::string(std::regex_replace(a[1].string_value,std::regex(a[0].string_value),a[2].string_value));}catch(...){return a[1];}}
}
