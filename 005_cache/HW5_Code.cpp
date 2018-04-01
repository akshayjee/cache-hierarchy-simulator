#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <iomanip>
#include <cstring>
using namespace std;

class metrics {
public:
  int Dem_fetch, Dem_miss;
  int Compul_miss, Capac_miss, Confl_miss; 
  float Fract_total, Miss_rate;
  metrics():Dem_fetch(0),Dem_miss(0),Compul_miss(0),Capac_miss(0),Confl_miss(0),Fract_total(0),Miss_rate(0){}
};

int main()
{
  // Input Variables are:
  int Associativity[4], Block_size[4], Capacity[4];
  int L1_hit_time, L2_hit_time;
  char L1_config, L2_config, Allocate_wm[4], Repl_algo[4];
  string Dinero_trace;
  char * Curr_line = NULL;
  char * Line_addr = NULL;
  size_t Line_size = 0;
  int Trace_valid = 0;
  int Num_caches = 0;

  //Output Variables are:
  metrics Total1,Instrn1, Data1, Read1, Write1, Misc1;

  //Accepting Data from user here:
  cout << endl;
  cout << "Hello User, Please enter the following:\n\n";

  //Determine L1 Config and update the number of caches
  do {
  cout << "L1 configuration is (None/Split/Unified): ";
  cin >> L1_config;
  L1_config = tolower (L1_config);
  } while (L1_config != 'n' && L1_config != 's' && L1_config != 'u');
  if (L1_config == 's')
    Num_caches += 2;
  if (L1_config == 'u')
    Num_caches += 1;

  //Determine L2 Config and update the number of caches
  do {
  cout << "L2 configuration is (None/Split/Unified): ";
  cin >> L2_config;
  L2_config = tolower (L2_config);
  } while  (L2_config != 'n' && L2_config != 's' && L2_config != 'u');
  if (L2_config == 's')
    Num_caches += 2;
  if (L2_config == 'u')
    Num_caches += 1;

  //Receive input from user on A, B, C, Repl. Algo and AOWM for each cache
  //For clarity assume: #0 Data Caches L1, #1 Instruction Caches L1, #2 Data Caches L2, #3 Instruction Caches L2  
  //cin.clear() and cin.ignore() were picked up online - Used to ensure that input is correct
  for (int i = 0; i < Num_caches; i++) {
    cout << "\nEnter Details for Cache #" << i << endl;
    cout << "Associativity A" << i << ": ";
    while (!(cin >> Associativity[i])) {
      cout << "Associativity A" << i << ": ";
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cout << "Block Size B" << i << ": ";
    while (!(cin >> Block_size[i])) {
      cout << "Block Size B" << i << ": ";
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cout << "Capacity C" << i << ": ";
    while (!(cin >> Capacity[i])) {
      cout << "Capacity C" << i << ": ";
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    if (Associativity[i] != 1) {
      do {
	cout << "Replacement algorithm (LRU/RND/NMRU/FIFO) for Cache #" << i << ": ";
	cin >> Repl_algo[i];
	Repl_algo[i] = tolower(Repl_algo[i]);
      } while (Repl_algo[i] != 'l' && Repl_algo[i] != 'r' && Repl_algo[i] != 'n' && Repl_algo[i] != 'f');
    }
    else ;

    do {
      cout << "Allocate on write miss (Y/N) for Cache #" << i << ": ";
      cin >> Allocate_wm[i];
      Allocate_wm[i] = tolower(Allocate_wm[i]);
    } while (Allocate_wm[i] != 'y' && Allocate_wm[i] != 'n');
  }

  //Ask for L1 and L2 hit times if Caches are present
  cout << endl;
  if (L1_config != 'n') {
    cout << "L1 hit time: ";
    cin >> L1_hit_time;
    }
  if (L2_config != 'n') {
    cout << "L2 hit time: ";
    cin >> L2_hit_time;
  }

  //Accept Trace file from User and Open/Check if present
  FILE * f1;
  do {
  cout << "Specify the Trace file with Data in Dinero III Format: ";
  cin >> Dinero_trace;
  f1 = fopen (Dinero_trace.c_str(),"r");
  if (f1 == NULL) {
    fprintf(stderr,"Could not open specified Trace file. Please try again!\n\n");
    Trace_valid = 1;
  }
  else Trace_valid = 0;
  } while (Trace_valid == 1);

  //Define Hash Table:
  //Some code

  //Parse Data from Trace file
  while (getline(&Curr_line, &Line_size, f1)>=0) {
    Total1.Dem_fetch++;
    if (Curr_line[0] == '0'){
      Data1.Dem_fetch++;
      Read1.Dem_fetch++;
    }
    else if (Curr_line[0] == '1') {
      Data1.Dem_fetch++;
      Write1.Dem_fetch++;
    }
    else if (Curr_line[0] == '2')
      Instrn1.Dem_fetch++;
    else
      Misc1.Dem_fetch++;

    Line_addr = strdup (Curr_line);
    Curr_line = strsep (&Line_addr, " ");

    //Decide what to do if there's no address in the line (CORNER CASE)
    if (Line_addr == NULL)
      continue;

    /*At this point the variables you will need from the Trace file are stored as follows:
      The mode of the input line is in Curr_line[0], this is the first character of a char *
      The Hex address in the input line is in Line_addr, this is a char * */
   
    //Split Address into Tag/Index/Offset
    //Some code

    //Use provided data and update Hash Table:
    //Some code

  }

  //Update values of all metrics:
  Total1.Fract_total = Total1.Dem_fetch/(float)Total1.Dem_fetch;
  Instrn1.Fract_total = Instrn1.Dem_fetch/(float)Total1.Dem_fetch;
  Data1.Fract_total = Data1.Dem_fetch/(float)Total1.Dem_fetch;
  Read1.Fract_total = Read1.Dem_fetch/(float)Total1.Dem_fetch;
  Write1.Fract_total = Write1.Dem_fetch/(float)Total1.Dem_fetch;
  Misc1.Fract_total = Misc1.Dem_fetch/(float)Total1.Dem_fetch;

  //Display output from here:
  cout << endl << endl;
  cout<<setw(40)<<left<<"Metrics"<<setw(20)<<left<<"Total"<<setw(20)<<left<<"Instrn"<<setw(20)<<left<<"Data"<<setw(20)<<left<<"Read"<<setw(20)<<left<<"Write"<<setw(20)<<left<<"Misc"<<endl;

  cout<<setw(40)<<left<<"______________________________"<<setw(20)<<left<<"_______________"<<setw(20)<<left<<"_______________"<<setw(20)<<left<<"_______________"<<setw(20)<<left
      <<"_______________"<<setw(20)<<left<<"_______________"<<setw(20)<<left<<"_______________"<<endl;

  cout<<setw(40)<<left<<"Demand Fetches"<<setw(20)<<left<<Total1.Dem_fetch<<setw(20)<<left<<Instrn1.Dem_fetch<<setw(20)<<left<<Data1.Dem_fetch
      <<setw(20)<<left<<Read1.Dem_fetch<<setw(20)<<left<<Write1.Dem_fetch<<setw(20)<<left<<Misc1.Dem_fetch<<endl;

  cout<<setw(40)<<left<<" Fraction of Total"<<setw(20)<<setprecision(4)<<left<<Total1.Fract_total<<setw(20)<<setprecision(4)<<left<<Instrn1.Fract_total
      <<setw(20)<<setprecision(4)<<left<<Data1.Fract_total<<setw(20)<<setprecision(4)<<left<<Read1.Fract_total<<setw(20)<<setprecision(4)<<left<<Write1.Fract_total
      <<setw(20)<<setprecision(4)<<left<<Misc1.Fract_total<<endl;

  cout << endl;
  
  cout<<setw(40)<<left<<"Demand Misses"<<setw(20)<<left<<Total1.Dem_miss<<setw(20)<<left<<Instrn1.Dem_miss<<setw(20)<<left<<Data1.Dem_miss
      <<setw(20)<<left<<Read1.Dem_miss<<setw(20)<<left<<Write1.Dem_miss<<setw(20)<<left<<Misc1.Dem_miss<<endl;

  cout<<setw(40)<<left<<" Demand miss rate"<<setw(20)<<setprecision(4)<<left<<Total1.Miss_rate<<setw(20)<<setprecision(4)<<left<<Instrn1.Miss_rate
      <<setw(20)<<setprecision(4)<<left<<Data1.Miss_rate<<setw(20)<<setprecision(4)<<left<<Read1.Miss_rate<<setw(20)<<setprecision(4)<<left<<Write1.Miss_rate
      <<setw(20)<<setprecision(4)<<left<<Misc1.Miss_rate<<endl;

  cout<<setw(40)<<left<<"  Compulsory Misses"<<setw(20)<<left<<Total1.Compul_miss<<setw(20)<<left<<Instrn1.Compul_miss<<setw(20)<<left<<Data1.Compul_miss<<setw(20)
      <<left<<Read1.Compul_miss<<setw(20)<<left<<Write1.Compul_miss<<setw(20)<<left<<Misc1.Compul_miss<<endl;

  cout<<setw(40)<<left<<"  Capacity Misses"<<setw(20)<<left<<Total1.Capac_miss<<setw(20)<<left<<Instrn1.Capac_miss<<setw(20)<<left<<Data1.Capac_miss<<setw(20)
      <<left<<Read1.Capac_miss<<setw(20)<<left<<Write1.Capac_miss<<setw(20)<<left<<Misc1.Capac_miss<<endl;

  cout<<setw(40)<<left<<"  Conflict Misses"<<setw(20)<<left<<Total1.Confl_miss<<setw(20)<<left<<Instrn1.Confl_miss<<setw(20)<<left<<Data1.Confl_miss<<setw(20)
      <<left<<Read1.Confl_miss<<setw(20)<<left<<Write1.Confl_miss<<setw(20)<<left<<Misc1.Confl_miss<<endl;

  if (fclose(f1)!=0) {
    fprintf(stderr,"The file did not close properly!\n");
    exit (EXIT_FAILURE);
  }

  cout << endl << "Okay thank you running away with your data bye bye" << endl; 
}
