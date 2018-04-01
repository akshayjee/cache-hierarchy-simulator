#include<cstdio>
#include "cmdCenter.h"

int main(void){
  cmdCenter cmd;
  cmd.createStghold();
  cmd.askUserInfo();
  cmd.reinStghold();
  cmd.readall();
  /*std::cout<<"Address NO.0:"<<cmd.allAddr[0]<<std::endl;
  std::cout<<cmd.getCode(cmd.allAddr[0])<<std::endl;
  std::cout<<"Address NO.1:"<<cmd.allAddr[1]<<std::endl;
  std::cout<<cmd.getCode(cmd.allAddr[1])<<std::endl;
  cmd.addFetch(cmd.getCode(cmd.allAddr[1]));
  std::cout<<"InstnFetch"<<cmd.InstnFetch<<std::endl;
  std::cout<<"DataRdFetch"<<cmd.DataRd<<std::endl;
  std::cout<<"DataWrFetxh"<<cmd.DataWr<<std::endl;
  cache::decoder enigma;
  enigma.decodeAddr(cmd.allAddr[0],(cmd.level_1)->wareHouse[0]);
  std::cout<<"index: "<<enigma.index<<std::endl;
  std::cout<<"tag: "<<enigma.tag<<std::endl;
  std::cout<<"addr: "<<enigma.line<<std::endl;
  */
  cmd.simulate();
  cmd.displayeverything();
  return(EXIT_SUCCESS);
}

