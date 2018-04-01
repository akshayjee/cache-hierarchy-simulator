#include "stghold.h"
#include "cache.h"
#include "cmdCenter.h"
int cache::getWay(string & ass){
  int ans;
//HERE TO IMPLEMENT
  string ass_int;
  string::iterator it;
  if( !ass.empty() ) {
    for( it = ass.begin(); it != ass.end(); it++) {
      ass_int.push_back(*it);
    }
  }
  stringstream ss;
  ss<<std::dec<<ass_int;
  ss>>ans;
  return ans;
}

replacer * cache::createReplacer(){
  if (rep == "FIFO"){
    return( new FIFO(set,way));
  }
  else if(rep ==  "LRU"){
    return( new LRU(set,way));
  }
  else if(rep == "NMRU"){
    return( new NMRU(set,way));
  }
  else if(rep == "RND"){
    return( new RND(set,way));
  }
  else{
    return NULL;
  }
}

		  
void cache::askCacheInfo(scout & cachInfo){
//if fully
  if(cachInfo.assoc == "FULLY"){
    set = 1;
    way = cachInfo.cap/cachInfo.bsize;
  }
  else if(cachInfo.assoc == "DIRECT"){
    way = 1;
    set = cachInfo.cap/cachInfo.bsize;
  }
  else{
    way = getWay(cachInfo.assoc); //get way according to associativity
    int setTimesWay = cachInfo.cap/cachInfo.bsize;
    set = setTimesWay/way;
  }
  rep = cachInfo.rep;
  wralloc = cachInfo.wralloc;
  blockSize = cachInfo.bsize;
  wpRecorder = vector<vector<weapon> > (set);
  for (int i = 0; i < (int) wpRecorder.size();++i){
    wpRecorder[i] = vector<weapon> (way);
  }
  repPolicy = createReplacer();//create replacement class  
}

void cache::decoder::decodeAddr(string & oneLine, cache * Info){
  //read one line and store it
  //and decode that line;
  //figure out what index and tag is for this addr
  string line_addr (oneLine.begin()+2,oneLine.end());
  //Convert oneLine string from Hex to Binary:
  stringstream hexLine;
  string binaryLine;
  unsigned n;
  hexLine << hex << line_addr;
  hexLine >> n;
  bitset<32> b(n);
  binaryLine = b.to_string();
  
  //For a Fully Associative Cache:
  if (Info->set == 1) {
    string tempBin(binaryLine.begin(),binaryLine.end() - log2(Info->blockSize));
    tag = tempBin;
    index = 0;
  }
  
  //For an n-Way set associative Cache as well as Direct Mapped:
  else{
    string tempBin(binaryLine.begin(), binaryLine.end() - log2(Info->blockSize) - log2(Info->set));
    tag = tempBin;
    string index_s(binaryLine.end() - log2(Info->blockSize) - log2(Info->set), binaryLine.end() - log2(Info->blockSize));
    //stringstream index_ss (index_s);
    bitset <32> temp_string(index_s);
    stringstream index_ss;
    index_ss << temp_string.to_ulong();
    index_ss >> index;
  }
  line = oneLine;
}

int cache::isSetFull(int whichSet){  //This function check whether any of the block in certain set has invalid bit, if any, return that index,otherwise
                                     //return 0
  for(int i = 0; i < (int) wpRecorder[whichSet].size(); ++i){
    if (!wpRecorder[whichSet][i].isValid){
      return i;
    }
  }
  return -1;
}   
  
void cache::replaceBlock(int set, int index, cache::decoder & enigma, bool needDirty){
  //vector<weapon> weapon_line = wpRecorder[set];
  weapon replaceWeapon = wpRecorder[set][index];
  replaceWeapon.tag = enigma.tag;
  replaceWeapon.addr = enigma.line;
  replaceWeapon.isValid = 1;
  if(needDirty == true) {
    replaceWeapon.isDirty = 1;      //check dirty bit for different conditions 
  }
  else{
    replaceWeapon.isDirty = 0;
  }
  wpRecorder[set][index] = replaceWeapon;
}

int cache::tagComparator(int set, string tag){
  int bucket = 0;
  vector<weapon> weapon_line;
  weapon_line = wpRecorder[set];
  vector<weapon>::iterator it;
  for(it = weapon_line.begin(); it != weapon_line.end(); it++) {
    if(it->tag == tag) {
      return bucket;
    }
    bucket++;
  }
  return -1;
}

void cache::addMiss(int m){
  ++missTimes;
  switch(m) {
  case 0: ++dataReadMisses;
    break; 
  case 1: ++dataWriteMisses;
    break;
  case 2: ++instMisses;
    break;
  default: ++miscMisses;
    break;
  }
}

