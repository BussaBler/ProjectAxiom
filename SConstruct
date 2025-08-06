import os, shutil
from SCons.Script import Environment, Exit, SConscript, ARGUMENTS

platform = ARGUMENTS.get('platform', 'windows').lower()

def msvc_available():
    test_env = Environment(ENV=os.environ)
    if test_env.Detect('cl'):
        return True
    if os.environ.get('VSINSTALLDIR') or os.environ.get('VCINSTALLDIR'):
        return True
    return False

tools = []
if platform.startswith('windows'):
    use_mingw = ARGUMENTS.get('use_mingw', 'no').lower() in ['yes','true','1']
    if not use_mingw and msvc_available():
        tools = ['msvc', 'mslib', 'mslink']
    elif shutil.which('g++'):
        tools = [ 'gcc', 'g++', 'ar', 'link' ]
    else:
        print("ERROR: No compiler found; install MSVC or MinGW.")
        Exit(1)
elif platform.startswith('linux'):
    if shutil.which('g++'):
        tools = ['gcc', 'g++', 'ar']
    elif shutil.which('clang++'):
        tools = ['clang', 'clang++', 'ar']
    else:
        print("ERROR: install g++ or clang++")
        Exit(1)
else:
    print("ERROR: Unsupported platform")
    Exit(1)

VULKAN_SDK = os.environ.get('VULKAN_SDK')
if not VULKAN_SDK or not os.path.isdir(VULKAN_SDK):
    print("ERROR: Set VULKAN_SDK to your SDK root")
    Exit(1)

print("Using tools:", tools)


env = Environment(
    tools=tools,
    ENV=os.environ,
)

env.Append(
    CPPPATH=[ os.path.join(VULKAN_SDK, 'Include') ],
    LIBPATH=[ os.path.abspath('build/Axiom') ],
)

if platform.startswith('windows'):
    env.Append(LIBS=['vulkan-1','user32','gdi32','winmm'])
else:
    env.Append(LIBS=['vulkan'])


if 'msvc' in tools:
    debug_ccflags   = ['/Zi','/Od', '/EHsc']
    release_ccflags = ['/O2', '/EHsc']
    debug_linkflags   = ['/DEBUG']
    release_linkflags = []
    debug_defines   = ['AX_DEBUG','AX_ENABLE_ASSERTS']
    release_defines = ['AX_RELEASE']
else:
    debug_ccflags   = ['-g','-O0']
    release_ccflags = ['-O3','-march=native']
    debug_linkflags   = []
    release_linkflags = []
    debug_defines   = ['AX_DEBUG','AX_ENABLE_ASSERTS']
    release_defines = ['AX_RELEASE']


Debug = env.Clone(
    CCFLAGS=debug_ccflags,
    LINKFLAGS=debug_linkflags,
    CPPDEFINES=debug_defines
)

Release = env.Clone(
    CCFLAGS=release_ccflags,
    LINKFLAGS=release_linkflags,
    CPPDEFINES=release_defines
)

SConscript('Axiom/SConscript', variant_dir='build/Axiom', duplicate=0, exports=['env', 'Debug','Release'])
SConscript('Theorem/SConscript', variant_dir='build/Theorem', duplicate=0, exports=['env', 'Debug','Release'])
