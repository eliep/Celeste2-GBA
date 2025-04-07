from math import sin, radians, cos, pi

break_after = 8

degrees = 512
#(32-num).num
num = 16

print(f"const float SIN[{degrees}] = "+"{\n    ",end='')
for i in range(degrees):
	val = sin(radians(i/degrees*360))
	valstr = str(hex(int(val*(2**num))&0xFFFFFFFF))
	print(valstr+", ", end='')
	if(i%break_after==break_after-1 and i != degrees-1):
		print('\n    ',end='')
print("\n};")

print(f"const float COS[{degrees}] = "+"{\n    ",end='')
for i in range(degrees):
	val = cos(radians(i/degrees*360))
	valstr = str(hex(int(val*(2**num))&0xFFFFFFFF))
	print(valstr+", ", end='')
	if(i%break_after==break_after-1 and i != degrees-1):
		print('\n    ',end='')
print("\n};")