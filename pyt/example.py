
#-----------------------------------------------------------------------------#

import raw_eater
import raw_meat
import pyarrow as pa
import pyarrow.parquet as pq

rawlist1 = [
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

rawlist2 = [
"smp/Mercedes_E-Klasse-2019-08-08/ACC_lat.raw",
"smp/Mercedes_E-Klasse-2019-08-08/ACC_long.raw",
"smp/Mercedes_E-Klasse-2019-08-08/Flex_AccelPdlPosn.raw",
"smp/Mercedes_E-Klasse-2019-08-08/Flex_AirTemp_Outsd_IC.raw",
"smp/Mercedes_E-Klasse-2019-08-08/Flex_AirTemp_Outsd.raw",
"smp/Mercedes_E-Klasse-2019-08-08/Flex_BrkPdl_Stat.raw",
"smp/Mercedes_E-Klasse-2019-08-08/Flex_BrkTrq_D_V2.raw",
"smp/Mercedes_E-Klasse-2019-08-08/Flex_BrkTrq_R.raw",
"smp/Mercedes_E-Klasse-2019-08-08/Flex_BrkTrq_V2.raw",
"smp/Mercedes_E-Klasse-2019-08-08/Flex_EngLoad_OBD.raw",
"smp/Mercedes_E-Klasse-2019-08-08/Flex_EngRPM.raw",
"smp/Mercedes_E-Klasse-2019-08-08/Flex_Odo.raw",
"smp/Mercedes_E-Klasse-2019-08-08/Flex_PkBrk_Stat.raw",
"smp/Mercedes_E-Klasse-2019-08-08/Flex_StWhl_Angl.raw",
"smp/Mercedes_E-Klasse-2019-08-08/Flex_StWhl_AnglSpd.raw",
"smp/Mercedes_E-Klasse-2019-08-08/Flex_TC_liquidFuelCons1.raw",
"smp/Mercedes_E-Klasse-2019-08-08/Flex_VehSpd_Disp.raw",
"smp/Mercedes_E-Klasse-2019-08-08/Flex_WhlRPM_FL.raw",
"smp/Mercedes_E-Klasse-2019-08-08/Flex_WhlRPM_FR.raw",
"smp/Mercedes_E-Klasse-2019-08-08/Flex_WhlRPM_RL.raw",
"smp/Mercedes_E-Klasse-2019-08-08/Flex_WhlRPM_RR.raw",
"smp/Mercedes_E-Klasse-2019-08-08/GPS.height.raw",
"smp/Mercedes_E-Klasse-2019-08-08/GPS.speed.raw",
"smp/Mercedes_E-Klasse-2019-08-08/GPS.time.sec.raw",
"smp/Mercedes_E-Klasse-2019-08-08/Pressure_FL.raw",
"smp/Mercedes_E-Klasse-2019-08-08/Pressure_PC.raw",
"smp/Mercedes_E-Klasse-2019-08-08/Pressure_RR.raw",
"smp/Mercedes_E-Klasse-2019-08-08/Pressure_SC.raw",
"smp/Mercedes_E-Klasse-2019-08-08/setup_id.raw",
"smp/Mercedes_E-Klasse-2019-08-08/Temp_Disc_FL.raw",
"smp/Mercedes_E-Klasse-2019-08-08/Temp_Disc_FR.raw",
"smp/Mercedes_E-Klasse-2019-08-08/Temp_Disc_RL.raw",
"smp/Mercedes_E-Klasse-2019-08-08/Temp_Disc_RR.raw",
"smp/Mercedes_E-Klasse-2019-08-08/Temp_Fluid_FL.raw",
"smp/Mercedes_E-Klasse-2019-08-08/Temp_Fluid_RL.raw",
"smp/Mercedes_E-Klasse-2019-08-08/Travel_Piston.raw",
"smp/Mercedes_E-Klasse-2019-08-08/Vacuum_Booster.raw",
"smp/Mercedes_E-Klasse-2019-08-08/vehicle_id.raw"
]

rawlist = rawlist2

print("")

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

    # use global instance of "raw_eater"
    eatraw.set_file(rf.encode())

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
eatmea = raw_meat.rawmerger(''.encode()) #rawlist[0].encode())

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

# write merged table to .csv output
eatmea.write_table_all('allchannels.csv'.encode(),ord(','))

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
# df.to_pandas().to_csv('allchannels.csv',index=False,encoding='utf-8',sep=",")

#-----------------------------------------------------------------------------#
