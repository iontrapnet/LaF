#!D:/conda2\python.exe
# EASY-INSTALL-ENTRY-SCRIPT: 'nbconvert==5.1.1','console_scripts','jupyter-nbconvert'
__requires__ = 'nbconvert==5.1.1'
import re
import sys
from pkg_resources import load_entry_point

if __name__ == '__main__':
    sys.argv[0] = re.sub(r'(-script\.pyw?|\.exe)?$', '', sys.argv[0])
    sys.exit(
        load_entry_point('nbconvert==5.1.1', 'console_scripts', 'jupyter-nbconvert')()
    )
