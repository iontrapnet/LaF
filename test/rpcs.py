import zerorpc

class EchoRPC(object):
    def echo(self, args):
        print(args)
        return args

s = zerorpc.Server(EchoRPC())
s.bind("tcp://0.0.0.0:4242")
s.run()