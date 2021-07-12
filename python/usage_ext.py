
import imc_termite
import json
import os
import datetime

# declare and initialize instance of "imctermite" by passing a raw-file
try :
    imcraw = imc_termite.imctermite(b"samples/sampleB.raw")
except RuntimeError as e :
    raise Exception("failed to load/parse raw-file: " + str(e))

# obtain list of channels as list of dictionaries (without data)
channels = imcraw.get_channels(False)
print(json.dumps(channels,indent=4, sort_keys=False))

# obtain all channels (including full data)
channelsdata = imcraw.get_channels(True)

# everything that follows is an example that specifically makes use only of
# the first (index = 0) channel ...
idx = 0

if len(channelsdata) > 0 :

    # get first channel's data
    chnydata = channelsdata[idx]['ydata']
    chnxdata = channelsdata[idx]['xdata']
    print("xdata: " + str(len(chnxdata)))
    print("ydata: " + str(len(chnydata)))

    # extract trigger-time
    trigtim = datetime.datetime.fromisoformat(channels[idx]["trigger-time"])
    print(trigtim)

    # file output of data with absolute timestamp in 1st column
    filname = os.path.join("./",channelsdata[idx]['name']+".csv")
    print("writing output into " + filname)
    with open(filname,'w') as fout :
        # include column header
        fout.write( str(channelsdata[idx]['xname']) + '[' + str(channelsdata[idx]['xunit']) + "]"
                  + ","
                  + str(channelsdata[idx]['yname']) + '[' + str(channelsdata[idx]['yunit']) + "]"
                  + "\n" )
        # add data (introduce time shift according to trigger-time)
        for row in range(0,len(chnxdata)) :
            fout.write( str( (trigtim + datetime.timedelta(seconds=chnxdata[row])).isoformat() )
                      + ","
                      + str( chnydata[row])
                      + "\n" )
