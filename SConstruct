import os, shutil, platform, multiprocessing
from SCons.Script import Environment, Progress
from SCons.Script import Exit, SConscript, ARGUMENTS, Default, SetOption, Alias, GetOption

SetOption('num_jobs', multiprocessing.cpu_count())
targetPlatform = ARGUMENTS.get('platform', 'windows').lower()
buildConfig = ARGUMENTS.get('config', 'debug').lower()
vsproj = ARGUMENTS.get('vsproj', 'no').lower() in ['yes', 'true', '1']
verbose = ARGUMENTS.get('verbose', 'no').lower() in ['yes', 'true', '1']
architecture = platform.machine()
if architecture == 'AMD64':
    architecture = 'x86_64'

COLORS = {
    'reset': '\033[0m',
    'bold': '\033[1m',
    'green': '\033[32m',
    'blue': '\033[34m',
    'yellow': '\033[33m',
    'cyan': '\033[36m',
    'magenta': '\033[35m',
    'red': '\033[31m'
}

def detectMsvc():
    env = Environment(tools=['default', 'msvc'])

    if not (env.WhereIs('cl') or env.WhereIs('cl.exe')):
        return False
    return True

def printWithColor(*objects, color, sep=' ', end='\n', file=None, flush=False):
    print(f'{color}', end='', file=file, flush=flush)
    print(*objects, '\033[0m', sep=sep, end=end, file=file, flush=flush)

def detectCompilerTools():
    """detect and return appropriate compiler tools for the target platform"""
    if targetPlatform.startswith('windows'):
        compilerType = ARGUMENTS.get('compiler', 'msvc').lower()
        match compilerType:
            case 'msvc':
                if detectMsvc():
                    return ['msvc', 'mslib', 'mslink'], 'msvc'
                else:
                    printWithColor("ERROR: No MSVC compiler found; install a valid MSVC compiler or use a different one.", color=COLORS['red'])
                    Exit(1)
            case 'gcc' | 'g++':
                if shutil.which('g++') or shutil.which('gcc'):
                    return ['gcc', 'g++', 'ar', 'link'], 'gcc'
                else:
                    printWithColor("ERROR: No GCC compiler found; install a valid GCC compiler or use a different one.", color=COLORS['red'])
                    Exit(1)
            case 'clang':
                if shutil.which('clang') or shutil.which('clang++'):
                    return ['clang', 'clang++', 'llvm-ar', 'llvm-link'], 'clang'
                else:
                    printWithColor("ERROR: No Clang compiler found; install a valid Clang compiler or use a different one.", color=COLORS['red'])
                    Exit(1)
    elif targetPlatform.startswith('linux'):
        compilerType = ARGUMENTS.get('compiler', 'gcc').lower()
        match compilerType:
            case 'gcc' | 'g++':
                if shutil.which('g++') or shutil.which('gcc'):
                    return ['gcc', 'g++', 'ar', 'link'], 'gcc'
                else:
                    printWithColor("ERROR: No GCC compiler found; install a valid GCC compiler or use a different one.", color=COLORS['red'])
                    Exit(1)
            case 'clang':
                if shutil.which('clang') or shutil.which('clang++'):
                    return ['clang', 'clang++', 'llvm-ar', 'llvm-link'], 'clang'
                else:
                    printWithColor("ERROR: No Clang compiler found; install a valid Clang compiler or use a different one.", color=COLORS['red'])
                    Exit(1)
    else:
        printWithColor(f'Current platform {targetPlatform} not supported.', color=COLORS['red'])
        Exit(1)


tools, compilerType = detectCompilerTools()

printWithColor(f"Platform: {targetPlatform}, Compiler: {compilerType}, Architecture: {architecture}", color=COLORS['cyan'])
if vsproj and compilerType == 'msvc':
    tools.extend(['msvs'])
    printWithColor("Visual Studio project generation enabled", color=COLORS['cyan'])

vulkanSdk = os.environ.get('VULKAN_SDK')
if not vulkanSdk or not os.path.isdir(vulkanSdk):
    printWithColor("ERROR: Set VULKAN_SDK environment variable to your Vulkan SDK root", color=COLORS['red'])
    Exit(1)

if vsproj and compilerType != 'msvc':
    printWithColor("ERROR: Visual Studio project generation requires MSVC compiler", color=COLORS['red'])
    Exit(1)

baseEnv = Environment(
    tools=tools,
    ENV=os.environ,
)

baseEnv.Append(
    CPPPATH=[os.path.join(vulkanSdk, 'Include'), os.path.join(vulkanSdk, 'include')],
    LIBPATH=[os.path.abspath('Build/Axiom'), os.path.abspath('Build/ImGui')],
)

if targetPlatform.startswith('windows'):
    baseEnv.Append(LIBS=['vulkan-1', 'user32', 'gdi32', 'winmm'])
else:
    baseEnv.Append(LIBS=['vulkan', 'X11'])

