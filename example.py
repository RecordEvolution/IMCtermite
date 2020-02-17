
import raw_eater

eatraw = raw_eater.raweater(b"/home/mario/raw_eater/sample/VehicleSpeed_HS.raw")

print(eatraw.channel_name())
print(eatraw.unit())

print(eatraw.get_time())
print(eatraw.get_channel())

eatraw.print(b"mycsv.csv")
