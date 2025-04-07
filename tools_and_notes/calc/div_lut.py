#from math import sin, radians, cos, pi

break_after = 8
#132-[-448,-1], 
lut_range = [0,99] #inclusive
m = 10

print(f"// LUT for {lut_range} mod {m}")
print(f"const int MOD_{m}[{lut_range[1]-lut_range[0]+1}] = "+"{\n    ",end='')
for i in range(lut_range[0],lut_range[1]+1):
	val = i%m
	if val < 0:
		val += m
	valstr = str(val)
	print(valstr+", ", end='')
	if(i%break_after==break_after-1):
		print('\n    ',end='')
print("\n};")
