#include "base64.h"
static const std::string b64="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
namespace mantra::stdlib {
MantraValue builtinBase64Encode(const std::vector<MantraValue>& a){if(a.empty())return MantraValue::string("");const std::string& s=a[0].string_value;std::string o;int v=0,b=-6;for(unsigned char c:s){v=(v<<8)+c;b+=8;while(b>=0){o.push_back(b64[(v>>b)&0x3F]);b-=6;}}if(b>-6)o.push_back(b64[((v<<8)>>(b+8))&0x3F]);while(o.size()%4)o.push_back('=');return MantraValue::string(o);}
MantraValue builtinBase64Decode(const std::vector<MantraValue>& a){if(a.empty())return MantraValue::string("");const std::string& s=a[0].string_value;std::string o;std::vector<int> T(256,-1);for(int i=0;i<64;i++)T[b64[i]]=i;int v=0,b=-8;for(unsigned char c:s){if(T[c]==-1)break;v=(v<<6)+T[c];b+=6;if(b>=0){o.push_back(char((v>>b)&0xFF));b-=8;}}return MantraValue::string(o);}
}
