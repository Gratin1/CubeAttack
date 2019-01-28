import fileinput
import subprocess
import sys


def convert(value):
	if value[-1] == 'm':
		return 1024*eval(value[:-1])
	if value[-1] == 'g':
		return 1024*1024*eval(value[:-1])
	else:
		return eval(value)

if len(sys.argv)!= 2:
	raise Exception('need executable')

with open("memory_use", "w") as mem_file:
	mem_file.write("Process: {}, PID: \n".format(sys.argv[1]))
	mem_file.write("{:*^39}".format('*'))
	mem_file.write("\nVIRT(kb) | RES(kb)\n")
	mem_file.write("{:-^15}\n".format('-'))
	mem_file.write("{:.3f}  {:.3f}\n".format(0,0))

while True:

	pric = subprocess.Popen(['top','-n', '1',], stdout=subprocess.PIPE)
	proc2 = subprocess.Popen(['grep', sys.argv[1]], stdin=pric.stdout,stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	pric.stdout.close()
	data = str(proc2.communicate()[0])
	data = data.split(" ")
	result = [x for x in data if x]
	#print(result)
	with open("memory_use") as outfile:
		for i in range(4):
			_=outfile.readline()
		file_value = outfile.readline().split("  ")
		file_RES_value = file_value[1]
		file_VIRT_value = file_value[0]		
	
	try:
		#result[5]-VIRT, result[6]-RES
		current_RES = convert(result[6])
		current_VIRT = convert(result[5])
		if eval(file_RES_value) < current_RES:
			with fileinput.FileInput("memory_use", inplace=True) as outfile:
				for line in outfile:	
					print(line.replace(file_RES_value, str(current_RES)), end='')		
		if eval(file_VIRT_value) < current_VIRT:
			with fileinput.FileInput("memory_use", inplace=True) as outfile:
				for line in outfile:		
					print(line.replace(file_VIRT_value, str(current_VIRT)), end='')
	except Exception:
			exit('process stoped')
