
import raw_eater

rawlist = [ "sample/Rangerover_Evoque_F-RR534_2019-05-07/ABS_A_Port1.raw",
            "./pyt/example.py",
            "sample/Rangerover_Evoque_F-RR534_2019-05-07/LateralAcceleration_HS.raw" ]

print("")

for rf in rawlist :

    print("converting " + str(rf) + "...\n")

    eatraw = raw_eater.raweater(rf.encode())

    print(str(eatraw.validity()))

    if eatraw.validity() :

        print(eatraw.channel_name())
        print(eatraw.unit())

        print(eatraw.get_time())
        print(eatraw.get_channel())

        outname = rf.split('/')[-1].replace('raw','csv')

        eatraw.write_table(outname.encode(),ord(' '))

    else :

        print("invalid/corrupt .raw file")

    print("\n")
