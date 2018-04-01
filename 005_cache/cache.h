#ifndef _CACHE__H
#define _CACHE__H
#include<vector>
#include<string>
#include"replacer.h"
#include<iostream>
#include<sstream>
#include<bitset>
#include<cmath>

class cmdCenter;
class scout{ //gather info about each cache
public:
  int cap;
  int bsize;
  string assoc;
  string rep;
  bool wralloc;
  double hitTime;
  scout(){}
};


class cache{
 public:
  int set;
  int way;
  string rep;
  int blockSize;
  bool wralloc;
  class weapon{
  public: //You can think of this class as block in the cache. It contains tag, valid bit and dirty bit
    bool isDirty;
    bool isValid;
    string tag;
    string addr;
  
    weapon():isDirty(0),isValid(0),tag(),addr(){}
    ~weapon(){}
    weapon(bool d, bool v,string t,string a):isDirty(0),isValid(v),tag(t),addr(a){}
  };//end of weapon, still inside cache
  vector<vector<weapon> > wpRecorder;
  replacer * repPolicy;
  int level_cache;
  double hitTime; //cache hitTime
  double missTimes;   //how many times
  double instMisses; // # of instruction misses
  double dataReadMisses; // # of data read misses
  double dataWriteMisses; // # of data write misses
  double miscMisses; // # of miscellaneous misses  
  double accHowManyR; //how many times 
  double accHowManyW; //how many times
  double accHowManyI;  
 public:
  class decoder;  
  ~cache(){}
 cache(int l):level_cache(l),hitTime(0),missTimes(0),instMisses(0),dataReadMisses(0),dataWriteMisses(0),miscMisses(0),accHowManyR(0), accHowManyW(0),accHowManyI(0){}

  void askCacheInfo(scout & cachInfo);
  int getWay(string & ass);
  
  void replaceBlock(int set, int index, decoder & enigma, bool needDirty);
  int tagComparator(int set, string tag);
  void readCache(int code, cmdCenter * cmd, string & line);
  void readMiss(decoder & enigma,cmdCenter * cmd,int code);
  void readMissInvalid(decoder & enigma,cmdCenter *cmd, int code, int whichBlock);
  void writeCache(int code,cmdCenter * cmd, string & line);
  void writeMiss(decoder & enigma, cmdCenter *cmd, int code);
  void writeMissInvalid(decoder & enigma, cmdCenter *cmd,int code, int whichBlock);
  int isSetFull(int whichSet);
  replacer * createReplacer();
  void addMiss(int m);
};


class cache::decoder{
  public:
    int index;
    string tag;
    string line;
    decoder():index(0),tag(){}
    ~decoder(){}
    void decodeAddr(string & oneLine,cache * Info);
};

#endif

