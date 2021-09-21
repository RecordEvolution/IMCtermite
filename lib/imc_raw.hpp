//---------------------------------------------------------------------------//

#ifndef IMCRAW
#define IMCRAW

#include <fstream>
#include <filesystem>
#include <iostream>

// #include "hexshow.hpp"
#include "imc_key.hpp"
#include "imc_block.hpp"
#include "imc_datatype.hpp"
#include "imc_object.hpp"
#include "imc_result.hpp"
#include "imc_channel.hpp"

//---------------------------------------------------------------------------//

namespace imc
{
  class raw
  {
    // (path of) raw-file and its basename
    std::string raw_file_, file_name_;

    // buffer of raw-file
    std::vector<unsigned char> buffer_;

    // list and map of imc-blocks
    std::vector<imc::block> rawblocks_;
    std::map<std::string,imc::block> mapblocks_;

    // check computational complexity for parsing blocks
    unsigned long int cplxcnt_;

    // list groups and channels (including their affiliate blocks)
    std::map<std::string,imc::channel> channels_;

  public:

    // constructor
    raw() { };
    raw(std::string raw_file): raw_file_(raw_file) { set_file(raw_file); };

    // provide new raw-file
    void set_file(std::string raw_file)
    {
      raw_file_ = raw_file;
      this->fill_buffer();
      this->parse_blocks();
      this->generate_block_map();
      this->generate_channel_env();
    }

  private:

    // open file and stream data into buffer
    void fill_buffer()
    {
      buffer_.clear();

      // open file and put data in buffer
      try {
        std::ifstream fin(raw_file_.c_str(),std::ifstream::binary);
        if ( !fin.good() ) throw std::runtime_error("failed to open file");
        std::vector<unsigned char> buffer((std::istreambuf_iterator<char>(fin)),
                                          (std::istreambuf_iterator<char>()));
        buffer_ = buffer;
        fin.close();
      } catch ( const std::exception& e ) {
        throw std::runtime_error(
          std::string("failed to open raw-file and stream data in buffer: ") + e.what()
        );
      }
    }

    // parse all raw blocks in buffer
    void parse_blocks()
    {
      rawblocks_.clear();

      // reset counter to identify computational complexity
      cplxcnt_ = 0;

      // start parsing raw-blocks in buffer
      for ( std::vector<unsigned char>::iterator it=buffer_.begin();
                                                 it!=buffer_.end(); ++it )
      {
        cplxcnt_++;

        // check for "magic byte"
        if ( *it == ch_bgn_ )
        {
          // check for (non)critical key
          if ( *(it+1) == imc::key_crit_ || *(it+1) == imc::key_non_crit_ )
          {
            // compose (entire) key
            std::string newkey = { (char)*(it+1), (char)*(it+2) };
            imc::key itkey(*(it+1) == imc::key_crit_,newkey);

            // expecting ch_sep_ after key
            if ( *(it+3) == ch_sep_ )
            {
              // extract key version
              std::string vers("");
              unsigned long int pos = 4;
              while ( *(it+pos) != ch_sep_ )
              {
                vers.push_back((char)*(it+pos));
                pos++;
              }
              int version = std::stoi(vers);

              // try to retrieve full key
              itkey.version_ = version;
              itkey = imc::get_key(itkey.critical_,itkey.name_,itkey.version_);

              // check for known keys (including version)
              if ( imc::check_key(itkey) )
              {
                // get block length
                std::string leng("");
                pos++;
                while ( *(it+pos) != ch_sep_ )
                {
                  leng.push_back((char)*(it+pos));
                  pos++;
                }
                unsigned long int length = std::stoul(leng);

                // declare and initialize corresponding key and block
                // imc::key bkey( *(it+1)==imc::key_crit_ , newkey,
                //                imc::keys.at(newkey).description_, version );
                imc::block blk(itkey,(unsigned long int)(it-buffer_.begin()),
                                     (unsigned long int)(it-buffer_.begin()+pos+1+length),
                                     raw_file_, &buffer_);

                // add block to list
                rawblocks_.push_back(blk);

                // skip the remaining block according to its length
                if ( (unsigned long int)(it-buffer_.begin()+length) < (unsigned long int)(buffer_.size()) )
                {
                  std::advance(it,length);
                }
              }
              else
              {
                // all critical must be known !! while a noncritical may be ignored
                if ( *(it+1) == imc::key_crit_ )
                {
                  throw std::runtime_error(
                    std::string("unknown critical key: ") + newkey + std::to_string(version)
                  );
                }
                else
                {
                  std::cout<<"WARNING: unknown noncritical key '"
                           <<newkey<<version<<"' will be ignored\n";
                }
              }
            }
            else
            {
              throw std::runtime_error(
                  std::string("invalid block or corrupt buffer at byte: ")
                + std::to_string(it+3-buffer_.begin())
              );
            }
          }
        }
      }

      this->check_consistency();
    }

