#ifndef __STRSWITCH_H__
#define __STRSWITCH_H__
#define strswitch(s) {const char* __s__=s;if(0){
#define strcase(t) }else if(strcmp(t,__s__)==0){
#define strdefault }else
#define strend }}
#endif