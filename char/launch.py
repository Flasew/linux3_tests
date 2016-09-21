#! /usr/bin/python
from os import listdir
import subprocess
import sys

max_major = 1<<11

def get_majors():
	devices = listdir('/sys/dev/char')
	majors = [ int(d.split(':')[0]) for d in devices ]
	return majors

def unused_majors():
	unused = []
	majors = get_majors()
	for i in range(max_major):
		if not (i in majors):
			unused.append(i)
	return unused
			
def launch(m,irq):
		
	cmd = 'insmod char.ko major=%i irq=%i' % (m,irq)
	print cmd
	subprocess.call(cmd,shell=True)

if __name__ == '__main__':
	
	irq = int(sys.argv[1])
	m = unused_majors()[0]
	launch(m,irq)
	
