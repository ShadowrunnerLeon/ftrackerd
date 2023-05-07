import os

os.remove('/tmp/ftracker.log')
os.remove('/tmp/ftracker.config')

for count in range(5):
    path = '/tmp/ftracker' + str(count)
    os.remove(path)