void cache::writeMissInvalid(cache::decoder & enigma, cmdCenter *cmd, int code, int whichBlock){
  addMiss(code);
  if(wralloc == true){
    if (level_cache == 2){
      cmd->accessTime+=cmd->memAccTime;
      replaceBlock(enigma.index,whichBlock,enigma,true);
      if (rep == "LRU" || rep == "NMRU"){ //Don't have this for direct map
	repPolicy->update(enigma.index,whichBlock);
      }
    }
    else{
      cmd->read(0,cmd->level_2,enigma.line); //bring the block into this level
      replaceBlock(enigma.index,whichBlock,enigma,true);
      if (rep == "LRU" || rep == "NMRU"){ //Don't have this for direct map
	repPolicy->update(enigma.index,whichBlock);
      }
    }
  }
  else{
    if (level_cache == 2) {
      //if this is level 2, then we have a write miss on level 2. It's not write allocate, we directly write into next level, in this case main memory
      //the way to do this is by incrementing main memory access time
      cmd->accessTime+=cmd->memAccTime;
    }
    else { //this means we have a write miss on level 1. And it is not write allocated, we directly write into next level, in this case level 2. NOTE that we will only write something into data cache. Instruction cache can never be written.
      cmd->write(code,cmd->level_2,enigma.line);      
    }
  }
}
void cache::writeMiss(cache::decoder & enigma, cmdCenter *cmd, int code){
  //Some code
  addMiss(code);
  //if allocate on write miss is yes(= true in bool) - do same as read miss
  if (wralloc == true) {
    //if this is level_2, we reach the end
    if (level_cache == 2){
      //this means we have a writeMiss in level_2
      //add main memory access Time
      cmd->accessTime+=cmd->memAccTime;
      //find the block to replace according to replace information
	int which2Kick = 0;
	if (repPolicy != NULL){ //set or fully associative
	  which2Kick = repPolicy->which2Replace(enigma.index);
	}
      //if dirty
	if(wpRecorder[enigma.index][which2Kick].isDirty){
	//write this block to next level(new task)
	//the way to write to mem is by adding access time
	  cmd->accessTime+=cmd->memAccTime;
	}
      //replace the block(overwrite)
	replaceBlock(enigma.index,which2Kick,enigma,true); //remember set to dirty!
      //update replacement information
	if(repPolicy != NULL){ //set or fully associative doesn't have replacement policy
	  repPolicy->update(enigma.index,which2Kick);
	}
    }
    //else(it's not level_2 cache(level_1))
    //we have a write miss on level_1 (and it's write allocated)
    else{
      //since it's write allocated, you need to first bring the block you are going to write to into this level's cache, the way to do this is by reading the same address from the next level. You will only write to data cache or unified cache, so you will only read from data cache or unified cache.
      cmd->read(0,cmd->level_2,enigma.line); //bring the block into this level
      //find the block to replace according to replace information
      int which2Kick = 0;
      if (repPolicy != NULL){
	which2Kick = repPolicy->which2Replace(enigma.index);
      }
      //if dirty
      if(wpRecorder[enigma.index][which2Kick].isDirty){
	//write to next level(new task)
	//int code = cmd->getCode(wpRecorder[engima.index][which2Kick].addr);
	cmd->write(1,cmd->level_2,wpRecorder[enigma.index][which2Kick].addr); //you will never write into instruction cache
      }
      //replace the block(overwrite)
      replaceBlock(enigma.index,which2Kick,enigma,true); //enigma.index->which2Kick //remember set to dirty
      //tag information is in enigma
      if (repPolicy != NULL){
	repPolicy->update(enigma.index, which2Kick);
      }
      //update replacement information
    }
  }
  
  //if allocate on write miss is NO 
  else {
    if (level_cache == 2) {
      //if this is level 2, then we have a write miss on level 2. It's not write allocate, we directly write into next level, in this case main memory
      //the way to do this is by incrementing main memory access time
	cmd->accessTime+=cmd->memAccTime;
    }
    else { //this means we have a write miss on level 1. And it is not write allocated, we directly write into next level, in this case level 2. NOTE that we will only write something into data cache. Instruction cache can never be written.
      cmd->write(code,cmd->level_2,enigma.line);      
    }
  }
}
void cache::readMissInvalid(cache::decoder & enigma, cmdCenter * cmd, int code, int whichBlock){
  addMiss(code);
  if(level_cache == 2){
    cmd->accessTime+=cmd->memAccTime;
    replaceBlock(enigma.index,whichBlock,enigma,false);
    if (rep == "LRU" || rep == "NMRU"){ //Don't have this for direct map
      repPolicy->update(enigma.index,whichBlock);
    }
  }
  else{
    cmd->read(code,cmd->level_2,enigma.line);
         //find the block to replace according to replace information
        //if this block is invalid, we don't use replacement Policy just replace this block immediately
    replaceBlock(enigma.index,whichBlock,enigma,false);
    if (rep == "LRU" || rep == "NMRU"){ //Don't have this for direct map
      repPolicy->update(enigma.index,whichBlock);
    }
  }
}
void cache::readMiss(cache::decoder & enigma,cmdCenter * cmd,int code){
  addMiss(code);
  //if this is level_2, we reach the end
  if (level_cache == 2){
  //this means we have a readMiss in level_2
    //add main memory access Time
    cmd->accessTime+=cmd->memAccTime; 
         //find the block to replace according to replace information
    //valid miss
    int which2Kick = 0;
    if(repPolicy != NULL){ //FULLY OR SET ASSOCIATIVE
      which2Kick = repPolicy->which2Replace(enigma.index);
    }         //if dirty
    if(wpRecorder[enigma.index][which2Kick].isDirty){
       //write this block to next level(new task)
      //the way to write to mem is by adding access time
      cmd->accessTime+=cmd->memAccTime;
    }
         //replace the block(overwrite)
    replaceBlock(enigma.index,which2Kick,enigma,false);
    //update replacement information
    if (repPolicy != NULL){ //Don't have this for direct
      repPolicy->update(enigma.index,which2Kick);
    }
  } 
  //else(it's not level_2 cache(level_1))
  //we miss on level_1
  else{
    cmd->read(code,cmd->level_2,enigma.line); //read from next level
    int which2Kick = 0;
    if (repPolicy != NULL){ //SET OR FULLY
      which2Kick = repPolicy->which2Replace(enigma.index);
    }
         //if dirty
    if(wpRecorder[enigma.index][which2Kick].isDirty){
	//write to next level(new task)
      //int code = cmd->getCode(wpRecorder[engima.index][which2Kick].addr); //you need code because you need to know what type of cache you need to write to
      //you will always write to data cache
      cmd->write(1,cmd->level_2,wpRecorder[enigma.index][which2Kick].addr);
    }
      //replace the block(overwrite)
    replaceBlock(enigma.index,which2Kick,enigma,false); //enigma.indx->which2Kick
      //tag information is in enigma
    if (repPolicy != NULL){ //SET OR FULLY
      repPolicy->update(enigma.index, which2Kick);
    }
  }
  //update replacement information  
}

