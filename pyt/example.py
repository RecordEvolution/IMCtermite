
#-----------------------------------------------------------------------------#

import raw_eater
import raw_meat
import pyarrow as pa
import pyarrow.parquet as pq

rawlist = [
            "smp/Rangerover_Evoque_F-RR534_2019-05-07/BrakePedalActiveQF_HS.raw",
             "smp/Rangerover_Evoque_F-RR534_2019-05-07/BrakePressure_HS.raw",
             "smp/Rangerover_Evoque_F-RR534_2019-05-07/EngineSpeed_HS.raw",
             "smp/Rangerover_Evoque_F-RR534_2019-05-07/pressure_FL.raw",
            "smp/Rangerover_Evoque_F-RR534_2019-05-07/pressure_RL.raw",
            "smp/Rangerover_Evoque_F-RR534_2019-05-07/pressure_Vacuum.raw",
            "smp/VehicleSpeed_HS.raw",
            "smp/Rangerover_Evoque_F-RR534_2019-05-07/ABS_A_Port1.raw",
            "./pyt/example.py",
            "smp/Rangerover_Evoque_F-RR534_2019-05-07/LateralAcceleration_HS.raw",
            "smp/Rangerover_Evoque_F-RR534_2019-05-07/Temp_Disc_FR.raw"
          ]

print("")

#-----------------------------------------------------------------------------#

# convert every single listed file
for rf in rawlist :

    print("converting " + str(rf) + "...\n" + 90*("-") + "\n")

    # setup instance of "raw_eater" and trigger conversion
    eatraw = raw_eater.raweater(rf.encode())
    # eatraw = raw_meat.rawmerger(rf.encode())

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
        eatraw.write_table((outname).encode(),ord(' '))

    else :

        print("\nerror: invalid/corrupt .raw file")

    print("\n")

#-----------------------------------------------------------------------------#

print("convert and merge channels " + "\n" + 90*("-") + "\n")

# setup new instance to merge channels
eatmea = raw_meat.rawmerger(rawlist[0].encode())

# add every single channel/file in list
for rf in rawlist :
    print("\nadding channel " + str(rf))
    succ = eatmea.add_channel(rf.encode())
    if succ :
        print("\nrecent time series: length: " + str(len(eatmea.get_time_series())) + "\n")
    else :
        print("\nfailed to add channel\n")

# show summary of successfully merged channels
print("\nmerged channels:\n")

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
pq.write_table(pyarwtab,'allchannels.parquet',compression='BROTLI')  # compression='BROTLI', 'SNAPPY')

# try to read and decode the .parquet file
df = pq.read_table('allchannels.parquet')
print(df.to_pandas())
df.to_pandas().to_csv('allchannels.csv',index=False,encoding='utf-8',sep=",")

#-----------------------------------------------------------------------------#
