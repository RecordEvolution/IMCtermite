//---------------------------------------------------------------------------//

#include <iomanip>
#include <iostream>
#include "../lib/raweat.hpp"

//---------------------------------------------------------------------------//

int main(int argc, char* argv[])
{
  std::cout<<"number of CLI-arguments: "<<argc<<"\n";
  for ( int i = 0; i < argc; i++ ) std::cout<<std::setw(5)<<i<<":   "<<argv[i]<<"\n";

  // check number of CLI arguments
  assert( argc >= 3 );

  // provide --help like return statement 
  if ( argc < 3 )
  {
    std::cout<<"\n"<<"Usage: ./eatit INPUTRAW_1 INPUTRAW_2 ...INPUTRAW_N OUTPUTFILE"<<"\n"
                   <<"Convert set of related files in imc-format .raw corresponding to same measurement to single plain text .csv"<<"\n"
                   <<"Example: ./eatit Druck_THZ_DK.raw pressure_Vacuum.raw druck_thz_dk.csv"<<"\n\n";
    return 0;
  }

  // get list of names/paths of raw-files from CLI argument
  std::vector<std::string> rawfiles;
  for (int i = 1; i < argc-1; i++ ) rawfiles.push_back(std::string(argv[i]));

  // collect arrays of data (without time!), first, last time + timestep, channel names + units
  std::vector<std::vector<double>> alldata;
  std::vector<std::vector<double>> timedata;
  std::vector<std::vector<std::string>> channelinfo;

  // process all .raw files provided
  for ( auto rawfile: rawfiles )
  {
    std::cout<<rawfile<<"\n";

    // declare instance of "raw_eater"
    raw_eater eatraw(rawfile);

    // get time-offset and timestep
    double toff = eatraw.get_time_offset();
    double dt = eatraw.get_dt();
    unsigned long int n = eatraw.get_n();

    // collect data arrays
    alldata.push_back(eatraw.get_data());

    // collect time info
    std::vector<double> channtime = {toff,toff+(n-1)*dt,dt};
    timedata.push_back(channtime);

    // collect channel info
    std::vector<std::string> channinfo = {eatraw.get_name(),eatraw.get_unit(),eatraw.get_temp_unit()};
    channelinfo.push_back(channinfo);
  }

  // number of channels
  int num_chann = (int)alldata.size();

  // make sure all files feature same time unit
  for ( int ch = 0; ch < num_chann-1; ch++ )
  {
    assert( channelinfo[ch][2] == channelinfo[ch+1][2] && "time unit apparently differs in some channels!" );
  }

  // open .csv-file for dumping all data
  std::ofstream fout(argv[argc-1]);
  
  // define output properties
  int width = 30;
  int doubleprec = 9;

  // insert header with column names
  std::string colA = std::string("Time [") + channelinfo[0][2] + std::string("]");
  fout<<std::setw(width)<<std::right<<colA;
  for ( int ch = 0; ch < num_chann; ch++ )
  {
    std::string colB = channelinfo[ch][0] + std::string(" [") + channelinfo[ch][1] + std::string("]");
    fout<<std::setw(width)<<std::right<<colB;
  }
  fout<<"\n";

  // set of "next" indices for all channels
  std::vector<unsigned long int> channidx(num_chann,0);

  // while times in any channel left
  bool sometimeleft = true;
  while ( sometimeleft )
  {
    // find this round's "earliest" time w.r.t. to all channels
    double firsttime = 1.0e10;
    for ( int ch = 0; ch < num_chann; ch++ )
    {
      // get time to process in this channel
      double chtimenow = timedata[ch][0] + channidx[ch] * timedata[ch][2];
      
      if ( chtimenow <= firsttime ) // and channidx[ch]+1 < alldata[ch].size() )
      {
         firsttime = chtimenow;
      }
    }

    // print time step 
    fout<<std::fixed<<std::dec<<std::setprecision(doubleprec)
        <<std::setw(width)<<std::right<<firsttime;
    
    std::cout<<std::setprecision(9)<<firsttime<<"  "<<sometimeleft<<" ";
    for ( int ch = 0; ch < num_chann; ch++ ) std::cout<<channidx[ch]<<" / "<<alldata[ch].size()<<" ";
    std::cout<<"\n";

    // reset continuation flag and try to find channel with remaining data
    sometimeleft = false;

    // during every round accumulate all data associated to subsequent time step
    for ( int ch = 0; ch < num_chann; ch++ )
    {
      // this channel's time step
      double chtimenow = timedata[ch][0] + channidx[ch] * timedata[ch][2];
      
      // get set of channels featuring next time step
      if ( chtimenow == firsttime && channidx[ch] < alldata[ch].size() )
      {
        // write actual data to file
        fout<<std::fixed<<std::dec<<std::setprecision(doubleprec)
            <<std::setw(width)<<std::right<<alldata[ch][channidx[ch]];
        
        // increment time index of channel
        channidx[ch] = channidx[ch] + 1;
      }
      else 
      {
        // this channel gets a null = empty string for this timestep
        fout<<std::setw(width)<<"";
      }

      // check continuation condition
      if ( channidx[ch] < alldata[ch].size() ) sometimeleft = true;
    }

    // add line break
    fout<<"\n";
  }

  // close .csv-file
  fout.close();

  return 0;
}

//---------------------------------------------------------------------------//