void cache::readCache(int code, cmdCenter * cmd, string & line){
  //add hitTime to accessTime(because anyway this will access the cache of this level)
	cmd->accessTime = cmd->accessTime + hitTime;
	if (code == 0)
	  ++accHowManyR; //Accessing number + 1
	else
	  ++accHowManyI;
  //create a decoder
	decoder enigma;
  //decode the line,fill the information into decoder
	enigma.decodeAddr(line, this);
  //according to decoded info find the set we are looking for
  //compare to each tag in this set find which tag it is
  int whichBlock = tagComparator(enigma.index, enigma.tag);
  //if find one
  if (whichBlock != -1){
	  //if valid
    if(wpRecorder[enigma.index][whichBlock].isValid){
       //this is a hit
       //update replacement information if it's NMRU or LRU(FIFO and RND only need to be updated when kickout)
      if(rep == "NMRU" || rep == "LRU"){
	repPolicy->update(enigma.index,whichBlock);
      }	
	    //simply return
      return;
    }
  
    else{
      readMissInvalid(enigma,cmd,code,whichBlock);
    }
  //if not found or invalid then it's a miss
  }
  else{
    int whichBlock = isSetFull(enigma.index);
    if (whichBlock == -1){ //If there is still invalid block in this set
                           //that means the set is not full
                           //when we replace we don't follow replacement policy
      readMiss(enigma,cmd,code);
    }
    else{
      readMissInvalid(enigma,cmd,code,whichBlock);
    }
  }
}

void cache::writeCache(int code, cmdCenter * cmd, string & line){
  //add hitTime to accessTime(because anyway this will access the cache of this level)
  cmd->accessTime = cmd->accessTime + hitTime;
  ++accHowManyW; //Number of Accesses + 1
  //create a decoder
  decoder enigma;
  //decode the line,fill the information into decoder
  enigma.decodeAddr(line, this);
  //according to decoded info find the set we are looking for
  //compare to each tag in this set find which tag it is
  int whichBlock = tagComparator(enigma.index, enigma.tag);
  //if find one
  if (whichBlock != -1){
    //if valid
    if(wpRecorder[enigma.index][whichBlock].isValid){
      //this is a write hit, you write and change dirty bit to 1
      wpRecorder[enigma.index][whichBlock].isDirty = true;
      //update replacement information if it's NMRU or LRU(FIFO and RND only need to be updated when kickout)
      if(rep == "NMRU" || rep == "LRU"){
	repPolicy->update(enigma.index,whichBlock);
      }	
      //simply return
      return;
    }
    else{
      writeMissInvalid(enigma,cmd,code,whichBlock);
    }
  }
  //if not found or invalid then it's a miss
  else{
    int whichBlock = isSetFull(enigma.index);
    if (whichBlock == -1){ //If there is still invalid block in this set
                           //that means the set is not full
                           //when we replace we don't follow replacement policy
      writeMiss(enigma,cmd,code);
    }
    else{
      writeMissInvalid(enigma,cmd,code,whichBlock);
    }
    
  }
}

