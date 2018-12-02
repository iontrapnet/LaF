import tempfile
f = tempfile.NamedTemporaryFile(delete=False)
f.close()
print(f.name)