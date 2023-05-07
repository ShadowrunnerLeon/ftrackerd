log = open('/tmp/ftracker.log', 'w+')
log.close()

config = open('/tmp/ftracker.config', 'w+')
config.write('5\n')

for count in range(5):
    path = '/tmp/ftracker' + str(count)
    config.write(path + '\n')
    track_file = open(path, 'w+')
    track_file.close()

config.close()