#include "cmdCenter.h"
#include "stghold.h"
void cmdCenter::Delete(){
  delete level_1;
  delete level_2;
}
void cmdCenter::createStghold(){
  level_1 = new stghold(1);
  level_2 = new stghold(2);
}

void cmdCenter::askUserInfo(){
  //decideMemAccTime(); //decide Main Memory Access Time
  level_1->decideConfig(); //decide configuration
  level_2->decideConfig();
}



void cmdCenter::reinStghold(){
  level_1->createWareHouse();
  level_2->createWareHouse();
}

void cmdCenter::simulate(){
  vector<string>::iterator it = allAddr.begin();
    while (it != allAddr.end()){
      access(*it);
      ++it;
    }
}

void cmdCenter::access(string & oneLine){
  int code  = getCode(oneLine); //figure out code
  if ( (code != 0) && (code != 1) && (code != 2)){
    misc = misc + 1; //undefined code
    return;
  }
  else{
    addFetch(code); //corresponding fetch num + 1
    if(code == 0 || code == 2){ //read option
      read(code,level_1,oneLine);
    }
    else{
      //Assuming it's a data read operation and not misc
      write(code,level_1,oneLine);
    }
  }
}

int cmdCenter::getCode(string & line){ //TB checked
  string temp;
  int ans;
  stringstream str;
  str<<line;
  str>>temp;
  ans = atoi(temp.c_str());
  return ans;
}


void cmdCenter::addFetch(int code){
  switch (code){
  case 1:    ++DataWr;     break;
  case 0:    ++DataRd;     break;
  case 2:    ++InstnFetch; break;
    //default:   ++misc;       break;
  }
}


/*
void cmdCenter::decideMemAccTime(){
  //To write - or not to write?
}
*/
//we know you are reading from certain level of cache
//we know you are reading instruction or data
//we know the address you want to read
void cmdCenter::read (int code, stghold * stg, string & line){
  //No cache on this level
  if (stg->config == "NONE"){
    if (stg->level == 1){ //No level 1 cache
      read(code,level_2,line); //recursive
    }
    else{ //no level 2 cache
      //add main memory access time
      accessTime = accessTime + memAccTime;
      //main memory got everything so you got a hit
      //in main memory
      return; //read finish
    }
  }
  else if(stg->config == "SPLIT"){
    //this is split so we need to read cache according to code value;
    if (code == 0){ //read from data cache
      stg->wareHouse[0]->readCache(code,this,line);
    }
    else{ //read from instruction cache
      stg->wareHouse[1]->readCache(code,this,line);
    }
  }
  else if(stg->config == "UNIFIED"){
    //this is unified so we read from one single cache
    stg->wareHouse[0]->readCache(code, this, line);
  }
}

//We know you are writing to certain level of cache
//We know you are writing instruction or data
//we know the address you want to write
void cmdCenter::write (int code, stghold * stg, string & line){
  //no cache
  if(stg->config == "NONE"){
    if (stg->level == 1){ //No level 1 cache
      write(code,level_2,line); //recursive
    }
    else{ //no level 2 cache
      //add main memory access time
      accessTime = accessTime + memAccTime;
      //main memory got everything so you got a hit
      //in main memory
      return;
    }
  }
  else if(stg->config == "SPLIT"){
    //this is split so we need to write to cache according to code value;
    if (code == 1){ //Write to data cache
      stg->wareHouse[0]->writeCache(code,this,line);
    }
    else{ //There is something wrong! This function should not have been called if code !=1. Report error
      cout << "fatal:The code has entered the write function in cmdCenter.cpp for line: " << line << "which should not have happened! Aborting sorry.\n";
      exit(EXIT_FAILURE);	
    }
  }
  else if(stg->config == "UNIFIED"){
    //this is unified so we write to one single cache
    stg->wareHouse[0]->writeCache(code, this, line);
  }
}

void cmdCenter::readall(){
  //cout<<"Hello User!\n\n";
  //Opening the text file here:
  int Trace_valid = 0;
  string Dinero_trace;
  string Curr_line; 
  do {
    cout << "\nPlease specify the Trace file with Data in Dinero III Format: ";
    cin >> Dinero_trace;
    ifstream trace_file (Dinero_trace.c_str());
    
    if(!trace_file.is_open()) {
      fprintf(stderr,"Could not open specified Trace file. Please try again!\n");
	Trace_valid = 0;
	//break; We do not want to break here - user should enter another trace file name!
    }    
    //Storing each line of text file into allAddr:
    else {
      Trace_valid = 1;
      while(!trace_file.eof()) {
	getline(trace_file,Curr_line);
	if(trace_file.eof()){
	  break;
	}
	allAddr.push_back(Curr_line);
	totalfetches++;
      }
    }
  } while(Trace_valid != 1);

  cout << "Please enter the Access time to Main Memory: ";
  cin >> main_mem;
}


