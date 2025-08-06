import os, shutil, platform
from SCons.Script import Environment, Exit, SConscript, ARGUMENTS

# ============================================================================
# Build Configuration Detection
# ============================================================================

target_platform = ARGUMENTS.get('platform', 'windows').lower()
build_config = ARGUMENTS.get('config', 'Debug').lower()
architecture = platform.machine()
if architecture == 'AMD64':
    architecture = 'x86_64'

def detect_msvc():
    """Check if MSVC compiler is available"""
    test_env = Environment(ENV=os.environ)
    if test_env.Detect('cl'):
        return True
    return bool(os.environ.get('VSINSTALLDIR') or os.environ.get('VCINSTALLDIR'))

def detect_compiler_tools():
    """Detect and return appropriate compiler tools for the target platform"""
    if target_platform.startswith('windows'):
        use_mingw = ARGUMENTS.get('use_mingw', 'no').lower() in ['yes', 'true', '1']
        if not use_mingw and detect_msvc():
            return ['msvc', 'mslib', 'mslink'], 'msvc'
        elif shutil.which('g++'):
            return ['gcc', 'g++', 'ar', 'link'], 'gcc'
        else:
            print("ERROR: No compiler found; install MSVC or MinGW.")
            Exit(1)
    elif target_platform.startswith('linux'):
        if shutil.which('g++'):
            return ['gcc', 'g++', 'ar', 'link'], 'gcc'
        elif shutil.which('clang++'):
            return ['clang', 'clang++', 'ar', 'link'], 'clang'
        else:
            print("ERROR: install g++ or clang++")
            Exit(1)
    elif target_platform.startswith('darwin'):
        return ['clang', 'clang++', 'ar', 'link'], 'clang'
    else:
        print(f"ERROR: Unsupported platform: {target_platform}")
        Exit(1)

# Detect compiler
tools, compiler_type = detect_compiler_tools()
print(f"Platform: {target_platform}, Compiler: {compiler_type}, Architecture: {architecture}")

# Validate Vulkan SDK
vulkan_sdk = os.environ.get('VULKAN_SDK')
if not vulkan_sdk or not os.path.isdir(vulkan_sdk):
    print("ERROR: Set VULKAN_SDK environment variable to your Vulkan SDK root")
    Exit(1)

# ============================================================================
# Build Environment Setup
# ============================================================================

base_env = Environment(
    tools=tools,
    ENV=os.environ,
)

base_env.Append(
    CPPPATH=[os.path.join(vulkan_sdk, 'Include')],
    LIBPATH=[os.path.abspath('build/Axiom')],
)

# Platform-specific libraries
if target_platform.startswith('windows'):
    base_env.Append(LIBS=['vulkan-1', 'user32', 'gdi32', 'winmm'])
else:
    base_env.Append(LIBS=['vulkan'])

# ============================================================================
# Compiler-Specific Build Flags
# ============================================================================

def get_build_flags(compiler):
    """Return build flags based on compiler type"""
    if compiler == 'msvc':
        return {
            'debug_ccflags': ['/Zi', '/Od', '/EHsc'],
            'release_ccflags': ['/O2', '/EHsc'],
            'debug_linkflags': ['/DEBUG'],
            'release_linkflags': [],
            'debug_defines': ['AX_DEBUG', 'AX_ENABLE_ASSERTS'],
            'release_defines': ['AX_RELEASE']
        }
    else:  # GCC or Clang
        return {
            'debug_ccflags': ['-g', '-O0'],
            'release_ccflags': ['-O3', '-march=native'],
            'debug_linkflags': [],
            'release_linkflags': [],
            'debug_defines': ['AX_DEBUG', 'AX_ENABLE_ASSERTS'],
            'release_defines': ['AX_RELEASE']
        }

flags = get_build_flags(compiler_type)

debug_env = base_env.Clone(
    CCFLAGS=flags['debug_ccflags'],
    LINKFLAGS=flags['debug_linkflags'],
    CPPDEFINES=flags['debug_defines']
)

release_env = base_env.Clone(
    CCFLAGS=flags['release_ccflags'],
    LINKFLAGS=flags['release_linkflags'],
    CPPDEFINES=flags['release_defines']
)

# ============================================================================
# Build Information Package for Sub-Scripts
# ============================================================================

build_info = {
    'platform': target_platform,
    'architecture': architecture,
    'compiler': compiler_type,
    'config': build_config,
    'vulkan_sdk': vulkan_sdk
}

# ============================================================================
# Sub-Script Invocation
# ============================================================================

SConscript('Axiom/SConscript', 
          variant_dir='Build/Axiom', 
          duplicate=0, 
          exports=['base_env', 'debug_env', 'release_env', 'build_info'])

SConscript('Theorem/SConscript', 
          variant_dir='Build/Theorem', 
          duplicate=0, 
          exports=['base_env', 'debug_env', 'release_env', 'build_info'])
