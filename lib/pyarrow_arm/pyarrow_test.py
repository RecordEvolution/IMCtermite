
import pyarrow.parquet as pq
import pyarrow.csv as pv

csvfile = 'pressureVacuum.csv'

tb = pv.read_csv(csvfile,parse_options=pv.ParseOptions(delimiter=','))

print(tb)

parquetfile = 'pressureVacuum.parquet'

pq.write_table(tb,parquetfile,compression='SNAPPY')
#  {‘NONE’, ‘SNAPPY’, ‘GZIP’, ‘LZO’, ‘BROTLI’, ‘LZ4’, ‘ZSTD’}

df = pq.read_table(parquetfile,columns=None)

print(df)
