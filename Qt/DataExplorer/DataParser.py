class DataParser:
    def __init__(self):
        self.items = {}
        self.item = ''
        self.value = ''
        self.mode = 0
        self.depth = 0
        self.separator = ''
    
    def parse(self, line):
        if self.mode == 0:
            if line[-1] == '=':
                self.item = line[0:-1]
                self.mode = 1
            else:
                index = line.find('=')
                if index != -1:
                    self.item = line[0:index]
                    self.value = line[index+1:]
                    self.items[self.item] = self.value
        elif self.mode == 1:
            if line[-1] == '{':
                self.value = ''
                self.mode = 2
                self.depth = 1
                self.separator = line[0:-1]
        elif self.mode == 2:
            if line == self.separator + '{':
                self.depth = self.depth + 1
            elif line == '}' + self.separator:
                self.depth = self.depth - 1
            if self.depth == 0:
                self.items[self.item] = self.value
                self.mode = 0
            else:
                self.value = self.value + line + '\n'
            
    def parseText(self, text):
        self.items = {}
        lines = text.strip().replace('/\r/g','').split('\n')
        #print lines
        for line in lines:
            self.parse(line)
        if self.mode == 2:
            self.items[self.item] = self.value
        return self.items
    
    def parseStream(self, stream):
        pass
    
if __name__ == '__main__':
    parser = DataParser()
    result = parser.parseText('''
aa=bb
cc=
{
dd
}
ee=ff
''') 
    print result