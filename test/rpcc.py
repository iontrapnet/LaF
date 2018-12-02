import zerorpc

c = zerorpc.Client()
c.connect("tcp://127.0.0.1:4242")
print c.echo([1,2,3])
print c.echo({1:1,2:2,'a':3})