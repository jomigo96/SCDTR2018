import sys

file_name = sys.argv[1]

t=[]
l=[]

with open(file_name, "r") as fp:
    i = 0
    for line in fp:
        t.append(i)
        l.append(line)
        i = i + 0.005

with open(file_name, "w") as fp:
    for i in range(len(t)):
        fp.write("{:.3f},{}\n".format(t[i], l[i]) )
