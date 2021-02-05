
## Fileformat

A data file of _IMC Bus Format_ type with extension _.raw_ is a _mixed text/binary
file_ featuring a set of markers (keys) that indicate the start of various blocks
of data providing meta information and the actual measurement data. Every single
marker is introduced by character `"|" = 0x 7c` followed by two uppercase letters,
which characterize the type of marker. Each block is further divided into several
parameters separated by commata `"," = 0x 2c` and terminated by a semicolon
`";" = 0x 3b`. For instance, the header - first 600 bytes - of a raw file may
look like this (in UTF-8 encoding)

```
|CF,2,1,1;|CK,1,3,1,1;
|NO,1,86,0,78,imc STUDIO 5.0 R10 (04.08.2017)@imc DEVICES 2.9R7 (25.7.2017)@imcDev__15190567,0,;
|CG,1,5,1,1,1; |CD,2,  63,  5.0000000000000001E-03,1,1,s,0,0,0,  0.0000000000000000E+00,1;
|NT,1,16,1,1,1980,0,0,0.0;       |CC,1,3,1,1;|CP,1,16,1,4,7,32,0,0,1,0;
|CR,1,60,0,  1.0000000000000000E+00,  0.0000000000000000E+00,1,4,mbar;|CN,1,27,0,0,0,15,pressure_Vacuum,0,;
|Cb,1, 117,1,0,    1,         1,         0,      9608,         0,      9608,1,  2.0440300000000000E+03,  1.2416717060000000E+09,;
|CS,1,      9619,         1,�oD	�nD6�nD)�nD�
```

where line breaks where introduced for readability. Most of the markers introduce
blocks of text, while only the last one identified by `|CS` contains binary data.
The format supports the storage of _multiple data sets (channels)_ in a single
file. The channels may be ordered in _multiplex_ mode (ordering w.r.t. time) or
_block_ mode (ordering w.r.t. to channels).

### Markers

The markers (keys) are introduced by `"|" = 0x 7c` followed by two uppercase
letters. There are _two types_ of markers distinguished by the first letter:

1. _critical_ markers: introduced by `|C` featuring uppercase `C`
1. _noncritical_ markers: introduced by `|N` featuring uppercase `N`  

The second letter represents further details of the specific key. Note, that
while the _noncritical_ keys are optional, any _.raw_ file _cannot be_ correctly
decoded if any of the _critical_ markers are misinterpreted, invalid or damaged.
The second uppercase letter is followed by the first comma and the _version_
of the key starting from 1. After the next comma, an _(long) integer_ (in text
representation) specifies the length of the entire block, i.e. the number of
bytes between the following comma and the block-terminating semicolon. The further
structure of a block is not defined and may feature different numbers of additional
parameters. The format allows for any number of carriage returns (`CR = 0x0d`)
and line feeds (`LF = 0x 0a`) between keys, i.e. the block terminating semicolon
and the vertical bar (pipe) of the next key. The following _critical markers_
are defined

| marker | structure (example)                                                                           | description                                             |
|--------|-----------------------------------------------------------------------------------------------|---------------------------------------------------------|
| CF     | CF,2,1,1;                                                                                     | format version and processor                            |
| CK     | CK,1,3,1,<closed>;                                                                            | start of group of keys                                  |
|        |                                                                                               | length is always 3                                      |
|        |                                                                                               | <closed> must be 0 or 1 depending correct closure of the measurment series |
| CB     | CB,1,<length>,<groupindex>,<namelength>,<name>,<commentlength>,<comment>;                     | defines a group of cannels                              |
| CT     | CT,1,<length>,<groupindex>,<namelength>,<name>,<textlength>,<text>,<commentlength>,<comment>; | text definition                                         |
| CG     | CG,1,<length>,<numberofcompontent>,<fieldtype>,<dimension>;                                   | introduces group of components corresponding to CC keys |
|        |                                                                                               | field type is one of:  1 = real number                  |
|        |                                                                                               |                        2 = XY                           |
|        |                                                                                               |                        3 = 3XY                          |
|        |                                                                                               |                        4 = complex with Re + Im         |
|        |                                                                                               |                        5 = complex with Abs + Phase     |
|        |                                                                                               |                        6 = complex: Abs in dB + Phase   |
|        |                                                                                               | dimension: 1 for fieldtype 1, 2 for fieldtype 2-6       |
| CD     | CD,1,<length>,<dx>,<calibration>,<unitlength>,<unit>,0,0,0;                                   | dx = distance between two samples                       |
|        |                                                                                               | calibration = 1 : calibrated time-basis                 |
|        |                                                                                               |             = 0 : noncalibrated time-basis              |
|        |                                                                                               | unit: unit of this axis                                 |
| CZ     | CZ,1,<length>,<dz>,<dzcalibration>,<z0>,<z0calibration>,<unitlength>,<unit>,<segmentlength>;  | scaling of z-axis for segments                          |
| CC     | CC,1,<length>,<componentindex>,<analogdigital>;                                               | start of single component:                              |
|        |                                                                                               |  - componentindex = 1 real, real part, abs              |
|        |                                                                                               |                   = 2 complex, imaginar part, phase     |
|        |                                                                                               |  - analogdigital = 1 analog = 2 digital                 |
| CP     | CP,1,<lenght>,<buffref>,<bytes>,<datatype>,<signbits>,<mask>,<offset>,<numsamples>,<bytedist>;| buffref = int > 0 corresp.to buffer descript. in Cb key |
|        |                                                                                               | bytes: number of bytes for singel value                 |
|        |                                                                                               | datatype: 1 = unsigned byte, 2 = signed byte,           |
|        |                                                                                               |           3 = unsigned short, 4 = signed short          |
|        |                                                                                               |           5 = unsigned long, 6 = signed long            |
|        |                                                                                               |           7 = float, 8 = double                         |
|        |                                                                                               |           9 = imc Devices transitional recording        |
|        |                                                                                               |           10 = timestamp ascii, 11 = 2byte word digital |
|        |                                                                                               |           13 = 6byte unsigned long                      |
|        |                                                                                               | signbits: number of significant bits                    |
|        |                                                                                               | mask: default = 0                                       |
|        |                                                                                               | offset: offset of first sample in data block            |
|        |                                                                                               | numsamples: number of subsequent samples in channel     |
|        |                                                                                               | bytedist: number of bytes between last sample of channel|
|        |                                                                                               | to start of next channel                                |
| Cb     | Cb,1,<lenght>,<numbuff>,<bytesinuserinfo>,<buffref>,<indexsamplekey>,<offsetbuffer>,          |                                                         |
|        |      <bufferlength>,<offset>,<fillbytes>,0,X0,<addtime>,<userinfo>,<newevent>,[buffref,...];  | buffer description                                      |
| CR     |                      |                                                                                                                        |
| CN     |                      |                                                                                                                        |
| CS     |                      |
| CI     |                      |                                                                                                                        |
| Ca     |                      |                                                                                                                        |
