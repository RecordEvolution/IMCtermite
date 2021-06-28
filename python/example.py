
#-----------------------------------------------------------------------------#

import raw_eater
import raw_meat
import pyarrow as pa
import pyarrow.parquet as pq
from pathlib import Path

fileobj1 = Path("samples/datasetA/").rglob("*.raw")
rawlist1 = [str(fl) for fl in fileobj1]

fileobj2 = Path("samples/datasetB/").rglob("*.raw")
rawlist2 = [str(fl) for fl in fileobj2]

rawlist = rawlist1 #[rawlist1[0],rawlist1[4],rawlist2[0],rawlist2[6]]
for fil in rawlist2 :
    rawlist.append(fil)
rawlist.append("./README.md")

print("")
print(rawlist)
print()

#-----------------------------------------------------------------------------#

# alternatively create "empty" instance of "raw_eater" and set file names
eatraw = raw_eater.raweater()
# eatraw.set_file("../smp/pressure_Vacuum.raw".encode())

# convert every single listed file
for rf in rawlist :

    print("converting " + str(rf) + "...\n" + 90*("-") + "\n")

    # setup instance of "raw_eater" and trigger conversion
    # eatraw = raw_eater.raweater(rf.encode())
    # eatraw = raw_meat.rawmerger(rf.encode())

    # use global instance of "raw_eater" to set file and perform decoding
    eatraw.set_file(rf.encode())
    try :
        eatraw.do_conversion()
    except RuntimeError as e :
        print("conversion failed: " + str(e))

    # check validity of file format
    if eatraw.validity() :

        # show channel name and its unit
        entity = eatraw.channel_name().decode(encoding='UTF-8',errors='ignore')
        unit = eatraw.unit().decode(encoding='UTF-8',errors='ignore')
        print("\nentity: " + str(entity))
        print("unit:   " + str(unit) + "\n")

        # obtain extracted data
        xt = eatraw.get_time()
        yt = eatraw.get_channel()

        # show excerpt of data
        print("time (length: " + str(len(xt)) + ") \n"
             + str(xt[:10]) + "\n...\n" + str(xt[-10:]) + "\n")
        yttrunc = [round(y,4) for y in yt]
        print(str(entity) + " (length: " + str(len(yttrunc)) + ") \n"
            + str(yttrunc[:10]) + "\n...\n" + str(yttrunc[-10:]) + "\n")

        outname = rf.split('/')[-1].replace('raw','csv')

        print("write output to : " + outname)
        eatraw.write_table(("output/"+outname).encode(),ord(' '))

    else :

        print("\nerror: invalid/corrupt .raw file")

    print("\n")

#-----------------------------------------------------------------------------#

print("convert and merge channels " + "\n" + 90*("-") + "\n")

# setup new instance to merge channels
eatmea = raw_meat.rawmerger(''.encode()) #rawlist[0].encode())

# add every single channel/file in list
for rf in rawlist :
    print("\nadding channel " + str(rf))
    try :
        succ = eatmea.add_channel(rf.encode())
        print("\nrecent time series: length: " + str(len(eatmea.get_time_series())) + "\n")
    except RuntimeError as e :
        print("failed to add channel: " + str(e))

# show summary of successfully merged channels
print("\nmerged channels:\n")

# write merged table to .csv output
eatmea.write_table_all('output/allchannels.csv'.encode(),ord(','))

# get number of successfully merged channels and their names (+units)
numch = eatmea.get_num_channels()
chnames = [chnm.decode(encoding='UTF-8',errors='ignore') for chnm in eatmea.get_channel_names()]
print("number of channels: " + str(numch))
print("channel names: " + str(chnames))

# obtain final time series
timse = eatmea.get_time_series()
print("\nfinal time series:\nlength:" + str(len(timse)) + "\n")

# get time unit and prepend column name
chnames.insert(0,"Time ["+str(eatmea.time_unit().decode(encoding='UTF-8',errors='ignore'))+"]")

# prepare list of pyarrow arrays
pyarrs = []
pyarrs.append(pa.array(timse))

for i in range(0,numch) :
    print("\n" + str(i) + "  " + str(chnames[i]))
    dat = eatmea.get_channel_by_index(i)
    print("length: " + str(len(dat)))
    pyarrs.append(pa.array(dat))
print("")
# print("\npyarrow arrays\n" + str(pyarrs))

# create pyarrow table from data
pyarwtab = pa.Table.from_arrays(pyarrs,chnames)
print("\n" + 60*"-" + "\n" + str(pyarwtab) + "\n")

# write pyarrow table to .parquet file with compression
pq.write_table(pyarwtab,'output/allchannels.parquet',compression='BROTLI')  # compression='BROTLI', 'SNAPPY')

# try to read and decode the .parquet file
df = pq.read_table('output/allchannels.parquet')
print(df.to_pandas())
# df.to_pandas().to_csv('allchannels.csv',index=False,encoding='utf-8',sep=",")

#-----------------------------------------------------------------------------#
