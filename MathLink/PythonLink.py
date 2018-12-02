from __future__ import print_function
import sys, traceback, re, tokenize, struct, os.path
from cffi import FFI
ffi = FFI()

__dir__ = os.path.dirname(os.path.abspath(__file__)) + os.path.sep

with open(__dir__ + 'ffi_mathlink.h','r') as header:
    ffi.cdef(header.read())

X64 = struct.calcsize('P') == 8
PY3 = sys.version_info[0] == 3

if X64:
    ml = 'ml64i4'
else:
    ml = 'ml32i4'

ml = ffi.dlopen(__dir__ + ml + '.dll')

if PY3:
    integer = (int,)
    string = (str,)
    long = int
    import builtins as __builtin__
    #from past.builtins import execfile
    def execfile(filename, myglobals=None, mylocals=None):
        """
        Read and execute a Python script from a file in the given namespaces.
        The globals and locals are dictionaries, defaulting to the current
        globals and locals. If only globals is given, locals defaults to it.
        """
        if myglobals is None:
            # There seems to be no alternative to frame hacking here.
            caller_frame = inspect.stack()[1]
            myglobals = caller_frame[0].f_globals
            mylocals = caller_frame[0].f_locals
        elif mylocals is None:
            # Only if myglobals is given do we set mylocals to it.
            mylocals = myglobals
        if not isinstance(myglobals, Mapping):
            raise TypeError('globals must be a mapping')
        if not isinstance(mylocals, Mapping):
            raise TypeError('locals must be a mapping')
        with open(filename, "rbU") as fin:
             source = fin.read()
        code = compile(source, filename, "exec")
        exec_(code, myglobals, mylocals)
else:
    integer = (int, long)
    string = (str, unicode)
    import __builtin__

class Expr(tuple):
    def __new__ (cls, *args):
        return super(Expr, cls).__new__(cls, tuple(args))
        
    def __str__(self):
        head = self[0]
        listq = head == 'List'
        if listq:
            r = '{'
        else:
            r = str(head) + '['
        if len(self) > 1:
            r += str(self[1])
            for i in self[2:]:
                r += ', ' + str(i)
        return r + ('}' if listq else ']')

slots = {}
def slot(key, *args):
    if type(key) not in integer:
        return key
    if len(args) == 0:
        return slots.get(key, None)
    else:
        slots[key] = args[0]
        return args[0]

PyObject = slot
PyFunction = slot

def pyeval(expr):
    t = type(expr)
    if t == Expr:
        size = len(expr)
        t = [pyeval(i) for i in expr]
        if callable(t[0]):
            try:
                r = t[0](*t[1:])
            except Exception as e:
                r = Expr(u'Error', *traceback.format_exc().splitlines()[3:])
            finally:
                return r
        else:
            return Expr(*t)
    elif t in string:
        if expr[0] == '"':
            return expr[1:-1]
        else:
            return globals().get(expr, getattr(__builtin__, expr, expr))
    else:
        return expr

def pyget(*args):
    r = args[0]
    for v in args[1:]:
        if type(v) in string:
            if v[0] == '"':
                v = v[1:-1]
            r = getattr(r, v, None)
        else:
            r = None
    return r
Part = pyget
MessageName = pyget

def pyset(*args):
    r = args[0]
    size = len(args)
    for i in range(1,len-1):
        v = args[i]
        if type(v) in string:
            if v[0] == '"':
                v = v[1:-1]
            if i == len - 2:
                try:
                    setattr(r, v, args[len-1])
                except Exception as e:
                    traceback.print_exc()
            r = getattr(r, v, None)
    return r
    
