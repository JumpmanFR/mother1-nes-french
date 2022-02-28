import os
print("---------------------------------------------------------")
print("Creating UPS file (ebb.ups)")
print("---------------------------------------------------------")
os.system("ups diff -b ebb.nes -m test.nes -o ebb.ups")
os.system("pause")
