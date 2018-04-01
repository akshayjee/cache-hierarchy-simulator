#ifndef _REPLACER__H
#define _REPLACER__H
#include <vector>
#include <iostream>
#include <cstdlib>
using namespace std;
class replacer{
 public:
  std::vector<int> replacee;
  int way;
public:
  replacer(int s,int w):replacee(s),way(w){}
  ~replacer(){}
  virtual void update(int set, int bucket) = 0;
  virtual int  which2Replace(int set){
    return replacee[set];
  }

  friend std::ostream & operator << (std::ostream & ss, replacer * rep);
};

class FIFO: public replacer{
public:
  FIFO(int s, int w):replacer(s,w){}
  ~FIFO(){}
  virtual void update(int set,int bucket){
    replacee[set] = (replacee[set] + 1) % way; //update only happened when kick out 
  }
};



class LRU: public replacer{
 public:
  std::vector<std::vector<int> > track;
 public:
  LRU(int s,int w):replacer(s,w),track(s){
    for (int i = 0; i < s; i++){
      track[i] = vector<int> (w,0);
      for (int j = 0; j < w; j++){
	track[i][j] = w - 1 - j; // suppose we have 5-way, initial value should be 4 3 2 1 0
      }
    }
  }
  ~LRU(){}
  virtual void update(int set,int bucket){
    int temp = track[set][bucket];
    track[set][bucket] = 0; //this becomes most recently use
    for (int i = 0; i < (int) track[set].size(); ++i){
      if( i != bucket){ //skip the bucket that we have already changed
	if (track[set][i] < temp ){ //if the value inside of the bucket is less than the value replaced
	  ++track[set][i];   //increment by 1
	}
      }
    }
    for (int i = 0; i < (int) track[set].size(); ++i){
      if(track[set][i] == way - 1){
	replacee[set] = i;
	break;
      }
    }
  }
};


class NMRU:public replacer{
 public:
  NMRU(int s, int w):replacer(s,w){}
  ~NMRU(){}
  virtual void update(int set,int bucket){
    if (bucket == 0){
      replacee[set] = bucket + 1;
    }
    else{
      replacee[set] = bucket - 1;
    }  
  }
};

class RND: public replacer{
 public:
  RND(int s, int w):replacer(s,w){}
  ~RND(){}
  virtual void update(int set, int bucket){
    replacee[set] = rand() % (way);
  }
};


#endif