    // check consistency of blocks
    void check_consistency()
    {
      for ( unsigned long int b = 0; b < this->rawblocks_.size()-1 && this->rawblocks_.size() > 0; b++ )
      {
        if ( this->rawblocks_[b].get_end() >= this->rawblocks_[b+1].get_begin() )
        {
          throw std::runtime_error(
            std::string("inconsistent subsequent blocks:\n")
            + std::to_string(b) + std::string("-th block:\n") + this->rawblocks_[b].get_info()
            + std::string("\n")
            + std::to_string(b+1) + std::string("-th block:\n") + this->rawblocks_[b+1].get_info() );
        }
      }
    }

    // generate map of blocks using their uuid
    void generate_block_map()
    {
      mapblocks_.clear();

      for ( imc::block blk: rawblocks_ )
      {
        mapblocks_.insert( std::pair<std::string,imc::block>(blk.get_uuid(),blk) );
      }
    }

    // generate channel "environments"
    void generate_channel_env()
    {
      channels_.clear();

      // declare single channel environment
      imc::channel_env chnenv;
      chnenv.reset();

      // collect affiliate blocks for every channel WITH CHANNEL and AFFILIATE
      // BLOCK CORRESPONDENCE GOVERNED BY BLOCK ORDER IN BUFFER!!
      for ( imc::block blk: rawblocks_ )
      {
        if ( blk.get_key().name_ == "CN" ) chnenv.CNuuid_ = blk.get_uuid();
        else if ( blk.get_key().name_ == "CD" ) chnenv.CDuuid_ = blk.get_uuid();
        else if ( blk.get_key().name_ == "CT" ) chnenv.CTuuid_ = blk.get_uuid();
        else if ( blk.get_key().name_ == "Cb" ) chnenv.Cbuuid_ = blk.get_uuid();
        else if ( blk.get_key().name_ == "CP" ) chnenv.CPuuid_ = blk.get_uuid();
        else if ( blk.get_key().name_ == "CR" ) chnenv.CRuuid_ = blk.get_uuid();
        else if ( blk.get_key().name_ == "CS" ) chnenv.CSuuid_ = blk.get_uuid();
        else if ( blk.get_key().name_ == "NT" ) chnenv.NTuuid_ = blk.get_uuid();
        else if ( blk.get_key().name_ == "NO" ) chnenv.NOuuid_ = blk.get_uuid();
        else if ( blk.get_key().name_ == "NL" ) chnenv.NLuuid_ = blk.get_uuid();

        // check for currently associated channel
        if ( !chnenv.CNuuid_.empty() )
        {
          // any component/channel is closed by any of {CS, CC, CG, CB}
          if ( blk.get_key().name_ == "CS" || blk.get_key().name_ == "CC"
            || blk.get_key().name_ == "CG" || blk.get_key().name_ == "CB" )
          {
            // provide UUID for channel
            chnenv.uuid_ = chnenv.CNuuid_;

            // create channel object and add it to the map of channels
            channels_.insert( std::pair<std::string,imc::channel>
              (chnenv.CNuuid_,imc::channel(chnenv,&mapblocks_,&buffer_))
            );

            // reset channel uuid
            chnenv.CNuuid_.clear();
          }
        }

        // in contrast to component closed by CS block the blocks CB, CG, CC
        // already belong to NEXT component
        if ( blk.get_key().name_ == "CB" ) chnenv.CBuuid_ = blk.get_uuid();
        else if ( blk.get_key().name_ == "CG" ) chnenv.CGuuid_ = blk.get_uuid();
        else if ( blk.get_key().name_ == "CC" ) chnenv.CCuuid_ = blk.get_uuid();
      }
    }


