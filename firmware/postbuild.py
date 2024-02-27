import shutil 
import datetime
import subprocess

Import("env", "projenv")

def after_build(source, target, env):
    version = 'v1.0.0.'
    PLATFORM = "PDSPico_pico"
    BUILD_DIR = "./builds/"

    FILENAME_VERSION_H = 'include/version.h'
    BUILD_FILE = '.pio/build/{}/firmware.uf2'.format(PLATFORM)

    # update version for next build  
    FILENAME_BUILDNO = 'versioning'
    
    # maj.minor.patch.build
    
    build_no = 0
    try:
        with open(FILENAME_BUILDNO) as f:
            build_no = int(f.readline())
    except:
        print('Starting build number from 1..')
        build_no = 1
      
    result = subprocess.run(['git', 'rev-parse', '--short=8','HEAD'], stdout=subprocess.PIPE).stdout.decode('utf-8').strip()
    print('Git rev {}'.format(result))

    ver_short = version+str(build_no)+"-"+result
     
    NEWFILE = BUILD_DIR + BUILD_FILE[(BUILD_FILE.rfind('/')+1):].replace("firmware","{}_{}".format(PLATFORM,ver_short))

    print ("Copying build binary: " + NEWFILE)
    shutil.copyfile(BUILD_FILE,NEWFILE)
    
    # take a diff
    subprocess.run(['cmd','/c', 'git diff > {}/diffs/{}.diff'.format(BUILD_DIR,build_no)])
        
    # write a new build number for next build   
    build_no = build_no + 1 
    
    with open(FILENAME_BUILDNO, 'w+') as f:
        f.write(str(build_no))
      
# run after build done
env.AddPostAction("buildprog", after_build)