//---------------------------------------------------------------------------//

#ifndef IMCRAW
#define IMCRAW

#include <fstream>
// #include <filesystem>

#include "hexshow.hpp"
#include "imc_key.hpp"
#include "imc_block.hpp"
#include "imc_datatype.hpp"
#include "imc_object.hpp"

//---------------------------------------------------------------------------//

namespace imc
{
  class raw
  {
    // (path of) raw-file and its basename
    std::string raw_file_, file_name_;

    // buffer of raw-file
    std::vector<unsigned char> buffer_;

    // list of imc-blocks
    std::vector<imc::block> rawblocks_;

    // check computational complexity for parsing blocks
    unsigned long int cplxcnt_;

    // collect meta-information, channel definition, etc.
    std::vector<imc::keygroup> keygroups_;

  public:

    // constructor
    raw() {};
    raw(std::string raw_file): raw_file_(raw_file) { set_file(raw_file); };

    // provide new raw-file
    void set_file(std::string raw_file)
    {
      raw_file_ = raw_file;
      this->fill_buffer();
      this->parse_blocks();
    }

  private:

    // open file and stream data into buffer
    void fill_buffer()
    {
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
            // compose entire key
            std::string newkey = { (char)*(it+1), (char)*(it+2) };

            // check for known keys
            if ( keys.count(newkey) == 1 )
            {
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

                // get block length
                std::string leng("");
                pos++;
                while ( *(it+pos) != ch_sep_ )
                {
                  leng.push_back((char)*(it+pos));
                  pos++;
                }
                unsigned long length = std::stoul(leng);

                // declare and initialize corresponding key and block
                imc::key bkey( *(it+1)==imc::key_crit_ , newkey,
                               imc::keys.at(newkey).description_, version );
                imc::block blk(bkey,it-buffer_.begin(),
                                    it-buffer_.begin()+pos+1+length,
                                    raw_file_, &buffer_);

                // add block to list
                rawblocks_.push_back(blk);

                // skip the remaining block according to its length
                if ( it-buffer_.begin()+length < buffer_.size() )
                {
                  std::advance(it,length);
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
            else
            {
              throw std::runtime_error(std::string("unknown IMC key: ") + newkey);
            }
          }
        }
      }

      this->check_consistency();
    }

    // check consistency of blocks
    void check_consistency()
    {
      for ( unsigned long int b = 0; b < this->rawblocks_.size()-1; b++ )
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

    // parse channel's raw data

  public:

    // provide buffer size
    unsigned long int buffer_size()
    {
      return buffer_.size();
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

  };

}

#endif

//---------------------------------------------------------------------------//
