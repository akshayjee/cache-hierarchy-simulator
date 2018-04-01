#include "stghold.h"
#include <limits>
#include <iostream>
void stghold::decideConfig(){
  do {
    cout<<"\nPlease enter whether Level " << level << " Cache is SPLIT/UNIFIED/NONE: ";
    cin >> config;
  } while (config != "SPLIT" && config != "UNIFIED" && config != "NONE");

  if (config != "NONE") {
    cout << "Please enter the hit time for L" << level << " Cache: " ;
    cin >> hit_time;
  }
}


scout stghold::scoutDispatch(string type){
  scout a;
  cout << "\nPlease enter the specifications of Level " << level << " " << type << " Cache\n";

  //Input Associativity
  do {
    cout << "\nAssociativity (FULLY/DIRECT/n WAY): ";
    cin >> a.assoc;
    //    getline(cin, a.assoc);
  } while(a.assoc != "FULLY" && a.assoc != "DIRECT" && a.assoc != "1" && a.assoc != "2" && a.assoc != "4" && a.assoc != "8" && a.assoc != "16" && a.assoc != "32" && a.assoc != "64" && a.assoc != "128" && a.assoc != "256" && a.assoc != "512" && a.assoc != "1024" && a.assoc != "2048" && a.assoc != "4096" );
   
  //Input Block Size
  cout << "Block Size B: ";
  while (!(cin >> a.bsize)) {
    cout << "Block Size B: ";
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
  }

  //Input Capacity
  cout << "Capacity C: ";
  while (!(cin >> a.cap)) {
    cout << "Capacity C: ";
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
  }

  //Takes a value for Replacement Algorithm if it is not Direct Mapped
  if (a.assoc != "DIRECT") {
    do {
      cout << "Replacement algorithm (LRU/RND/NMRU/FIFO): ";
      cin >> a.rep;
    } while (a.rep != "NMRU" && a.rep != "LRU" && a.rep != "FIFO" && a.rep != "LRU"  && a.rep != "RND");
  }
  else ;

  //Takes input for Allocate on write miss
  char temp123;
  if( type != "inst"){
    do {
      cout << "Do you want to Allocate on Write miss? (Y/N): ";
      cin >> temp123;
      temp123 = tolower(temp123);
    } while (temp123 != 'y' && temp123 != 'n');
    
    if (temp123 == 'y'){  
      a.wralloc = true;
    }
    else{
      a.wralloc = false;
    }
  }
    return a;
}
void stghold::createWareHouse(){
    if ( config == "SPLIT"){
      scout scout_data = scoutDispatch("data");
      scout scout_inst = scoutDispatch("inst");
    //create two caches
      cacheNum = 2;
      wareHouse = new cache* [2]();
      wareHouse[0] = new cache(level);
      wareHouse[0]->askCacheInfo(scout_data);
      wareHouse[1] = new cache(level);
      wareHouse[1]->askCacheInfo(scout_inst);
    }
    else if(config == "UNIFIED"){
      //scout_1 dispatch
      scout scout_unified = scoutDispatch("unified");
    //create one cache
      cacheNum = 1;
      wareHouse = new cache * [1]();
      wareHouse[0] = new cache(level);
      wareHouse[0]->askCacheInfo(scout_unified);
    }
    //NONE
    //do nothing
  }


