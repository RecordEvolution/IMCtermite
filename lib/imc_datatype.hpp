//---------------------------------------------------------------------------//

#ifndef IMCDATATYPE
#define IMCDATATYPE

//---------------------------------------------------------------------------//

namespace imc
{
  // map datatype to machine's datatypes
  typedef unsigned char imc_Ubyte;
  typedef signed char imc_Sbyte;
  //
  typedef unsigned short imc_Ushort;
  typedef signed short imc_Sshort;
  //
  // e.g. ARM Cortex-A72 armv7l gcc version 10.2.0 (Ubuntu 10.2.0-13ubuntu1)
  // #ifdef __arm__
  typedef unsigned long int imc_Ulongint;
  typedef signed long int imc_Slongint;
  // e.g. Intel(R) Core(TM) i7-3520M CPU @ 2.90GHz x86_64 gcc version 10.2.0 (Ubuntu 10.2.0-13ubuntu1)
  // #ifdef i386 __i386 __i386__
  // typedef unsigned int imc_Ulongint;
  // typedef signed int imc_Slongint;
  //
  typedef float imc_float;
  typedef double imc_double;
  //
  // TODO not all remaining types are supported yet
  // typedef <whatever that is ->... > "imc Devices Transitional Recording"
  // typedf <sometimestamptype>  "Timestamp Ascii"
  typedef char16_t imc_digital;
  // typedef <  > imc_sixbyte "6byte unsigned long"

  class datatype
  {
  protected:
    imc_Ubyte ubyte_;      // 0
    imc_Sbyte sbyte_;      // 1
    imc_Ushort ushort_;    // 2
    imc_Sshort sshort_;    // 3
    imc_Ulongint ulint_;   // 4
    imc_Slongint slint_;   // 5
    imc_float sfloat_;     // 6
    imc_double sdouble_;   // 7
    imc_digital sdigital_; // 10
    short int dtidx_;      // \in \{0,...,7,10\}
  public:
    datatype(): ubyte_(0), sbyte_(0),
                ushort_(0), sshort_(0),
                ulint_(0), slint_(0),
                sfloat_(0.0), sdouble_(0.0),
                sdigital_(0),
                dtidx_(0) { };
    // every supported datatype gets its own constructor
    datatype(imc_Ubyte num): ubyte_(num), dtidx_(0) {};
    datatype(imc_Sbyte num): sbyte_(num), dtidx_(1) {};
    datatype(imc_Ushort num): ushort_(num), dtidx_(2) {};
    datatype(imc_Sshort num): sshort_(num), dtidx_(3) {};
    datatype(imc_Ulongint num): ulint_(num), dtidx_(4) {};
    datatype(imc_Slongint num): slint_(num), dtidx_(5) {};
    datatype(imc_float num): sfloat_(num), dtidx_(6) {};
    datatype(imc_double num): ubyte_(0), sbyte_(0), ushort_(0), sshort_(0),
                              ulint_(0), slint_(0), sfloat_(0.0), sdouble_(num),
                              sdigital_(0), dtidx_(7) {};
    datatype(imc_digital num): ubyte_(0), sbyte_(0), ushort_(0), sshort_(0),
                               ulint_(0), slint_(0), sfloat_(0.0), sdouble_(num),
                               sdigital_(num), dtidx_(10) {};

    // identify type
    short int& dtype() { return dtidx_; }

    // copy constructor
    datatype(const datatype &num)
    {
      this->ubyte_ = num.ubyte_;
      this->sbyte_ = num.sbyte_;
      this->ushort_ = num.ushort_;
      this->sshort_ = num.sshort_;
      this->ulint_ = num.ulint_;
      this->slint_ = num.slint_;
      this->sfloat_ = num.sfloat_;
      this->sdouble_ = num.sdouble_;
      this->sdigital_ = num.sdigital_;
      this->dtidx_ = num.dtidx_;
    }

    // overall assignment operator
    datatype& operator=(const datatype &num)
    {
      if ( this != &num )
      {
        this->ubyte_ = num.ubyte_;
        this->sbyte_ = num.sbyte_;
        this->ushort_ = num.ushort_;
        this->sshort_ = num.sshort_;
        this->ulint_ = num.ulint_;
        this->slint_ = num.slint_;
        this->sfloat_ = num.sfloat_;
        this->sdouble_ = num.sdouble_;
        this->sdigital_ = num.sdigital_;
        this->dtidx_ = num.dtidx_;
      }

  		return *this;
    }

    // implement assignment operator for individual datatypes
    datatype& operator=(const imc_Ubyte &num)
    {
      this->ubyte_ = num;
      this->dtidx_ = 0;
      return *this;
    }
    datatype& operator=(const imc_Sbyte &num)
    {
      this->sbyte_ = num;
      this->dtidx_ = 1;
      return *this;
    }
    datatype& operator=(const imc_Ushort &num)
    {
      this->ushort_ = num;
      this->dtidx_ = 2;
      return *this;
    }
    datatype& operator=(const imc_Sshort &num)
    {
      this->sshort_ = num;
      this->dtidx_ = 3;
      return *this;
    }
    datatype& operator=(const imc_Ulongint &num)
    {
      this->ulint_ = num;
      this->dtidx_ = 4;
      return *this;
    }
    datatype& operator=(const imc_Slongint &num)
    {
      this->slint_ = num;
      this->dtidx_ = 5;
      return *this;
    }
    datatype& operator=(const imc_float &num)
    {
      this->sfloat_ = num;
      this->dtidx_ = 6;
      return *this;
    }
    datatype& operator=(const imc_double &num)
    {
      this->sdouble_ = num;
      this->dtidx_ = 7;
      return *this;
    }
    datatype& operator=(const imc_digital &num)
    {
      this->sdigital_ = num;
      this->dtidx_ = 10;
      return *this;
    }

    // obtain number as double
    double as_double()
    {
      double num = 0.0;
      if ( dtidx_ == 0 ) num = (double)ubyte_;
      else if ( dtidx_ == 1 ) num = (double)sbyte_;
      else if ( dtidx_ == 2 ) num = (double)ushort_;
      else if ( dtidx_ == 3 ) num = (double)sshort_;
      else if ( dtidx_ == 4 ) num = (double)ulint_;
      else if ( dtidx_ == 5 ) num = (double)slint_;
      else if ( dtidx_ == 6 ) num = (double)sfloat_;
      else if ( dtidx_ == 7 ) num = (double)sdouble_;
      else if ( dtidx_ == 10 ) num = static_cast<double>(sdigital_);
      return num;
    }

    // define custom stream operator to print the correct type
    friend std::ostream& operator<<(std::ostream& out, const datatype& num)
    {
      if ( num.dtidx_ == 0 ) out<<num.ubyte_;
      else if ( num.dtidx_ == 1 ) out<<num.sbyte_;
      else if ( num.dtidx_ == 2 ) out<<num.ushort_;
      else if ( num.dtidx_ == 3 ) out<<num.sshort_;
      else if ( num.dtidx_ == 4 ) out<<num.ulint_;
      else if ( num.dtidx_ == 5 ) out<<num.slint_;
      else if ( num.dtidx_ == 6 ) out<<num.sfloat_;
      else if ( num.dtidx_ == 7 ) out<<num.sdouble_;
      else if ( num.dtidx_ == 10 ) out<<static_cast<double>(num.sdigital_);
      return out;
    }

  };


}

#endif

//---------------------------------------------------------------------------//
