
import IMCtermite
import json
import os

# list files in sample directory
# fileobj1 = Path("samples/").rglob("*.raw")
# rawlist1 = [str(fl) for fl in fileobj1]
rawlist1 = ["samples/datasetB/datasetB_29.raw"]
print(rawlist1)

for fl in rawlist1:

    print("converting " + str(fl) + " : " + str(os.path.basename(fl)) )

    # declare and initialize instance of "imctermite" by passing a raw-file
    try :
        imcraw = IMCtermite.imctermite(fl.encode())
    except RuntimeError as e :
        raise Exception("failed to load/parse raw-file: " + str(e))

    # obtain list of channels as list of dictionaries (without data)
    channels = imcraw.get_channels(False)
    print(json.dumps(channels,indent=4, sort_keys=False))

    # print the channels into a specific directory
    imcraw.print_channels(b"./",ord(','))

    # print all channels in single file
    imcraw.print_table(("./"+str(os.path.basename(fl).split('.')[0])+"_allchannels.csv").encode())
