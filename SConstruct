import os, shutil, platform, multiprocessing
from SCons.Script import Environment, Exit, SConscript, ARGUMENTS, Default, SetOption, Alias, GetOption

SetOption('num_jobs', multiprocessing.cpu_count())
target_platform = ARGUMENTS.get('platform', 'windows').lower()
build_config = ARGUMENTS.get('config', 'Debug').lower()
vsproj = ARGUMENTS.get('vsproj', 'no').lower() in ['yes', 'true', '1']
verbose = ARGUMENTS.get('verbose', 'no').lower() in ['yes', 'true', '1']
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

tools, compiler_type = detect_compiler_tools()

if vsproj and compiler_type == 'msvc':
    tools.extend(['msvs'])
    print(f"Platform: {target_platform}, Compiler: {compiler_type}, Architecture: {architecture}")
    print("Visual Studio project generation enabled")
else:
    print(f"Platform: {target_platform}, Compiler: {compiler_type}, Architecture: {architecture}")

vulkan_sdk = os.environ.get('VULKAN_SDK')
if not vulkan_sdk or not os.path.isdir(vulkan_sdk):
    print("ERROR: Set VULKAN_SDK environment variable to your Vulkan SDK root")
    Exit(1)

if vsproj and compiler_type != 'msvc':
    print("ERROR: Visual Studio project generation requires MSVC compiler")
    Exit(1)

base_env = Environment(
    tools=tools,
    ENV=os.environ,
)

base_env.Append(
    CPPPATH=[os.path.join(vulkan_sdk, 'Include'), os.path.join(vulkan_sdk, 'include')],
    LIBPATH=[os.path.abspath('Build/Axiom'), os.path.abspath('Build/ImGui')],
)

if target_platform.startswith('windows'):
    base_env.Append(LIBS=['vulkan-1', 'user32', 'gdi32', 'winmm'])
else:
    base_env.Append(LIBS=['vulkan', 'X11'])

def get_build_flags(compiler):
    """Return build flags based on compiler type"""
    if compiler == 'msvc':
        return {
            'debug_ccflags': ['/Zi', '/Od', '/EHsc', '/nologo', '/FS', '/MDd'],
            'release_ccflags': ['/O2', '/EHsc', '/nologo', '/FS', '/MD'],
            'debug_linkflags': ['/DEBUG', '/nologo'],
            'release_linkflags': ['/nologo'],
            'debug_defines': ['AX_DEBUG', 'AX_ENABLE_ASSERTS'],
            'release_defines': ['AX_RELEASE'],
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

build_info = {
    'platform': target_platform,
    'architecture': architecture,
    'compiler': compiler_type,
    'config': build_config,
    'vulkan_sdk': vulkan_sdk,
    'vsproj': vsproj
}

def setup_output(env):
    if verbose:
        return {}
    
    colors = {
        'reset': '\033[0m',
        'bold': '\033[1m',
        'green': '\033[32m',
        'blue': '\033[34m',
        'yellow': '\033[33m',
        'cyan': '\033[36m',
        'magenta': '\033[35m'
    }
    
    disable_colors = ARGUMENTS.get('no-color', 'no').lower() in ['yes', 'true', '1']
    if disable_colors:
        colors = {key: '' for key in colors}
    
    env['CXXCOMSTR'] = f"{colors['green']}[COMPILE]{colors['reset']} $SOURCE"
    env['CCCOMSTR'] = f"{colors['green']}[COMPILE]{colors['reset']} $SOURCE"
    env['LINKCOMSTR'] = f"{colors['yellow']}[LINK]{colors['reset']} $TARGET"
    env['ARCOMSTR'] = f"{colors['blue']}[ARCHIVE]{colors['reset']} $TARGET"
    env['RANLIBCOMSTR'] = f"{colors['blue']}[RANLIB]{colors['reset']} $TARGET"
    env['LIBCOMSTR'] = f"{colors['blue']}[LIB]{colors['reset']} $TARGET"
    
    return colors

def setup_progress_indicator():
    action = 'Cleaning' if GetOption('clean') else 'Building'
    print(f"\n{action} {build_config.capitalize()} configuration for {target_platform}-{architecture}")
    print(f"   Compiler: {compiler_type}")
    print(f"   Vulkan SDK: {vulkan_sdk}")
    if not verbose:
        print("   (Use 'verbose=yes' to see full command lines)")
    print("")
    
    return None

setup_progress_indicator()
colors = setup_output(base_env)
setup_output(debug_env)
setup_output(release_env)

imgui_lib, imgui_project = SConscript('Axiom/Vendor/ImGui/SConscript', 
            variant_dir='Build/ImGui',
            duplicate=0, 
            exports=['base_env', 'debug_env', 'release_env', 'build_info'])

axiom_lib, axiom_project = SConscript('Axiom/SConscript', 
            variant_dir='Build/Axiom', 
            duplicate=0, 
            exports=['base_env', 'debug_env', 'release_env', 'build_info'])

theorem_app, theorem_project = SConscript('Theorem/SConscript',
            variant_dir='Build/Theorem',
            duplicate=0,
            exports=['base_env', 'debug_env', 'release_env', 'build_info'])

if vsproj and compiler_type == 'msvc':
    axiom_solution = base_env.MSVSSolution(
        target='AxiomEngine' + base_env['MSVSSOLUTIONSUFFIX'],
        projects=[imgui_project, axiom_project, theorem_project],
        variant=['Debug|x64']
    )

    Alias('AxiomEngine', axiom_solution)
    Default('AxiomEngine')

    action = 'cleaned' if GetOption('clean') else 'generated'
    print(f"Visual Studio projects and solution will be {action}:")
    print("  - ImGui.vcxproj")
    print("  - Axiom.vcxproj")
    print("  - Theorem.vcxproj")
    print("  - AxiomEngine.sln")