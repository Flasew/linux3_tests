#! /usr/bin/python
from os import listdir
import subprocess
from optparse import OptionParser

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
			
def launch(m,irq,irqflags,target_dev):
	
	m = int(m)
	irq = int(irq)
	irqflags=int(irqflags)

	cmd = 'insmod char.ko major=%i irq=%i irqflags=%i target_dev=%s' % (m,irq,irqflags,target_dev);
	print cmd
	subprocess.call(cmd,shell=True)

if __name__ == '__main__':

	parser = OptionParser()
	parser.add_option('--irq',dest='irq', default='0')
	parser.add_option('--irqflags',dest='irqflags', default=0x0080)
	parser.add_option('--dev',dest='target_dev', default='/dev/null')
	
	o,r = parser.parse_args()
	
	m = unused_majors()[0]
	launch(m, o.irq, o.irqflags, o.target_dev)
	
