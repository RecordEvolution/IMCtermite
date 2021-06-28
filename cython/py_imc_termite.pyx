# distutils: language = c++

from imc_termite cimport imc_termite

import json as jn
import decimal
# import numpy as np

cdef class imctermite:

  # C++ instance of class => stack allocated (requires nullary constructor!)
  cdef imc_termite cpp_imc

  # constructor
  def __cinit__(self, string rawfile):
    self.cpp_imc = imc_termite(rawfile)

  # provide raw file
  def submit_file(self,string rawfile):
    self.cpp_imc.set_file(rawfile)

  # get JSON list of channels
  def get_channels(self, bool data):
    chnlst = self.cpp_imc.get_channels(True,data)
    chnlstjn = [jn.loads(chn.decode(errors="ignore")) for chn in chnlst]
    return chnlstjn

  # print single channel/all channels
  def print_channel(self, string channeluuid, string outputfile, char delimiter):
    self.cpp_imc.print_channel(channeluuid,outputfile,delimiter)
  def print_channels(self, string outputdir, char delimiter):
    self.cpp_imc.print_channels(outputdir,delimiter)

  # print table including channels
  def print_table(self, string outputfile):
    chnlst = self.cpp_imc.get_channels(True,True)
    chnlstjn = [jn.loads(chn.decode(errors="ignore")) for chn in chnlst]
    with open(outputfile.decode(),'w') as fout:
      for chn in chnlstjn:
        fout.write('#' +str(chn['xname']).rjust(19)+str(chn['yname']).rjust(20)+'\n')
        fout.write('#'+str(chn['xunit']).rjust(19)+str(chn['yunit']).rjust(20)+'\n')
        for n in range(0,len(chn['ydata'])):
          fout.write(str(chn['xdata'][n]).rjust(20)+
                     str(chn['ydata'][n]).rjust(20)+'\n')
