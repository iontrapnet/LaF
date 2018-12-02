if __name__ == '__main__':
    import os
    os.environ['JUPYTERLAB_DIR'] = os.path.abspath('./Lib/jupyterlab')
    import sys,jupyterlab.labapp
    sys.exit(jupyterlab.labapp.main())
