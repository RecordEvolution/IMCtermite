
import imctermite import imctermite

def show_results(imcraw) :

    channels = imcraw.get_channels(False)
    print(channels)

    channelsData = imcraw.get_channels(True)
    print("number of channels: " + str(len(channelsData)))

    for (i,chn) in enumerate(channelsData) :
        print(str(i) +  " | " + chn['name'])
        print(chn['xname'] + " | " + chn['xunit'])
        print(chn['xdata'][:10])
        print(chn['yname'] + " | " + chn['yunit'])
        print(chn['ydata'][:10])

    print("")

# create instance of 'imctermite'
imcraw = imctermite(b'samples/sampleA.raw')

show_results(imcraw)

# use previous instance of 'imctermite' to provide new file
imcraw.submit_file(b'samples/sampleB.raw')

show_results(imcraw)
