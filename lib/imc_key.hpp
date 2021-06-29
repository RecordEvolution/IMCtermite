//---------------------------------------------------------------------------//

#ifndef IMCKEY
#define IMCKEY

#include <iomanip>
#include <map>
#include <string>
#include <sstream>
#include <vector>

//---------------------------------------------------------------------------//

namespace imc
{
  // define "magic bytes" announcing critical (=C) non-critical keys (=N)
  const unsigned char key_crit_ = 0x43, key_non_crit_ = 0x4e;

  // define properties of marker/key
  struct key
  {
    // (non)critical key
    bool critical_;
    // name of key (two uppercase letters)
    std::string name_;
    // short description
    std::string description_;
    // version of marker
    int version_;

    // constructor
    key(bool critical, std::string name,
        std::string description = std::string(""), int version = -1)
    {
      critical_ = critical;
      if ( name.size() != 2 ) throw std::logic_error("invalid key name");
      name_ = name;
      description_ = description;
      version_ = version;
    }

    // comparison operator
    bool operator==(const key& akey)
    {
      return ( this->critical_ == akey.critical_
            && this->name_ == akey.name_
            && this->description_ == akey.description_
            && this->version_ == akey.version_ );
    }

    // get info string
    std::string get_info(int width = 20)
    {
      std::stringstream ss;
      ss<<std::setw(width)<<std::left<<"critical:"<<(critical_?"yes":"no")<<"\n"
        <<std::setw(width)<<std::left<<"name:"<<name_<<"\n"
        <<std::setw(width)<<std::left<<"description:"<<description_<<"\n"
        <<std::setw(width)<<std::left<<"version:"<<version_<<"\n";
      return ss.str();
    }

  };

  const std::vector<key> keys = {

    // critical keys
    key(true,"CF","format version and processor",2),
    key(true,"CK","start of group of keys",1),
    key(true,"CB","group of channels",1),
    key(true,"CT","text definition",1),
    key(true,"CG","group of components",1),
    key(true,"CD","abscissa description",1),
    key(true,"CD","abscissa description",2),
    key(true,"CZ","scaling of z-axis",1),
    key(true,"CC","start of component",1),
    key(true,"CP","buffer, datatype and samples of component",1),
    key(true,"Cb","buffer description",1),
    key(true,"CR","permissible range of values in component",1),
    key(true,"CN","name and comment of channel",1),
    key(true,"CS","raw binary data",1),
    key(true,"CI","single numerical value",1),
    key(true,"Ca","add reference key",1),

    // noncritical keys
    key(false,"NO","origin of data",1),
    key(false,"NT","timestamp of trigger",1),
    key(false,"ND","(color) display properties",1),
    key(false,"NU","user defined key",1),
    key(false,"Np","property of channel",1),
    key(false,"NE","extraction rule for BUS channels",1),
    key(false,"NL","language info and code page",1)

  };

  // check for existence of specific key
  bool check_key(key& mykey)
  {
    for ( key ky: keys ) if ( mykey == ky ) return true;
    return false;
  }

  // get key (with respect to name and evtl. version)
  key get_key(bool critical, std::string name, int version = -1)
  {
    // check validity of required key name
    if ( name.size() > 2 || name.size() < 1 )
    {
      throw std::runtime_error(std::string("invalid key name: ") + name);
    }

    // declare new key with available data
    key mykey(critical,name,std::string(""),version);

    // try to find matching key in list of predefined keys
    for ( key ky: keys )
    {
      if ( critical == ky.critical_ && name == ky.name_
           && ( version == ky.version_ || version == -1 ) )
      {
        mykey = ky;

        // provide first match
        return mykey;
      }
    }

    return mykey;
  }


}

#endif

//---------------------------------------------------------------------------//
