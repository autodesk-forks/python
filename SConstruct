# -*- python -*-
#
# Copyright (c) 2016 Stefan Seefeld
# All rights reserved.
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import SCons.Script.Main
import config
import config.ui
import platform
import os


#
# We try to mimic the typical autotools-workflow.
#
# * In a 'configure' step all the essential build parameters are established
#   (either by explicit command-line arguments or from configure checks)
# * A subsequent build step can then simply read the cached variables, so
#   users don't have to memorize and re-issue the arguments on each subsequent
#   invocation, and neither do the config checks need to be re-run.
#
# The essential part here is to define a 'config' target, which removes any
# caches that may still be lingering around, then runs the checks.

if 'config' in COMMAND_LINE_TARGETS:
    # Clear the cache
    try: os.remove('bin.SCons/config.py')
    except: pass
if not os.path.exists('bin.SCons/'):
    os.mkdir('bin.SCons/')
vars = Variables('bin.SCons/config.py', ARGUMENTS)
config.add_options(vars)
arch = ARGUMENTS.get('arch', platform.machine())
env = Environment(toolpath=['config/tools'],
                  tools=['default', 'libs', 'tests'],
                  variables=vars,
                  TARGET_ARCH=arch)

Help(config.ui.help(vars, env) + """
Variables are saved in bin.SCons/config.py and persist between scons invocations.
""")

if GetOption('help'):
    Return()

build_dir = config.prepare_build_dir(env)
config_log = '{}/config.log'.format(build_dir)

# configure
SConsignFile('{}/.sconsign'.format(build_dir))
#env.Decider('MD5-timestamp')
env.Decider('timestamp-newer')
checks = config.get_checks()
if 'config' in COMMAND_LINE_TARGETS:
    conf=env.Configure(custom_tests=checks, log_file=config_log, conf_dir=build_dir)
    if False in (getattr(conf, c)() for c in checks):
        Exit(1)
    env = conf.Finish()
    vars.Save('bin.SCons/config.py', env)

if not os.path.exists(config_log):
    print('Please run `scons config` first. (See `scons -h` for available options.)')
    Exit(1)

if not GetOption('verbose'):
    config.ui.pretty_output(env)    

# build
env['BPL_VERSION'] = '1.61'
for e in config.variants(env):
    variant_dir=e.subst("$BOOST_CURRENT_VARIANT_DIR")
    e.SConscript('src/SConscript', variant_dir=variant_dir + '/src',
                 exports = { 'env' : e.Clone(BOOST_LIB = 'python') })
    if 'test' in COMMAND_LINE_TARGETS:
        test_env = e.Clone(BOOST_LIB = 'python', BOOST_TEST = True)
        test_env.BoostUseLib('python')
        e.SConscript('test/SConscript', variant_dir=variant_dir + '/test',
                     exports = { 'env' : test_env })
