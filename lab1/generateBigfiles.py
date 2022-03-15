import random
import string

def generateString(size, chars=string.ascii_lowercase):
    return ''.join(random.choice(chars) for _ in range(size))


file = open("bigFile.txt", "w")

for i in range(1_00):
    file.write(generateString(1_00) + '\n')
    if i % 1000 == 0: print(i)

file.close()