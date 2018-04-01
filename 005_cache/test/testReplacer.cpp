#include<iostream>
#include"replacer.h"
int main(void){
  replacer * rep_RND = new RND(4,4);
  for (int i = 0;i <100000; ++i){
    rep_RND->update(1,0);
    std::cout<<rep_RND->which2Replace(1)<<std::endl;
  }
  return(EXIT_SUCCESS);
}
