
import imctermite
import json
import os

# declare and initialize instance of "imctermite" by passing a raw-file
try :
    imcraw = imctermite.imctermite(b"samples/exampleB.raw")
except RuntimeError as e :
    raise Exception("failed to load/parse raw-file: " + str(e))

# obtain list of channels as list of dictionaries (without data)
channels = imcraw.get_channels(False)
print(json.dumps(channels,indent=4, sort_keys=False))

# obtain data of first channel (with data)
channelsdata = imcraw.get_channels(True)
if len(channelsdata) > 0 :
    chnydata = channelsdata[0]['ydata']
    chnxdata = channelsdata[0]['xdata']

print(len(chnydata))
print(len(chnxdata))
print()

# print the channels into a specific directory
imcraw.print_channels(b"/tmp/",ord(','))

# print all channels separately
for i,chn in enumerate(channels) :
    print(str(i)+" : "+chn['name']+" : "+chn['uuid'])
    filname = os.path.join("/tmp/",str(i) + "_" + chn['name']+".csv")
    print(filname)
    imcraw.print_channel(chn['uuid'].encode(),filname.encode(),ord(','))

# print all channels in single file
imcraw.print_table(b"/tmp/allchannels.csv")

