
import raw_eater

eatraw = raw_eater.raweater(b"sample/Rangerover_Evoque_F-RR534_2019-05-07/ABS_A_Port1.raw")
# eatraw = raw_eater.raweater(b"/home/mario/Downloads/FAMOS_Datenformat_ab61.pdf")

print(str(eatraw.validity()))

print(eatraw.channel_name())
print(eatraw.unit())

print(eatraw.get_time())
print(eatraw.get_channel())

eatraw.write_table(b"mycsv.csv",ord(' '))
