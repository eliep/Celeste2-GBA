from math import sin, radians, cos, pi

degrees = 512
break_after = 8

print(f"const float SIN[{degrees}] = "+"{\n    ",end='')
for i in range(degrees):
	val = sin(radians(i/degrees*360))
	valstr = "{:.3f}".format(val)
	print(valstr+"f, ", end='')
	if(i%break_after==break_after-1 and i != degrees-1):
		print('\n    ',end='')
print("\n};")

print(f"const float COS[{degrees}] = "+"{\n    ",end='')
for i in range(degrees):
	val = cos(radians(i/degrees*360))
	valstr = "{:.3f}".format(val)
	print(valstr+"f, ", end='')
	if(i%break_after==break_after-1 and i != degrees-1):
		print('\n    ',end='')
print("\n};")