//This guy right below displays the final output results:
void cmdCenter::displayeverything(){

  float fractTotal = totalfetches/(float)totalfetches;
  float fractInstn = InstnFetch/(float)totalfetches;
  float fractDataWr = DataWr/(float)totalfetches;
  float fractDataRd = DataRd/(float)totalfetches;
  float fractData = fractDataWr + fractDataRd;
  float fractMisc = misc/(float)totalfetches;

  //The variables have been defined as follows:
  //missesTotal1[0] -> Total number of times level 1 Data cache missed
  //missesTotal1[1] -> Total number of times level 1 Instruction cache missed
  //missesTotal2[0] -> Total number of times level 2 Data cache missed
  //missesTotal2[1] -> Total number of times level 2 Instruction cache missed
  //and so on for others too..
  
  long missesTotal1[2] = {0,0};
  long missesTotal2[2] = {0,0};
  /*  if (level_1->config == "NONE" && level_2->config == "NONE") {
    missesTotal1 = totalfetches;
    missesTotal2 = totalfetches;
    }*/
  long missesInstn1[2] = {0,0};
  long missesInstn2[2] = {0,0};
  /*if (level_1->config == "NONE" && level_2->config == "NONE") {
    missesInstn1 = InstnFetch;
    missesInstn2 = InstnFetch;
    } */ 
  long missesDataWr1[2] = {0,0};
  long missesDataWr2[2] = {0,0};
  /*if (level_1->config == "NONE" && level_2->config == "NONE") {
    missesDataWr1 = DataWr;
    missesDataWr2 = DataWr;
    }*/
  long missesDataRd1[2] = {0,0};
  long missesDataRd2[2] = {0,0};
  /*if (level_1->config == "NONE" && level_2->config == "NONE") {
    missesDataRd1 = DataRd;
    missesDataRd2 = DataRd;
    }*/
  long missesMisc1[2] = {0,0};
  long missesMisc2[2] = {0,0};
  /*if (level_1->config == "NONE" && level_2->config == "NONE") {
    missesMisc1 = misc;
    missesMisc2 = misc;
    }*/
  
  for (int i = 0; i< level_1->cacheNum; i++){
    missesTotal1[i] += level_1->wareHouse[i]->missTimes;
    missesInstn1[i] += level_1->wareHouse[i]->instMisses;
    missesDataWr1[i] += level_1->wareHouse[i]->dataWriteMisses;
    missesDataRd1[i] += level_1->wareHouse[i]->dataReadMisses;
    missesMisc1[i] += level_1->wareHouse[i]->miscMisses;
  }
  
  for (int i = 0; i< level_2->cacheNum; i++){
    missesTotal2[i] += level_2->wareHouse[i]->missTimes;
    missesInstn2[i] += level_2->wareHouse[i]->instMisses;
    missesDataWr2[i] += level_2->wareHouse[i]->dataWriteMisses;
    missesDataRd2[i] += level_2->wareHouse[i]->dataReadMisses;
    missesMisc2[i] += level_2->wareHouse[i]->miscMisses;
  }

  float fractMissTotal1[2],fractMissInstn1[2],fractMissDataWr1[2], fractMissDataRd1[2],fractMissData1[2]; 
  float fractMissTotal2[2],fractMissInstn2[2],fractMissDataWr2[2], fractMissDataRd2[2],fractMissData2[2]; 
  float fractMissMisc1[2] = {0,0};
  float fractMissMisc2[2] = {0,0};

  for (int i = 0; i<=(level_1->cacheNum - 1); i++) {
    fractMissTotal1[i] = missesTotal1[i]/(float)(level_1->wareHouse[i]->accHowManyR+level_1->wareHouse[i]->accHowManyI+level_1->wareHouse[i]->accHowManyW);
    if (level_1->wareHouse[i]->accHowManyI != 0)
      fractMissInstn1[i] = missesInstn1[i]/(float)level_1->wareHouse[i]->accHowManyI;
    else
      fractMissInstn1[i] = 0;
    if (level_1->wareHouse[i]->accHowManyW !=0 )
      fractMissDataWr1[i] = missesDataWr1[i]/(float)level_1->wareHouse[i]->accHowManyW;
    else
      fractMissData1[i] = 0; 
    if (level_1->wareHouse[i]->accHowManyR != 0)
      fractMissDataRd1[i] = missesDataRd1[i]/(float)level_1->wareHouse[i]->accHowManyR;
    else
      fractMissData1[i] = 0;
    if (level_1->wareHouse[i]->accHowManyW + level_1->wareHouse[i]->accHowManyR !=0)
      fractMissData1[i] = (missesDataWr1[i] + missesDataRd1[i])/(float)(level_1->wareHouse[i]->accHowManyW+level_1->wareHouse[i]->accHowManyR);
    else
      fractMissData1[i] = 0;
  }
  for (int i=0; i<=(level_2->cacheNum -1); i++) {
    fractMissTotal2[i] = missesTotal2[i]/(float)(level_2->wareHouse[i]->accHowManyR+level_2->wareHouse[i]->accHowManyI+level_2->wareHouse[i]->accHowManyW);
    if (level_2->wareHouse[i]->accHowManyI != 0)
      fractMissInstn2[i] = missesInstn2[i]/(float)level_2->wareHouse[i]->accHowManyI;
    else
      fractMissInstn2[i] = 0;
    if (level_2->wareHouse[i]->accHowManyW !=0 )
      fractMissDataWr2[i] = missesDataWr2[i]/(float)level_2->wareHouse[i]->accHowManyW;
    else
      fractMissData2[i] = 0; 
    if (level_2->wareHouse[i]->accHowManyR != 0)
      fractMissDataRd2[i] = missesDataRd2[i]/(float)level_2->wareHouse[i]->accHowManyR;
    else
      fractMissData2[i] = 0;
    if (level_2->wareHouse[i]->accHowManyW + level_2->wareHouse[i]->accHowManyR !=0)
      fractMissData2[i] = (missesDataWr2[i] + missesDataRd2[i])/(float)(level_2->wareHouse[i]->accHowManyW+level_2->wareHouse[i]->accHowManyR);
    else
      fractMissData2[i] = 0;
  }  

  if (misc != 0) {
    fractMissMisc1[0] = missesMisc1[0]/(float)misc;
    fractMissMisc1[1] = missesMisc1[1]/(float)misc;
    fractMissMisc2[0] = missesMisc1[0]/(float)misc;
    fractMissMisc2[1] = missesMisc1[1]/(float)misc;
  }
  else ;

  //While calcuating for L1 avg access times, we need to know which L2 Cache it goes to
  //we should techinically use that to calculate L1 avg. access time but for simplicity
  //I have assumed that L2 miss rates are small enough that we can assume any one value
  //for miss time (Here assumed miss time for L1 = avg_access_L2_u).
  float avg_access_L2_u,avg_access_L2_i,avg_access_L2_d,avg_access_L1_u,avg_access_L1_i,avg_access_L1_d;

  if (level_2 -> config != "NONE") {
    //For L2 unified Cache:
    avg_access_L2_u = level_2->hit_time + fractMissTotal2[0]*main_mem;
    //For L2 Data Cache:
    avg_access_L2_d = level_2->hit_time + fractMissData2[0]*main_mem;
    //For L2 Instruction Cache:
    avg_access_L2_i = level_2->hit_time + fractMissInstn2[1]*main_mem;
  }
  if (level_1 -> config != "NONE" && level_2 -> config != "NONE") {
    //For L1 unified Cache:
    avg_access_L1_u = level_1->hit_time + fractMissTotal1[0]*avg_access_L2_u;
    //For L1 Data Cache:
    avg_access_L1_d = level_1->hit_time + fractMissData1[0]*avg_access_L2_u;
    //For L1 Instruction Cache:
    avg_access_L1_i = level_1->hit_time + fractMissInstn1[1]*avg_access_L2_u;
  }

  if (level_1 -> config != "NONE" && level_2 -> config == "NONE") {
    //For L1 unified Cache:
    avg_access_L1_u = level_1->hit_time + fractMissTotal1[0]*main_mem;
    //For L1 Data Cache:
    avg_access_L1_d = level_1->hit_time + fractMissData1[0]*main_mem;
    //For L1 Instruction Cache:
    avg_access_L1_i = level_1->hit_time + fractMissInstn1[1]*main_mem;
  }  
  
  //Display begins from here:
  cout<< endl << endl;
  cout<<"Top level metrics for the entire system of caches is as follows:\n";
  cout<<setw(40)<<left<<"Metrics"<<setw(20)<<left<<"Total"<<setw(20)<<left<<"Instrn"<<setw(20)<<left<<"Data"<<setw(20)<<left<<"Read"<<setw(20)<<left<<"Write"<<setw(20)<<left<<"Misc"<<endl;
  cout<<setw(40)<<left<<"______________________________"<<setw(20)<<left<<"_______________"<<setw(20)<<left<<"_______________"<<setw(20)<<left<<"_______________"<<setw(20)<<left
      <<"_______________"<<setw(20)<<left<<"_______________"<<setw(20)<<left<<"_______________"<<endl;  
  cout<<setw(40)<<left<<"Demand Fetches"<<setw(20)<<left<<fixed<<totalfetches<<setw(20)<<left<<fixed<<InstnFetch<<setw(20)<<left<<fixed<<(DataWr+DataRd)
      <<setw(20)<<left<<fixed<<DataRd<<setw(20)<<left<<fixed<<DataWr<<setw(20)<<left<<fixed<<misc<<endl;  
  cout<<setw(40)<<left<<fixed<<" Fraction of Total"<<setw(20)<<setprecision(4)<<left<<fixed<<fractTotal<<setw(20)<<setprecision(4)<<left<<fixed<<fractInstn
      <<setw(20)<<setprecision(4)<<left<<fixed<<fractData<<setw(20)<<setprecision(4)<<left<<fixed<<fractDataRd<<setw(20)<<setprecision(4)<<left<<fixed<<fractDataWr
      <<setw(20)<<setprecision(4)<<left<<fixed<<fractMisc<<endl<<endl;
  
  if (level_1->config == "UNIFIED") {
    cout<<"\nFor LEVEL 1 UNIFIED CACHE: " << "(Average access time = " << avg_access_L1_u << " ns)\n";
    cout<<setw(40)<<left<<"Metrics"<<setw(20)<<left<<"Total"<<setw(20)<<left<<"Instrn"<<setw(20)<<left<<"Data"<<setw(20)<<left<<"Read"<<setw(20)<<left<<"Write"<<setw(20)<<left<<"Misc"<<endl;
    cout<<setw(40)<<left<<"______________________________"<<setw(20)<<left<<"_______________"<<setw(20)<<left<<"_______________"<<setw(20)<<left<<"_______________"<<setw(20)<<left
	<<"_______________"<<setw(20)<<left<<"_______________"<<setw(20)<<left<<"_______________"<<endl;
    double tot_u1 = level_1->wareHouse[0]->accHowManyR+level_1->wareHouse[0]->accHowManyW+level_1->wareHouse[0]->accHowManyI;
    cout<<setw(40)<<left<<"Demand Fetches"<<setw(20)<<left<<fixed<<tot_u1<<setw(20)<<left<<fixed<<level_1->wareHouse[0]->accHowManyI
	<<setw(20)<<left<<fixed<<(level_1->wareHouse[0]->accHowManyW+level_1->wareHouse[0]->accHowManyR)
	<<setw(20)<<left<<fixed<<level_1->wareHouse[0]->accHowManyR<<setw(20)<<left<<fixed<<level_1->wareHouse[0]->accHowManyW<<setw(20)<<left<<fixed<<misc<<endl;  
    cout<<setw(40)<<left<<" Fraction of Total"<<setw(20)<<setprecision(4)<<left<<fixed<<(tot_u1/tot_u1)<<setw(20)<<setprecision(4)<<left<<fixed<<(level_1->wareHouse[0]->accHowManyI/(float)tot_u1)
	<<setw(20)<<setprecision(4)<<left<<fixed<<((level_1->wareHouse[0]->accHowManyR+level_1->wareHouse[0]->accHowManyW)/(float)tot_u1)
	<<setw(20)<<setprecision(4)<<left<<fixed<<(level_1->wareHouse[0]->accHowManyR/(float)tot_u1)<<setw(20)<<setprecision(4)<<left<<fixed<<(level_1->wareHouse[0]->accHowManyW/(float)tot_u1)
	<<setw(20)<<setprecision(4)<<left<<fixed<<fractMisc<<endl<<endl;
    cout<<setw(40)<<left<<"Demand Misses"<<setw(20)<<left<<fixed<<missesTotal1[0]<<setw(20)<<left<<fixed<<missesInstn1[0]<<setw(20)<<left<<fixed<<(missesDataWr1[0]+missesDataRd1[0])
	<<setw(20)<<left<<fixed<<missesDataRd1[0]<<setw(20)<<left<<fixed<<missesDataWr1[0]<<setw(20)<<left<<fixed<<missesMisc1[0]<<endl;    
    cout<<setw(40)<<left<<" Demand miss rate"<<setw(20)<<setprecision(4)<<left<<fixed<<fractMissTotal1[0]<<setw(20)<<setprecision(4)<<left<<fixed<<fractMissInstn1[0]
	<<setw(20)<<setprecision(4)<<left<<fixed<<fractMissData1[0]<<setw(20)<<setprecision(4)<<left<<fixed<<fractMissDataRd1[0]<<setw(20)<<setprecision(4)<<left<<fixed<<fractMissDataWr1[0]
	<<setw(20)<<setprecision(4)<<left<<fixed<<fractMissMisc1[0]<<endl<<endl;
  }
  
  if (level_1->config == "SPLIT") {
    
    cout<<"\nFor LEVEL 1 INSTRUCTION CACHE: " << "(Average access time = " << avg_access_L1_i << " ns)\n";
    
    cout<<setw(40)<<left<<"Metrics"<<setw(20)<<left<<"Total"<<setw(20)<<left<<"Instrn"<<setw(20)<<left<<"Data"<<setw(20)<<left<<"Read"<<setw(20)<<left<<"Write"<<setw(20)<<left<<"Misc"<<endl;
    cout<<setw(40)<<left<<"______________________________"<<setw(20)<<left<<"_______________"<<setw(20)<<left<<"_______________"<<setw(20)<<left<<"_______________"<<setw(20)<<left
	<<"_______________"<<setw(20)<<left<<"_______________"<<setw(20)<<left<<"_______________"<<endl;
    double tot_si1 = level_1->wareHouse[1]->accHowManyI;
    cout<<setw(40)<<left<<"Demand Fetches"<<setw(20)<<left<<fixed<<fixed<<tot_si1<<setw(20)<<left<<fixed<<level_1->wareHouse[1]->accHowManyI
	<<setw(20)<<left<<fixed<<(level_1->wareHouse[1]->accHowManyR+level_1->wareHouse[1]->accHowManyW)
	<<setw(20)<<left<<fixed<<level_1->wareHouse[1]->accHowManyR<<setw(20)<<left<<fixed<<level_1->wareHouse[1]->accHowManyW<<setw(20)<<left<<fixed<<misc<<endl;  
    cout<<setw(40)<<left<<" Fraction of Total"<<setw(20)<<setprecision(4)<<left<<fixed<<(tot_si1/(float)tot_si1)
	<<setw(20)<<setprecision(4)<<left<<fixed<<(level_1->wareHouse[1]->accHowManyI/(float)tot_si1)
	<<setw(20)<<setprecision(4)<<left<<fixed<<((level_1->wareHouse[1]->accHowManyR+level_1->wareHouse[1]->accHowManyW)/(float)tot_si1)
	<<setw(20)<<setprecision(4)<<left<<fixed<<(level_1->wareHouse[1]->accHowManyR/(float)tot_si1)
	<<setw(20)<<setprecision(4)<<left<<fixed<<(level_1->wareHouse[1]->accHowManyW/(float)tot_si1)
	<<setw(20)<<setprecision(4)<<left<<fixed<<fractMisc<<endl<<endl;
    cout<<setw(40)<<left<<"Demand Misses"<<setw(20)<<left<<fixed<<missesTotal1[1]<<setw(20)<<left<<fixed<<missesInstn1[1]<<setw(20)<<left<<fixed<<(missesDataWr1[1]+missesDataRd1[1])
	<<setw(20)<<left<<fixed<<missesDataRd1[1]<<setw(20)<<left<<fixed<<missesDataWr1[1]<<setw(20)<<left<<fixed<<missesMisc1[1]<<endl;
    cout<<setw(40)<<left<<" Demand miss rate"<<setw(20)<<left<<fixed<<fractMissTotal1[1]<<setw(20)<<left<<fixed<<fractMissInstn1[1]
	<<setw(20)<<left<<fixed<<fractMissData1[1]<<setw(20)<<left<<fixed<<fractMissDataRd1[1]<<setw(20)<<left<<fixed<<fractMissDataWr1[1]
	<<setw(20)<<left<<fixed<<fractMissMisc1[1]<<endl<<endl;

    cout<<"\nFor LEVEL 1 DATA CACHE: " << "(Average access time = " << avg_access_L1_d << " ns)\n";
    cout<<setw(40)<<left<<"Metrics"<<setw(20)<<left<<fixed<<"Total"<<setw(20)<<left<<fixed<<"Instrn"<<setw(20)<<left<<fixed<<"Data"<<setw(20)<<left<<fixed<<"Read"<<setw(20)<<left<<fixed<<"Write"<<setw(20)<<left<<fixed<<"Misc"<<endl;
    cout<<setw(40)<<left<<"______________________________"<<setw(20)<<left<<"_______________"<<setw(20)<<left<<"_______________"<<setw(20)<<left<<"_______________"<<setw(20)<<left
	<<"_______________"<<setw(20)<<left<<"_______________"<<setw(20)<<left<<"_______________"<<endl;    
    double tot_sd1 = level_1->wareHouse[0]->accHowManyR + level_1->wareHouse[0]->accHowManyW;
    cout<<setw(40)<<left<<"Demand Fetches"<<setw(20)<<left<<fixed<<fixed<<tot_sd1<<setw(20)<<left<<fixed<<level_1->wareHouse[0]->accHowManyI
	<<setw(20)<<left<<fixed<<(level_1->wareHouse[0]->accHowManyR+level_1->wareHouse[0]->accHowManyW)
	<<setw(20)<<left<<fixed<<level_1->wareHouse[0]->accHowManyR<<setw(20)<<left<<fixed<<level_1->wareHouse[0]->accHowManyW<<setw(20)<<left<<fixed<<misc<<endl;  
    cout<<setw(40)<<left<<" Fraction of Total"<<setw(20)<<setprecision(4)<<left<<fixed<<(tot_sd1/(float)tot_sd1)
	<<setw(20)<<setprecision(4)<<left<<fixed<<(level_1->wareHouse[0]->accHowManyI/(float)tot_sd1)
	<<setw(20)<<setprecision(4)<<left<<fixed<<((level_1->wareHouse[0]->accHowManyR+level_1->wareHouse[0]->accHowManyW)/(float)tot_sd1)
	<<setw(20)<<setprecision(4)<<left<<fixed<<(level_1->wareHouse[0]->accHowManyR/(float)tot_sd1)
	<<setw(20)<<setprecision(4)<<left<<fixed<<(level_1->wareHouse[0]->accHowManyW/(float)tot_sd1)
	<<setw(20)<<setprecision(4)<<left<<fixed<<fractMisc<<endl<<endl;
    cout<<setw(40)<<left<<"Demand Misses"<<setw(20)<<left<<fixed<<missesTotal1[0]<<setw(20)<<left<<fixed<<missesInstn1[0]<<setw(20)<<left<<fixed<<(missesDataWr1[0]+missesDataRd1[0])
	<<setw(20)<<left<<fixed<<missesDataRd1[0]<<setw(20)<<left<<fixed<<missesDataWr1[0]<<setw(20)<<left<<fixed<<missesMisc1[0]<<endl;    
    cout<<setw(40)<<left<<" Demand miss rate"<<setw(20)<<setprecision(4)<<left<<fixed<<fractMissTotal1[0]<<setw(20)<<setprecision(4)<<left<<fixed<<fractMissInstn1[0]
	<<setw(20)<<setprecision(4)<<left<<fixed<<fractMissData1[0]<<setw(20)<<setprecision(4)<<left<<fixed<<fractMissDataRd1[0]<<setw(20)<<setprecision(4)<<left<<fixed<<fractMissDataWr1[0]
	<<setw(20)<<setprecision(4)<<left<<fixed<<fractMissMisc1[0]<<endl<<endl;
  }
   
  if (level_2->config == "UNIFIED") {
    cout<<"\nFor LEVEL 2 UNIFIED CACHE: " << "(Average access time = " << avg_access_L2_u << " ns)\n";
    cout<<setw(40)<<left<<"Metrics"<<setw(20)<<left<<"Total"<<setw(20)<<left<<"Instrn"<<setw(20)<<left<<"Data"<<setw(20)<<left<<"Read"<<setw(20)<<left<<"Write"<<setw(20)<<left<<"Misc"<<endl;
    cout<<setw(40)<<left<<"______________________________"<<setw(20)<<left<<"_______________"<<setw(20)<<left<<"_______________"<<setw(20)<<left<<"_______________"<<setw(20)<<left
	<<"_______________"<<setw(20)<<left<<"_______________"<<setw(20)<<left<<"_______________"<<endl;
    double tot_u2 = level_2->wareHouse[0]->accHowManyR+level_2->wareHouse[0]->accHowManyW+level_2->wareHouse[0]->accHowManyI;
    cout<<setw(40)<<left<<"Demand Fetches"<<setw(20)<<left<<fixed<<tot_u2<<setw(20)<<left<<fixed<<level_2->wareHouse[0]->accHowManyI
	<<setw(20)<<left<<fixed<<(level_2->wareHouse[0]->accHowManyW+level_2->wareHouse[0]->accHowManyR)
	<<setw(20)<<left<<fixed<<level_2->wareHouse[0]->accHowManyR<<setw(20)<<left<<fixed<<level_2->wareHouse[0]->accHowManyW<<setw(20)<<left<<fixed<<misc<<endl;  
    cout<<setw(40)<<left<<" Fraction of Total"<<setw(20)<<setprecision(4)<<left<<fixed<<(tot_u2/tot_u2)<<setw(20)<<setprecision(4)<<left<<fixed<<(level_2->wareHouse[0]->accHowManyI/(float)tot_u2)
	<<setw(20)<<setprecision(4)<<left<<fixed<<((level_2->wareHouse[0]->accHowManyR+level_2->wareHouse[0]->accHowManyW)/(float)tot_u2)
	<<setw(20)<<setprecision(4)<<left<<fixed<<(level_2->wareHouse[0]->accHowManyR/(float)tot_u2)<<setw(20)<<setprecision(4)<<left<<fixed<<(level_2->wareHouse[0]->accHowManyW/(float)tot_u2)
	<<setw(20)<<setprecision(4)<<left<<fixed<<fractMisc<<endl<<endl;
    cout<<setw(40)<<left<<"Demand Misses"<<setw(20)<<left<<fixed<<missesTotal2[0]<<setw(20)<<left<<fixed<<missesInstn2[0]<<setw(20)<<left<<fixed<<(missesDataWr2[0]+missesDataRd2[0])
	<<setw(20)<<left<<fixed<<missesDataRd2[0]<<setw(20)<<left<<fixed<<missesDataWr2[0]<<setw(20)<<left<<fixed<<missesMisc2[0]<<endl;
    cout<<setw(40)<<left<<" Demand miss rate"<<setw(20)<<setprecision(4)<<left<<fixed<<fractMissTotal2[0]<<setw(20)<<setprecision(4)<<left<<fixed<<fractMissInstn2[0]
	<<setw(20)<<setprecision(4)<<left<<fixed<<fractMissData2[0]<<setw(20)<<setprecision(4)<<left<<fixed<<fractMissDataRd2[0]<<setw(20)<<setprecision(4)<<left<<fixed<<fractMissDataWr2[0]
	<<setw(20)<<setprecision(4)<<left<<fixed<<fractMissMisc2[0]<<endl<<endl;
  }

  if (level_2->config == "SPLIT") {
    
    cout<<"\nFor LEVEL 2 INSTRUCTION CACHE: " << "(Average access time = " << avg_access_L2_i << " ns)\n";
    cout<<setw(40)<<left<<"Metrics"<<setw(20)<<left<<"Total"<<setw(20)<<left<<"Instrn"<<setw(20)<<left<<"Data"<<setw(20)<<left<<"Read"<<setw(20)<<left<<"Write"<<setw(20)<<left<<"Misc"<<endl;
    cout<<setw(40)<<left<<"______________________________"<<setw(20)<<left<<"_______________"<<setw(20)<<left<<"_______________"<<setw(20)<<left<<"_______________"<<setw(20)<<left
	<<"_______________"<<setw(20)<<left<<"_______________"<<setw(20)<<left<<"_______________"<<endl;
    double tot_si2 = level_2->wareHouse[1]->accHowManyI;
    cout<<setw(40)<<left<<"Demand Fetches"<<setw(20)<<left<<fixed<<tot_si2<<setw(20)<<left<<fixed<<level_2->wareHouse[1]->accHowManyI
	<<setw(20)<<left<<fixed<<(level_2->wareHouse[1]->accHowManyR+level_2->wareHouse[1]->accHowManyW)
	<<setw(20)<<left<<fixed<<level_2->wareHouse[1]->accHowManyR<<setw(20)<<left<<fixed<<level_2->wareHouse[1]->accHowManyW<<setw(20)<<left<<fixed<<misc<<endl;  
    cout<<setw(40)<<left<<" Fraction of Total"<<setw(20)<<setprecision(4)<<left<<fixed<<(tot_si2/(float)tot_si2)
	<<setw(20)<<setprecision(4)<<left<<fixed<<(level_2->wareHouse[1]->accHowManyI/(float)tot_si2)
	<<setw(20)<<setprecision(4)<<left<<fixed<<((level_2->wareHouse[1]->accHowManyR+level_2->wareHouse[1]->accHowManyW)/(float)tot_si2)
	<<setw(20)<<setprecision(4)<<left<<fixed<<(level_2->wareHouse[1]->accHowManyR/(float)tot_si2)
	<<setw(20)<<setprecision(4)<<left<<fixed<<(level_2->wareHouse[1]->accHowManyW/(float)tot_si2)
	<<setw(20)<<setprecision(4)<<left<<fixed<<fractMisc<<endl<<endl;
    cout<<setw(40)<<left<<fixed<<"Demand Misses"<<setw(20)<<left<<fixed<<missesTotal2[1]<<setw(20)<<left<<fixed<<missesInstn2[1]<<setw(20)<<left<<fixed<<(missesDataWr2[1]+missesDataRd2[1])
	<<setw(20)<<left<<fixed<<missesDataRd2[1]<<setw(20)<<left<<fixed<<missesDataWr2[1]<<setw(20)<<left<<fixed<<missesMisc2[1]<<endl;
    cout<<setw(40)<<left<<" Demand miss rate"<<setw(20)<<setprecision(4)<<left<<fixed<<fractMissTotal2[1]<<setw(20)<<setprecision(4)<<left<<fixed<<fractMissInstn2[1]
	<<setw(20)<<setprecision(4)<<left<<fixed<<fractMissData2[1]<<setw(20)<<setprecision(4)<<left<<fixed<<fractMissDataRd2[1]<<setw(20)<<setprecision(4)<<left<<fixed<<fractMissDataWr2[1]
	<<setw(20)<<setprecision(4)<<left<<fixed<<fractMissMisc2[1]<<endl<<endl;

    cout<<"\nFor LEVEL 2 DATA CACHE: " << "(Average access time = " << avg_access_L2_d << " ns)\n";
    cout<<setw(40)<<left<<"Metrics"<<setw(20)<<left<<"Total"<<setw(20)<<left<<"Instrn"<<setw(20)<<left<<"Data"<<setw(20)<<left<<"Read"<<setw(20)<<left<<"Write"<<setw(20)<<left<<"Misc"<<endl;
    cout<<setw(40)<<left<<"______________________________"<<setw(20)<<left<<"_______________"<<setw(20)<<left<<"_______________"<<setw(20)<<left<<"_______________"<<setw(20)<<left
	<<"_______________"<<setw(20)<<left<<"_______________"<<setw(20)<<left<<"_______________"<<endl;
    double tot_sd2 = level_2->wareHouse[0]->accHowManyR + level_2->wareHouse[0]->accHowManyW;
    cout<<setw(40)<<left<<"Demand Fetches"<<setw(20)<<left<<fixed<<tot_sd2<<setw(20)<<left<<fixed<<level_2->wareHouse[0]->accHowManyI
	<<setw(20)<<left<<fixed<<(level_2->wareHouse[0]->accHowManyR+level_2->wareHouse[0]->accHowManyW)
	<<setw(20)<<left<<fixed<<level_2->wareHouse[0]->accHowManyR<<setw(20)<<left<<fixed<<level_2->wareHouse[0]->accHowManyW<<setw(20)<<left<<fixed<<misc<<endl;  
    cout<<setw(40)<<left<<" Fraction of Total"<<setw(20)<<setprecision(4)<<left<<fixed<<(tot_sd2/(float)tot_sd2)
	<<setw(20)<<setprecision(4)<<left<<fixed<<(level_2->wareHouse[0]->accHowManyI/(float)tot_sd2)
	<<setw(20)<<setprecision(4)<<left<<fixed<<((level_2->wareHouse[0]->accHowManyR+level_2->wareHouse[0]->accHowManyW)/(float)tot_sd2)
	<<setw(20)<<setprecision(4)<<left<<fixed<<(level_2->wareHouse[0]->accHowManyR/(float)tot_sd2)
	<<setw(20)<<setprecision(4)<<left<<fixed<<(level_2->wareHouse[0]->accHowManyW/(float)tot_sd2)
	<<setw(20)<<setprecision(4)<<left<<fixed<<fractMisc<<endl<<endl;
    cout<<setw(40)<<left<<"Demand Misses"<<setw(20)<<left<<fixed<<missesTotal2[0]<<setw(20)<<left<<fixed<<missesInstn2[0]<<setw(20)<<left<<fixed<<(missesDataWr2[0]+missesDataRd2[0])
	<<setw(20)<<left<<fixed<<missesDataRd2[0]<<setw(20)<<left<<fixed<<missesDataWr2[0]<<setw(20)<<left<<fixed<<missesMisc2[0]<<endl;
    cout<<setw(40)<<left<<" Demand miss rate"<<setw(20)<<setprecision(4)<<left<<fixed<<fractMissTotal2[0]<<setw(20)<<setprecision(4)<<left<<fixed<<fractMissInstn2[0]
	<<setw(20)<<setprecision(4)<<left<<fixed<<fractMissData2[0]<<setw(20)<<setprecision(4)<<left<<fixed<<fractMissDataRd2[0]<<setw(20)<<setprecision(4)<<left<<fixed<<fractMissDataWr2[0]
	<<setw(20)<<setprecision(4)<<left<<fixed<<fractMissMisc2[0]<<endl<<endl;
  }
  
  //Commenting out the extra-credit part:
  /*
    cout<<setw(40)<<left<<"  Compulsory Misses"<<setw(20)<<left<<Total1.Compul_miss<<setw(20)<<left<<Instrn1.Compul_miss<<setw(20)<<left<<Data1.Compul_miss<<setw(20)
      <<left<<Read1.Compul_miss<<setw(20)<<left<<Write1.Compul_miss<<setw(20)<<left<<Misc1.Compul_miss<<endl;

  cout<<setw(40)<<left<<"  Capacity Misses"<<setw(20)<<left<<Total1.Capac_miss<<setw(20)<<left<<Instrn1.Capac_miss<<setw(20)<<left<<Data1.Capac_miss<<setw(20)
      <<left<<Read1.Capac_miss<<setw(20)<<left<<Write1.Capac_miss<<setw(20)<<left<<Misc1.Capac_miss<<endl;

  cout<<setw(40)<<left<<"  Conflict Misses"<<setw(20)<<left<<Total1.Confl_miss<<setw(20)<<left<<Instrn1.Confl_miss<<setw(20)<<left<<Data1.Confl_miss<<setw(20)
      <<left<<Read1.Confl_miss<<setw(20)<<left<<Write1.Confl_miss<<setw(20)<<left<<Misc1.Confl_miss<<endl;

  */
  

}

