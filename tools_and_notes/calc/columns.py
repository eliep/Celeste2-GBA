x = 0
while x < 128:
	tx = x * 8 + 20 * 0.1
	#rectfill(tx, 0, tx + (x % 2) * 8 + 8, level.height * 8, level.columns)
	print(f"{tx}-{(tx + (x % 2) * 8 + 8)}")
	x += 1 + x % 7