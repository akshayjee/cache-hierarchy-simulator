#include "cmdCenter.h"
#include <cstdlib>
#include <cstdio>

int main(void){
  cmdCenter overlord;
  overlord.createStghold();
  overlord.askUserInfo();
  
  overlord.reinStghold();
  
