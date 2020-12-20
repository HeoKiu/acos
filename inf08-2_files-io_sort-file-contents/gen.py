size = 100
from random import randint
with open('file', 'wb') as f:
	for i in range(size):
		value = randint(0, 100)
		f.write(value.to_bytes(4, 'little'))
