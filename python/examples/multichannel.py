
import IMCtermite
import pandas

if __name__ == "__main__" :

    imctm = IMCtermite.imctermite(b"Measurement.raw")

    chns = imctm.get_channels(True)

    df = pandas.DataFrame()
    
    xcol = "time ["+chns[0]['xunit']+"]"    
    df[xcol] = pandas.Series(chns[0]['xdata'])

    for idx,chn in enumerate(chns) :
        #xcol = (chn['xname'] if chn['xname'] != '' else "x_"+str(idx))+" ["+chn['xunit']+"]"
        #df[xcol] = pandas.Series(chn['xdata'])
        ycol = chn['yname']+" ["+chn['yunit']+"]"
        df[ycol] = pandas.Series(chn['ydata'])

    print(df)
    df.to_csv("Measurement.csv",header=True,sep='\t',index=False)

