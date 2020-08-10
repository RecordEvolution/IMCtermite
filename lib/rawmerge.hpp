//---------------------------------------------------------------------------//
//
//  @file rawmerge.hpp
//  @author Mario Fink <mario.fink@record-evolution.de>
//  @date Aug 2020
//  @brief unify multiple channels and time series from .raw in single table
//
//---------------------------------------------------------------------------//

#ifndef RAW_MERGER
#define RAW_MERGER

#include "raweat.hpp"

//---------------------------------------------------------------------------//

class raw_merger : public raw_eater
{
private:

  // number of channels
  int num_channels_;

  // (merged) channel data related to SINGLE time series
  std::vector<std::vector<double>> channels_;
  std::vector<double> timeseries_;

  // timestep of original timeseries
  double dt_;

  // temporal unit and channel meta data
  std::string temp_unit_;
  std::vector<std::string> channel_names_;

public:

  // constructor
  raw_merger(std::string rawfile): raw_eater(rawfile)
  {
    // add first initial channel
    this->add_channel(rawfile);
  }

  raw_merger(): raw_eater()
  {

  }

  // destructor
  ~raw_merger()
  {

  }

  // get list of channel names
  std::vector<std::string> get_channel_names()
  {
    return this->channel_names_;
  }

  // get unified timeseries
  std::vector<double> get_time_series()
  {
    return this->timeseries_;
  }

  // get array representation i-th channel
  std::vector<double> get_channel(long unsigned int chidx)
  {
    return chidx >= 0 && chidx < this->channels_.size() ?
      this->channels_[chidx] : std::vector<double>();
  }

  // add a single channel and its associated time series
  bool add_channel(std::string rawfile, bool log = false)
  {
    // set raw file and perform conversion
    this->set_file(rawfile,false);

    // show channel name, unit, timestep, time unit, etc.
    if ( log )
    {
      std::cout<<this->get_name()<<" ["<<this->get_unit()<<"]"<<"\n";
      std::cout<<"Time ["<<this->get_temp_unit()<<"]"<<"\n";
      for ( unsigned long int i = 0; i < 5; i++ ) std::cout<<this->get_time()[i]<<"\n";
      for ( unsigned long int i = 0; i < 5; i++ ) std::cout<<this->get_data()[i]<<"\n";
      std::cout<<"lenght of channel "<<this->get_time().size()<<"\n";
      std::cout<<"\n";
    }

    // add first/initial time series (and channel data)
    if ( this->get_valid() && timeseries_.size() == 0 && channels_.size() == 0 )
    {
      std::cout<<"adding initial channel "<<rawfile<<"\n\n";

      // insert timeseries and its unit
      this->timeseries_ = this->get_time();
      this->temp_unit_ = this->get_temp_unit();

      // get timestep size
      this->dt_ = this->get_dt();

      // insert channel data and its meta data
      this->channels_.push_back(this->get_data());
      this->channel_names_.push_back(this->get_name() + std::string(" [")
                                   + this->get_unit() + std::string("]"));

      return true;
    }
    else
    {
      std::cout<<"adding next channel "<<rawfile<<"\n\n";

      // check consistency of temporal unit
      if ( this->get_temp_unit() == this->temp_unit_ )
      {
        // get time series
        std::vector<double> ts = this->get_time();

        // compare start/end of timeseries (define tolerance)
        double deltat = 10*this->dt_;
        if ( ( this->timeseries_[0]     - ts[0]     < deltat  )
          && ( this->timeseries_.back() - ts.back() < deltat ) )
        {
          // submerge channels and their time series
          // TODO
          // this->merge_channels(this.timeseries_,this)

          // insert channel data and its meta data
          this->channels_.push_back(this->get_data());
          this->channel_names_.push_back(this->get_name() + std::string(" [")
                                       + this->get_unit() + std::string("]"));


          return true;
        }
        else
        {
          // refuse to merge due to inconsistent start of timeseries
          std::cerr<<"rawmerge: add_channel '"<<rawfile
                   <<"' : inconsistent start of time series, i.e. "
                   <<timeseries_[0]<<" vs. "<<ts[0]<<"\n";

          return false;
        }
      }
      else
      {
        // refuse to merge due to different temporal units
        std::cerr<<"rawmerge: add_channel '"<<rawfile
                 <<"' : inconsistent time units\n";

        return false;
      }
    }
  }

  // print all data to file
  void write_table(std::string filename, char delimiter = ',', int precision = 6, int width = 25)
  {
    // if at least one channel including its time series is present
    if ( timeseries_.size() > 0 && channels_.size() > 0 )
    {
      // open file
      std::ofstream fout(filename.c_str());

      if ( fout.good() )
      {
        // define temporal column header
        std::string colTime = std::string("Time [") + temp_unit_ + std::string("]");

        // write header
        if ( delimiter != ' ' )
        {
          fout<<colTime;
          for ( std::string chnam: this->channel_names_ ) fout<<delimiter<<chnam;
        }
        else
        {
          fout<<std::setw(width)<<std::right<<colTime;
          for ( std::string chnam: this->channel_names_ ) fout<<std::setw(width)<<std::right<<chnam;
        }
        fout<<"\n";

        // write actual time and channel data
        unsigned long int tidx = 0;
        for ( double ti: this->timeseries_ )
        {
          if ( delimiter != ' ' )
          {
            fout<<std::fixed<<std::dec<<std::setprecision(precision)<<ti;
            for ( std::vector<double> chn: this->channels_ )
            {
              fout<<delimiter<<std::dec<<std::setprecision(precision)<<chn[tidx];
            }
          }
          else
          {
            fout<<std::fixed<<std::dec<<std::setprecision(precision)
                <<std::setw(width)<<std::right<<ti;
            for ( std::vector<double> chn: this->channels_ )
            {
              fout<<std::fixed<<std::dec<<std::setprecision(precision)
                  <<std::setw(width)<<std::right<<chn[tidx];
            }
          }
          fout<<"\n";

          // count number of samples
          tidx++;
        }

        // close file
        fout.close();
      }
    }
  }

};

#endif

//---------------------------------------------------------------------------//