def getBuildFlags(compiler):
    """return build flags based on compiler type"""
    if compiler == 'msvc':
        return {
            'debugCcflags': ['/Zi', '/Od', '/EHsc', '/nologo', '/FS', '/MDd', '/permissive-'],
            'releaseCcflags': ['/O2', '/EHsc', '/nologo', '/FS', '/MD'],
            'debugLinkflags': ['/DEBUG', '/nologo'],
            'releaseLinkflags': ['/nologo'],
            'debugDefines': ['AX_DEBUG', 'AX_ENABLE_ASSERTS'],
            'releaseDefines': ['AX_RELEASE'],
        }
    else:  # GCC or Clang
        return {
            'debugCcflags': ['-g', '-O0', '-pedantic'],
            'releaseCcflags': ['-O3', '-march=native'],
            'debugLinkflags': [],
            'releaseLinkflags': [],
            'debugDefines': ['AX_DEBUG', 'AX_ENABLE_ASSERTS'],
            'releaseDefines': ['AX_RELEASE']
        }

flags = getBuildFlags(compilerType)

debugEnv = baseEnv.Clone(
    CCFLAGS=flags['debugCcflags'],
    LINKFLAGS=flags['debugLinkflags'],
    CPPDEFINES=flags['debugDefines']
)

releaseEnv = baseEnv.Clone(
    CCFLAGS=flags['releaseCcflags'],
    LINKFLAGS=flags['releaseLinkflags'],
    CPPDEFINES=flags['releaseDefines']
)

buildInfo = {
    'platform': targetPlatform,
    'architecture': architecture,
    'compiler': compilerType,
    'config': buildConfig,
    'vulkanSdk': vulkanSdk,
    'vsproj': vsproj
}

def setupOutputColors(env):
    if verbose:
        return {}
    
    disableColors = ARGUMENTS.get('no-color', 'no').lower() in ['yes', 'true', '1']
    if disableColors:
        env['CXXCOMSTR'] = f"{COLORS['reset']}[COMPILE]{COLORS['reset']} $SOURCE"
        env['CCCOMSTR'] = f"{COLORS['reset']}[COMPILE]{COLORS['reset']} $SOURCE"
        env['LINKCOMSTR'] = f"{COLORS['reset']}[LINK]{COLORS['reset']} $TARGET"
        env['ARCOMSTR'] = f"{COLORS['reset']}[ARCHIVE]{COLORS['reset']} $TARGET"
        env['RANLIBCOMSTR'] = f"{COLORS['reset']}[RANLIB]{COLORS['reset']} $TARGET"
        env['LIBCOMSTR'] = f"{COLORS['reset']}[LIB]{COLORS['reset']} $TARGET"
    else:
        env['CXXCOMSTR'] = f"{COLORS['green']}[COMPILE]{COLORS['reset']} $SOURCE"
        env['CCCOMSTR'] = f"{COLORS['green']}[COMPILE]{COLORS['reset']} $SOURCE"
        env['LINKCOMSTR'] = f"{COLORS['yellow']}[LINK]{COLORS['reset']} $TARGET"
        env['ARCOMSTR'] = f"{COLORS['blue']}[ARCHIVE]{COLORS['reset']} $TARGET"
        env['RANLIBCOMSTR'] = f"{COLORS['blue']}[RANLIB]{COLORS['reset']} $TARGET"
        env['LIBCOMSTR'] = f"{COLORS['blue']}[LIB]{COLORS['reset']} $TARGET"


def printInfo():
    action = 'Cleaning' if GetOption('clean') else 'Building'
    printWithColor(f'\n{action} {buildConfig.capitalize()} configuration for {targetPlatform}-{architecture}', color=COLORS['cyan'])
    printWithColor(f' Compiler: {compilerType}', color=COLORS['magenta'])
    printWithColor(f' Vulkan SDK: {vulkanSdk}', color=COLORS['magenta'])
    if not verbose:
        printWithColor(' (Use \'verbose=yes\' to see full command lines)', color=COLORS['yellow'])
    print("")
    return None

printInfo()
setupOutputColors(baseEnv)
setupOutputColors(debugEnv)
setupOutputColors(releaseEnv)

imguiLib, imguiProject = SConscript('Axiom/Vendor/ImGui/SConscript', 
            variant_dir='Build/ImGui',
            duplicate=0,
            exports=['baseEnv', 'debugEnv', 'releaseEnv', 'buildInfo'])

axiomLib, axiomProject = SConscript('Axiom/SConscript', 
            variant_dir='Build/Axiom', 
            duplicate=0, 
            exports=['baseEnv', 'debugEnv', 'releaseEnv', 'buildInfo'])

theoremApp, theoremProject = SConscript('Theorem/SConscript',
            variant_dir='Build/Theorem',
            duplicate=0,
            exports=['baseEnv', 'debugEnv', 'releaseEnv', 'buildInfo'])

if vsproj and compilerType == 'msvc':
    axiom_solution = baseEnv.MSVSSolution(
        target='AxiomEngine' + baseEnv['MSVSSOLUTIONSUFFIX'],
        projects=[imguiProject, axiomProject, theoremProject],
        variant=['Debug|x64']
    )

    Alias('AxiomEngine', axiom_solution)
    Default('AxiomEngine')

    if GetOption('clean'):
        action = 'cleaned'
        color = COLORS['red']
        symbol = '-'
    else:
        action = 'generated'
        color = COLORS['green']
        symbol = '+'
    printWithColor(f'Visual Studio projects and solution will be {action}:', color=COLORS['cyan'])
    printWithColor(f' {symbol}', 'ImGui.vcproj', color=color)
    printWithColor(f' {symbol}', 'Axiom.vcxproj', color=color)
    printWithColor(f' {symbol}', 'Theorem.vcxproj', color=color)
    printWithColor(f' {symbol}', 'AxiomEngine.sln', color=color)