class PythonLink(object):
    @classmethod
    def init(cls):
        cls.env = ml.MLInitialize(ffi.NULL)
    
    @classmethod
    def deinit(cls):
        ml.MLDeinitialize(cls.env)
            
    def __init__(self):
        pass
        
    def install(self):
        err = ffi.new('int*')
        mldir = br'C:\Program Files\Wolfram Research\Mathematica\11.0'
        #mlarg = b"-linklaunch -linkname '" + mldir + br"\math.exe'"
        mlarg = (' '.join(sys.argv[1:])).encode('utf-8')
        self.mlp = ml.MLOpenString(self.env, mlarg, err)
        ml.MLConnect(self.mlp)
        for i in range(len(self.defs)):
            v = self.defs[i]
            self.put_expr(Expr(u'DefineExternal',u'"' + v[1] + u'"', u'"' + v[2] + u'"', i))
        self.put_expr(Expr(u'ToExpression', u'''"
        ClearAll[PyObject, PyFunction];
        Format[PyObject[id_Integer]] := Py[repr[slot[id]]];
        Format[PyFunction[id_Integer]] := Py[repr[slot[id]]];
        PyObject[id_Integer][(f:_[___])[args___]] := PyObject[id][f][args];
        PyObject[id_Integer][(f:Except[List])[args___]] := PyFunction[id, f][args];
        PyObject[id_Integer][k:Except[_List]] := Py[pyget[slot[id], k]];
        PyObject[id_Integer][{k__}] := Py[pyget[slot[id], k]];
        PyObject[id_Integer][k:Except[_List], v_] := Py[pyset[slot[id], k, v]];
        PyObject[id_Integer][{k__}, v_] := Py[pyset[slot[id], k, v]];
        PyFunction[id_Integer, k___][f_[args___]] := PyFunction[id, k, f][args];
        PyFunction[id_Integer][args___] := Py[slot[id][args]];
        PyFunction[id_Integer, k___, $][i_] := Py[pyget[slot[id], k, "__getitem__"][i]];
        PyFunction[id_Integer, k___, $][i_, v_] := Py[pyget[slot[id], k, "__setitem__"][i,v]];
        PyFunction[id_Integer, k__][args___] := Py[pyget[slot[id], k][args]];
        PyObject[PyFunction[id_Integer]] := PyObject[id];
        PyFunction[PyObject[id_Integer]] := PyFunction[id];
        PyObject[obj:Except[_Integer|_Blank|_Pattern]] := PyObject@@Py[obj];
        PyFunction[obj:Except[_Integer|_Blank|_Pattern]] := PyFunction@@Py[obj];
        $[PyObject[id_Integer]] := Py@mexpr@slot@id;
        "'''))
        ml.MLPutSymbol(self.mlp, b'End')
        ml.MLFlush(self.mlp)
    
    def uninstall(self):
        ml.MLClose(self.mlp)
    
    def get_error(self):
        r = ml.MLErrorMessage(self.mlp)
        r = b'"' + r + b'"'
        r = r.decode('utf-8')
        ml.MLClearError(self.mlp)
        ml.MLNewPacket(self.mlp)
        return Expr('Error', r)
    
    def get_packet(self):
        waitResult = ml.MLWaitForLinkActivity(self.mlp)
        print('waitResult = ',waitResult)
        if not waitResult:
            return 0
        pkt = ml.MLNextPacket(self.mlp)
        print('pkt = ',pkt)
        return pkt
    
    def get_expr(self):
        t = ml.MLGetNext(self.mlp)
        if t == ml.MLTKINT:
            r = ffi.new('long long*')
            return r[0] if ml.MLGetInteger64(self.mlp, r) else self.get_error()
        elif t == ml.MLTKREAL:
            r = ffi.new('double*')
            ml.MLGetReal64(self.mlp, r)
            return r[0]
        elif t == ml.MLTKSYM:
            bufp = ffi.new('char**')
            ml.MLGetSymbol(self.mlp, bufp)
            r = ffi.string(bufp[0])
            ml.MLReleaseString(self.mlp, bufp[0])
            return r.decode('utf-8')
        elif t == ml.MLTKSTR:
            bufp = ffi.new('unsigned char**')
            size = ffi.new('int*')
            ml.MLGetByteString(self.mlp, bufp, size, 0)
            r = str(ffi.buffer(bufp[0],size[0]))
            r = b'"' + r + b'"'
            ml.MLReleaseByteString(self.mlp, bufp[0], size[0])
            return r.decode('utf-8')
        elif t == ml.MLTKFUNC:
            n = ffi.new('int*')
            ml.MLGetArgCount(self.mlp, n)
            n = n[0]
            return Expr(*(self.get_expr() for i in range(n+1)))
    
    def put_expr(self, expr):
        t = type(expr)
        if t == Expr:
            head = expr[0]
            if type(head) in string:
                ml.MLPutFunction(self.mlp, head.encode('utf-8'), len(expr) - 1)
            else:
                ml.MLPutNext(self.mlp, ml.MLTKFUNC)
                ml.MLPutArgCount(self.mlp, len(expr) - 1)
                self.put_expr(head)
            for i in expr[1:]:
                self.put_expr(i)
        elif t in string:
            if expr[0] == '"':
                expr = expr[1:-1].encode('utf-8')
                ml.MLPutByteString(self.mlp, expr, len(expr))
            #elif expr.isidentifier():
            elif re.match(tokenize.Name + '$', expr):
                ml.MLPutSymbol(self.mlp, expr.encode('utf-8'))
            else:
                expr = expr.encode('utf-8')
                ml.MLPutByteString(self.mlp, expr, len(expr))
        elif t in integer:
            ml.MLPutInteger64(self.mlp, expr)
        elif t == float:
            ml.MLPutReal64(self.mlp, expr)
        elif t == type(None):
            ml.MLPutSymbol(self.mlp, b'Null')
        else:
            key = long(id(expr))
            slot(key, expr)
            if callable(expr):
                ml.MLPutFunction(self.mlp, b'PyFunction', 1)
            else:
                ml.MLPutFunction(self.mlp, b'PyObject', 1)
            ml.MLPutInteger64(self.mlp, key)
    
    def answer(self):
        pkt = self.get_packet()
        if pkt == ml.CALLPKT:
            t = self.get_expr()
            expr = self.get_expr()
            f = self.defs[t]
            if type(f) == tuple:
                f = f[0]
            r = f(self,expr)
            self.put_expr(r)
            ml.MLEndPacket(self.mlp)
        elif pkt == ml.EVALUATEPKT:
            expr = (self.get_expr(),)
            expr = self.py(expr)
            self.put_expr(expr)
            ml.MLEndPacket(self.mlp)
        elif pkt == ml.RETURNPKT:
            expr = self.get_expr()
            return expr
        else:
            print(self.get_expr())
        return pkt != 0
    
    def py(self,expr):
        if len(expr) == 2:
            r = expr[1]
            if type(r) in string and r[0] == '"':
                r = r[1:-1]
                if r[:3] == 'def' or r[:4] == 'from' or r[:6] == 'import' or r.find('=') > 0:
                    try:
                        r = compile(r, '<string>', 'exec')
                    except Exception as e:
                        return Expr(u'Error', *traceback.format_exc().splitlines()[3:])
                try:
                    r = eval(r, globals())
                except Exception as e:
                    return Expr(u'Error', *traceback.format_exc().splitlines()[3:])
            return pyeval(r)
        elif len(expr) == 3:
            r = expr[1]
            if type(r) in string:
                if r[0] == '"':
                    r = r[1:-1]
                t = self.py(Expr(None,expr[2])) 
                g = globals()
                g[r] = t
                return t
            elif type(r) == Expr:
                return pyset(*(r[1:] + (self.py(Expr(None,expr[2])),)))
        
    defs = [
        (py, u'Py[expr__]', u'{expr}')
    ]
    
