
import raw_eater

rawlist = [ "sample/VehicleSpeed_HS.raw",
            "sample/Rangerover_Evoque_F-RR534_2019-05-07/ABS_A_Port1.raw",
            "./pyt/example.py",
            "sample/Rangerover_Evoque_F-RR534_2019-05-07/LateralAcceleration_HS.raw" ]

print("")

# convert every single listed file
for rf in rawlist :

    print("converting " + str(rf) + "...\n" + 90*("-") + "\n")

    # setup instance of "raw_eater" and trigger conversion
    eatraw = raw_eater.raweater(rf.encode())

    # check validity of file format
    if eatraw.validity() :

        # show channel name and its unit
        entity = eatraw.channel_name().decode()
        unit = eatraw.unit().decode()
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

        eatraw.write_table(outname.encode(),ord(' '))

    else :

        print("\nerror: invalid/corrupt .raw file")

    print("\n")
