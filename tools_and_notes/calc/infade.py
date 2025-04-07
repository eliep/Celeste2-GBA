from math import sin,pi,floor
# 0<=infade<15
"""local e = infade / 12
for i=0,127 do
    s = (127 + 64) * e - 32 + sin(i * 0.2) * 16 + (127 - i) * 0.25
    rectfill(camera_x+s,camera_y+i,camera_x+128,camera_y+i,0)
end"""
def sin_(x):
    return sin(-(-x-0.5)*2*pi)

e = 0/12
for i in range(0,127+1):
    s = (127 + 64) * e - 32 + sin_(i * 0.2) * 16 + (127 - i) * 0.25
    print(f"{i}: {(s)}")