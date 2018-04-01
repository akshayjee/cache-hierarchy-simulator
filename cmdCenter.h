#ifndef _CMDCENTER__H
#define _CMDCENTER__H
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <iomanip>
#include "stghold.h"


class cmdCenter{
 public:
  stghold * level_1;
  stghold * level_2;
  long InstnFetch; //number of instrn fetch address
  long DataWr; //number of dataWrite address
  long DataRd;  //number of dataRead Address
  long misc;    //undefined code
  long totalfetches; //total number of fetches
  double accessTime; //total access Time
  double memAccTime; //main memory access Time
  int main_mem;
  vector<string> allAddr;
  
 public:
 cmdCenter():level_1(NULL),level_2(NULL),InstnFetch(0),DataWr(0),DataRd(0),misc(0),totalfetches(0),accessTime(0),memAccTime(0),main_mem(0){}
  void askUserInfo();
  void Delete();
  ~cmdCenter(){
    Delete();
  }
  void createStghold();
  void reinStghold();  
  void simulate();
  int getCode(string & line);   
  void addFetch(int code);
  void readall();  
  void access(string & oneLine);
  //void decideMemAccTime();
  void displayeverything();
  void read (int code, stghold * stg, string & line);
  void write (int code, stghold * stg, string & line);
};


#endif
