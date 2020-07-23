
import raw_eater

eatraw = raw_eater.raweater(b"sample/pressure_Vacuum.raw")
# eatraw = raw_eater.raweater(b"/home/mario/Downloads/FAMOS_Datenformat_ab61.pdf")

print(str(eatraw.validity()))

print(eatraw.channel_name())
print(eatraw.unit())

print(eatraw.get_time())
print(eatraw.get_channel())

eatraw.print(b"mycsv.csv")
