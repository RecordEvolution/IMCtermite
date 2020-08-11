
import pyarrow as pa
import numpy as np
import pyarrow.parquet as pq

db = pa.array(np.linspace(10,50,6))
print(db)
da = pa.array(np.linspace(0,5,6))
print(db)

filenam = 'pyarrow_testtab.parquet'

patab = pa.Table.from_arrays([da,db],['entity A [unitA]','entity B [unitB]'])
print(patab)

# pq.write_table(patab,filenam,compression='BROTLI')
pq.write_table(patab,filenam,compression='SNAPPY')

df = pq.read_table(filenam)
print(df)
print(df.to_pandas())

#import readline
#readline.write_history_file('generate_pyarrow_table_and_write_parquet.py')
