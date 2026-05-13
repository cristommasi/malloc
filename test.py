

PAGE_SIZE = 4096
SMALL = (32 * PAGE_SIZE)
TINY = (4 * PAGE_SIZE)
tcount = 0
scount = 0
for i in range(16, 1024, 16):

	if i <= 112:
		print(f'TINY:  {i}')
		tcount += 1
	else:
		print(f'SMALL: {i}')
		scount += 1

print(f'tiny  count = [{tcount}]')
print(f'small count = [{scount}]')

print(f'tiny  / 144 + 16 = {TINY / (144)}')
print(f'small / 1168 + 16 = {SMALL / (1168)}')
#T112 chunks = 146.0
#S112 chunks = 1170.0