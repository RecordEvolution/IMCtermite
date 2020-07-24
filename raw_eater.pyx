
from raw_eater cimport raweater
import numpy as np
import re

cdef class raweater:

    # pointer to C++ instance (since there's no nullary constructor)
    cdef raw_eater *rawit

    def __cinit__(self, string rawfile):
        self.rawit = new raw_eater(rawfile)

    def __dealloc__(self):
        del self.rawit

    def validity(self):
        return self.rawit.get_valid()

    def channel_name(self):
        return self.rawit.get_name()

    def unit(self):
        return self.rawit.get_unit()

    def dt(self):
        return self.rawit.get_dt()

    def time_unit(self):
        return self.rawit.get_temp_unit()

    def get_time(self):
        return self.rawit.get_time()

    def get_channel(self):
        return self.rawit.get_data()

    def write_table(self, const char* csvfile, char delimiter):
        return self.rawit.write_table(csvfile,delimiter)