def meval(*args):
    expr = args[-1]
    if type(expr) in string:
        expr = Expr(u'ToExpression', '"' + expr + '"')
    if len(args) == 2:
        expr = Expr(u'Set', args[0], expr)
    pl.put_expr(Expr(u'EvaluatePacket', expr))
    ml.MLEndPacket(pl.mlp)
    return pl.answer()

def mfunc(expr):
    def f(*args):
        return meval(pl,Expr(expr, *args))
    return f

def mexpr(expr):
    t = type(expr)
    if t in (tuple, list):
        return Expr('List', *expr)
    elif t == dict:
        r = []
        for k,v in expr.items():
            r.append(Expr('Rule', k, mexpr(v)))
        return Expr('Association', *r)
    else:
        return expr
        
#globals()['$'] = mexpr

def List(*args):
    return list(args)

def Association(*args):
    r = {}
    for v in args:
        if type(v) == Expr:
            r[v[1]] = v[2]
    return r

if __name__ == '__main__':    
    PythonLink.init()
            
    pl = PythonLink()
    pl.install()

    r = pl.answer()
    while r:
        r = pl.answer()
        
    #pl.answer()
    #pl.put_expr(Expr('EvaluatePacket', Expr('ToExpression', '"100!"')))
    #print(pl.answer())
    #pl.put_expr(Expr('EvaluatePacket', Expr('ToExpression', '"10!"')))
    #print(pl.answer())

    pl.uninstall()
    
    PythonLink.deinit()