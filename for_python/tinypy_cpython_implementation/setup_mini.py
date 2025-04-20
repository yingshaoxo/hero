
import os
import sys

VARS = {'$CPYTHON':''}
TOPDIR = os.path.abspath(os.path.dirname(__file__))
TEST = False
CLEAN = False
BOOT = False
DEBUG = False
VALGRIND = False
SANDBOX = False
CORE = ['tokenize','parse','encode','py2bc']
MODULES = []

def main():
    vars_linux()

    build_gcc()

HELP = """
python setup.py
"""

def vars_linux():
    VARS['$RM'] = 'rm -f'
    VARS['$VM'] = './vm'
    VARS['$TINYPY'] = './tinypy'
    VARS['$SYS'] = '-linux'
    VARS['$FLAGS'] = ''

    VARS['$WFLAGS'] = '-std=c89 -Wall -Wc++-compat'
    #-Wwrite-strings - i think this is included in -Wc++-compat

    if 'pygame' in MODULES:
        VARS['$FLAGS'] += ' `sdl-config --cflags --libs` '

def do_cmd(cmd):
    for k,v in VARS.items():
        cmd = cmd.replace(k,v)
    if '$' in cmd:
        sys.exit(-1)
    r = os.system(cmd)
    if r:
        sys.exit(r)

def do_chdir(dest):
    os.chdir(dest)

def build_bc(opt=False):
    out = []
    for mod in CORE:
        out.append("""unsigned char tp_%s[] = {"""%mod)
        fname = mod+".tpc"
        data = open(fname,'rb').read()
        cols = 16
        for n in xrange(0,len(data),cols):
            out.append(",".join([str(ord(v)) for v in data[n:n+cols]])+',')
        out.append("""};""")
    out.append("")
    f = open('bc.c','wb')
    f.write('\n'.join(out))
    f.close()

def open_tinypy(fname,*args):
    return open(os.path.join(TOPDIR,'tinypy',fname),*args)

def py2bc(cmd,mod):
    src = '%s.py'%mod
    dest = '%s.tpc'%mod
    if not os.path.exists(dest) or os.stat(src).st_mtime > os.stat(dest).st_mtime:
        cmd = cmd.replace('$SRC',src)
        cmd = cmd.replace('$DEST',dest)
        do_cmd(cmd)

def build_gcc():
    mods = CORE[:]
    do_chdir(os.path.join(TOPDIR,'tinypy'))
    nopos = ' -nopos '
    for mod in mods:
        py2bc('python py2bc.py $SRC $DEST'+nopos,mod)
    build_bc(True)
    do_cmd('rm -fr *.tpc')
    do_cmd('rm -fr *.pyc')
    return

if __name__ == '__main__':
    main()
