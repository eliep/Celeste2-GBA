import sys
from pathlib import Path

if len(sys.argv)!=2:
    print(f"Usage: {Path(__file__).name} <ROM File>.gba")
    exit()

with open(sys.argv[1], "rb") as f:
    f.seek(0xA0)
    title = f.read(12).decode("ascii", errors="replace")
    gamecode = f.read(4).decode("ascii")
    makercode = f.read(2).decode("ascii")
    f.seek(0xBC)
    version = int.from_bytes(f.read(1), "little")

print(f"Title     : {title.strip()}")
print(f"Game Code : {gamecode}")
print(f"Maker Code: {makercode}")
print(f"Version   : {version}")