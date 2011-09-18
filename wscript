from shutil import copy

srcdir = '.'
blddir = 'build'
VERSION = '0.0.1'

def set_options(opt):
  opt.tool_options('compiler_cxx')

def configure(conf):
  conf.check_tool('compiler_cxx')
  conf.check_tool('node_addon')

def build(bld):
  obj = bld.new_task_gen('cxx', 'shlib', 'node_addon')
  obj.target = 'htmlscanner'
  obj.source = 'src/xh_scanner.cc src/htmlscanner.cc'

def install(inst):
  copy('build/Release/htmlscanner.node', 'lib/htmlscanner.node')
