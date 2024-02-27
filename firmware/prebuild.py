import subprocess
import datetime

# maj.minor.patch.build
version = 'v1.0.0.'

subprocess.call(['powershell','-ExecutionPolicy','Bypass','-File','.\\uf2conv\\Generate-Headers.ps1'])

FILENAME_BUILDNO = 'versioning'
FILENAME_VERSION_H = 'include/version.h'

build_no = 0
try:
    with open(FILENAME_BUILDNO) as f:
        build_no = int(f.readline())
except:
    print('Starting build number from 1..')
    build_no = 1
    
result = subprocess.run(['git', 'rev-parse', '--short=8','HEAD'], stdout=subprocess.PIPE).stdout.decode('utf-8').strip()

print('Build number: {}'.format(build_no))
print('Git rev {}'.format(result))

hf = """
#ifndef BUILD_NUMBER
  #define BUILD_NUMBER "{}"
#endif
#ifndef VERSION_SHORT
  #define VERSION_SHORT "{}"
#endif
#ifndef VERSION_MICRO
  #define VERSION_MICRO "{}"
#endif
""".format(build_no, version+str(build_no)+"-"+result,version[0:4])
with open(FILENAME_VERSION_H, 'w+') as f:
    f.write(hf)
    