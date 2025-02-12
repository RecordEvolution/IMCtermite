
import imctermite
import pandas
import datetime

def add_trigger_time(trigger_time, add_time) :
    trgts = datetime.datetime.strptime(trigger_time,'%Y-%m-%dT%H:%M:%S')
    dt = datetime.timedelta(seconds=add_time)
    return (trgts + dt).strftime('%Y-%m-%dT%H:%M:%S:%f')

if __name__ == "__main__" :

    # read file and extract data
    imctm = imctermite.imctermite(b"Measurement.raw")
    chns = imctm.get_channels(True)
    
    # prepare abscissa
    xcol = "time ["+chns[0]['xunit']+"]"
    #xcol = "timestamp"
    xsts = [add_trigger_time(chns[0]['trigger-time'],tm) for tm in chns[0]['xdata']]

    # sort channels
    chnnms = sorted([chn['name'] for chn in chns], reverse=False)
    chnsdict = {}
    for chn in chns :
        chnsdict[chn['name']] = chn

    # construct dataframe
    df = pandas.DataFrame()
    df[xcol] = pandas.Series(chns[0]['xdata'])
    #df[xcol] = pandas.Series(xsts)
    #for idx,chn in enumerate(chns) :
    for chnnm in chnnms :
        chn = chnsdict[chnnm]
        #xcol = (chn['xname'] if chn['xname'] != '' else "x_"+str(idx))+" ["+chn['xunit']+"]"
        #df[xcol] = pandas.Series(chn['xdata'])
        ycol = chn['yname']+" ["+chn['yunit']+"]"
        df[ycol] = pandas.Series(chn['ydata'])

    # show entire dataframe and write file
    print(df)
    df.to_csv("Measurement.csv",header=True,sep='\t',index=False)

