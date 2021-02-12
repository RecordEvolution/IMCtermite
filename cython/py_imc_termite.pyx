# distutils: language = c++

from imc_termite cimport imc_termite

import json as jn
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
    chnlstjn = [jn.loads(chn.decode()) for chn in chnlst]
    return chnlstjn

  # print a channels
  def print_channels(self, string outputdir):
    self.cpp_imc.print_channels(outputdir)
