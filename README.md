
# raw_eater

The _raw_eater_ package is used to parse files with extension `*.raw`, which 
are usually binary files produced by the labsoftware _Famos_ to dump measurement
time series.

## File Structure

The binary `*.raw` file features a series of markers that indicate the starting
point of various blocks of information. Every markers is introduced by character
"|" = `0x 7c` followed by two uppercase letters, which characterize the type of 
marker. The following markers are defined:
  
1. CF (0x 43 46)
1. CK (0x 43 4b)
1. NO (0x 4e 4f)
1. CG (0x 43 47)
1. CD (0x 43 44)
1. NT (0x 4e 54)
1. CC (0x 43 43)
1. CP (0x 43 50)
1. CR (0x 43 52)
1. CN (0x 43 4e)
1. Cb (0x 43 62)
1. CS (0x 43 53)

Each of these markers are followed by multiple commata (0x 2c) separated parameters
and are terminated by a semicolon `;` = 0x 3b, except for the sequence following 
the data marker CS, that may have any number of 0x3b occurencies, while still
terminated by a semicolon at the very end of the file (since CS is the last marker
section in the file). The markers have the following meaning:

- *CF* (mostly 4 parameters) 
       this marker is mostly just `|CF,2,1,1;` and hence I've got no fucking 
       idea what it actually means! 
- *CK* (mostly 4 parameters)
       same problem for this one: it always seems to be `|CK,1,3,1,1;` ...  
- *NO* (mostly 6 parameters)
       provides some info about the software package/device and its version that 
       produced the file, e.g. something like 
       `|NO,1,85,0,77,imc STUDIO 5.0 R3 (10.09.2015)@imc DEVICES 2.8R7 (26.8.2015)@imcDev__15190567,0,;`
- *CG* (mostly 5 parameters)
       another one of these apparently useless markers, looks for instance like 
       `|CG,1,5,1,1,1;`
- *CD* (mostly 11 parameters)
       since we're dealing with measured entities from the lab this markers contains
       info about the measurement frequency, i.e. sample rate. For instance
       `|CD,2,  63,  5.0000000000000001E-03,1,1,s,0,0,0,  0.0000000000000000E+00,1;`
       indicates a measured entity every 0.005 seconds, i.e. a sample rate = 200Hz
- *NT* (mostly 8 parameters)
        whatever ?!? for instance `|NT,1,16,1,1,1980,0,0,0.0;` 
         maybe it's the datatype ??
- *CC*  (mostly 4 parameters)
        `|CC,1,3,1,1;`
- *CP*  (mostly 10 parameters)
        `|CP,1,16,1,4,7,32,0,0,1,0;`
- *CR*  (mostly 8 parameters)
         provides the _physical unit_ of the measured entity, maybe shows the
         minimum and maximum value during the measurment, e.g.
        `|CR,1,60,0,  1.0000000000000000E+00,  0.0000000000000000E+00,1,4,mbar;`
- *CN*  (mostly 9 parameters)
         gives the _name_ of the measured entity
        `|CN,1,27,0,0,0,15,pressure_Vacuum,0,;` 
- *Cb*  (mostly 14 paramters) (optional?)
        this one probably gives the minimum/maximum measured values!!
        `|Cb,1,117,1,0,1,1,0,341288,0,341288,1,0.0000000000000000E+00,1.1781711390000000E+09,;`
- *CS*  (mostly 4 parameters)
        this markers announces the actual measurement data in binary format, 
        provide the number of values and the actual data, 
        e.g. `|CS,1, 341299, 1, ...data... ;`

## Open Issues and question?

- which parameter indicate(s) little vs. big endian?



