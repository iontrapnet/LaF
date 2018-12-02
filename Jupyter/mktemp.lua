local f = os.getenv('TEMP')..os.tmpname()
io.open(f,'w'):close()
print(f)