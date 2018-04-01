#ifndef _STGHOLD__H
#define _STGHOLD__H
#include "cache.h"

class stghold{
 public:
  int level;
  string config;
  int hit_time;
  cache ** wareHouse;
  int cacheNum;
 public:
 stghold(int i):level(i),config(),hit_time(0),wareHouse(NULL),cacheNum(0){}
  ~stghold(){
    for (int i = 0; i<cacheNum;++i){
      delete wareHouse[i];
    }
    delete []wareHouse;
  }
  void decideConfig();
  scout scoutDispatch(string type);
  void createWareHouse();    
};

#endif