  public:

    // provide buffer size
    unsigned long int buffer_size()
    {
      return (unsigned long int)buffer_.size();
    }

    // get blocks
    std::vector<imc::block>& blocks()
    {
      return rawblocks_;
    }

    // get computational complexity
    unsigned long int& computational_complexity()
    {
      return cplxcnt_;
    }

    // get list of channels with metadata
    std::vector<std::string> get_channels(bool json = false, bool include_data = false)
    {
      std::vector<std::string> chns;
      for ( std::map<std::string,imc::channel>::iterator it = channels_.begin();
                                                         it != channels_.end(); ++it)
      {
        if ( !json )
        {
          chns.push_back(it->second.get_info());
        }
        else
        {
          chns.push_back(it->second.get_json(include_data));
        }
      }
      return chns;
    }

    // get particular channel including data by its uuid
    imc::channel get_channel(std::string uuid)
    {
      if ( channels_.count(uuid) )
      {
        return channels_.at(uuid);
      }
      else
      {
        throw std::runtime_error(std::string("channel does not exist:") + uuid);
      }
    }

    // list a particular type of block
    std::vector<imc::block> list_blocks(const imc::key &mykey)
    {
      std::vector<imc::block> myblocks;
      for ( imc::block blk: this->rawblocks_ )
      {
        if ( blk.get_key() == mykey ) myblocks.push_back(blk);
      }
      return myblocks;
    }

    // list all groups (associated to blocks "CB")
    std::vector<imc::block> list_groups()
    {
      return this->list_blocks(imc::get_key(true,"CB"));
    }

    // list all channels
    std::vector<std::string> list_channels()
    {
      std::vector<std::string> channels;
      for ( imc::block blk: this->rawblocks_ )
      {
        if ( blk.get_key() == imc::get_key(true,"CN") )
        {
          imc::parameter prm = blk.get_parameters()[6];
          channels.push_back(blk.get_parameter(prm));
        }
      }

      return channels;
    }

    // print single specific channel
    void print_channel(std::string channeluuid, std::string outputfile, const char sep)
    {
      // check for given parent directory of output file
      std::filesystem::path pdf = outputfile;
      if ( !std::filesystem::is_directory(pdf.parent_path()) )
      {
        throw std::runtime_error(std::string("required directory does not exist: ")
                                 + pdf.parent_path().u8string() );
      }

      // find channel with given name
      if ( channels_.count(channeluuid) == 1 )
      {
        channels_.at(channeluuid).print(outputfile,sep);
      }
      else
      {
        throw std::runtime_error(std::string("channel does not exist:")
                                 + channeluuid);
      }
    }

    // print all channels into given directory
    void print_channels(std::string output, const char sep)
    {
      // check for given directory
      std::filesystem::path pd = output;
      if ( !std::filesystem::is_directory(pd) )
      {
        throw std::runtime_error(std::string("given directory does not exist: ")
                                 + output);
      }

      for ( std::map<std::string,imc::channel>::iterator it = channels_.begin();
                                                         it != channels_.end(); ++it)
      {
        // construct filename
        std::string chid = std::string("channel_") + it->first;
        std::string filenam = it->second.name_.empty() ? chid + std::string(".csv")
                                           : it->second.name_ + std::string(".csv");
        std::filesystem::path pf = pd / filenam;

        // and print the channel
        it->second.print(pf.u8string(),sep);
      }
    }

  };

}

#endif

//---------------------------------------------------------------------------//
