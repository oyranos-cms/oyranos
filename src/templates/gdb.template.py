# -*- coding: utf-8 -*-
# Pretty-printers for Oyranos.

import gdb
import itertools
import re


##autoload:
# /usr/lib/debug/usr/local/lib/liboyranos.so.0.3.2-gdb.py
## TODO
# * Put common class functionality to a base class

indent = ''

class Struct:
  "Pretty Printer for oyStruct_s"
  name__ = "oyStruct_s"
  privName__ = "oyStruct_s"
  verbose__ = False

  def __init__(self, val):
    self.val = val

  def to_string(self):
    enum = str(self.val['type_'])
    if enum == 'oyOBJECT_STRUCT_LIST_S':
      self.message = StructList(self.val.cast(gdb.lookup_type("oyStructList_s"))).to_string()
    elif enum == 'oyOBJECT_OPTION_S':
      self.message = Option(self.val.cast(gdb.lookup_type("oyOption_s"))).to_string()
    else:
      oy = self.val['oy_'].dereference()
      self.message = "%s: [%s], [refcount: %s]" % (Struct.name__, self.val['type_'], oy['ref_'])
    return self.message

class StructList:
    "Pretty Printer for oyStructList_s"
    name__ = "oyStructList_s"
    privName__ = "oyStructList_s_"
    verbose__ = False

    def __init__(self, val):
        self.val = val
        self.priv = self.val.cast(gdb.lookup_type(StructList.privName__))

    def to_string(self):
      if StructList.verbose__:
        return self.to_string_verbose();
      else:
        return self.to_string_short();

    def to_string_short(self):
      message = "%s: [%s <= %s:parent], [used/reserved] -> [%d/%d], [name: %s]"
      args = (StructList.name__, self.priv['type_'], self.priv['parent_type_'],self.priv['n_'], self.priv['n_reserved_'], self.priv['list_name'])
      return message % args

    def to_string_verbose(self):
      global indent

      header = '\n' + indent + self.to_string_short()

      header += '\n' if self.priv['n_'] == 0 else '\n' + indent + 'Children:\n'
      footer = ''
      array = self.priv['ptr_']
      reserved = int(str(self.priv['n_reserved_']))
      for i in range(reserved):
        struct = array[i]
        if str(struct) == "0x0":
          footer += indent
          footer += '[%02d:%s] --> [NULL]\n' % (i, struct.address)
        else:
          indent += "   "
          args = (i, struct.address, struct.dereference().address, Struct(struct.dereference()).to_string())
          indent = indent[:-3]
          footer += indent + '[%02d:%s] --> { %s : %s }\n' % args

      return header + footer

class Value:
    "Pretty Printer for oyValue_u"
    name__ = "oyValue_u"
    privName__ = "oyValue_u"
    verbose__ = False

    def __init__(self, val, valtype):
      self.val = val
      self.valtype = valtype

    def to_string(self):
      if Value.verbose__:
        return self.to_string_verbose();
      else:
        return self.to_string_short();

    def to_string_short(self):
      if self.isList():
        message = "%s: [%s], [#elements: %s]"
        if str(valtype) == 'oyVAL_STRING_LIST':
          count = self.numStrElements()
        else:
          count = int(str(array.dereference()))
        args = (Value.name__, self.valtype, count)
        return message % args
      else:
        message = "%s: [%s], [element: %s]"
        args = (Value.name__, self.valtype, self.getScalarString())
        return message % args

    def to_string_verbose(self):
      global indent

      message = '\n' + indent + self.to_string_short()
      if self.isList():
        message += '\n' + indent + self.getListString()

      return header

    def isList(self):
      if 'LIST' in str(self.valtype):
        return True
      else:
        return False

    def getScalarString(self):
      v = str(self.valtype)
      if v == 'oyVAL_INT':
        return str(self.value['int32'])
      elif v == 'oyVAL_DOUBLE':
        return str(self.value['dbl'])
      elif v == 'oyVAL_STRING':
        return str(self.value['string'])
      elif v == 'oyVAL_STRUCT':
        return Struct(self.value['oy_struct'].dereference()).to_string()
      else:
        return None

    def getListString(self):
      elements = ''
      v = str(self.valtype)
      if v == 'oyVAL_INT_LIST':
        array = self.val['int32_list']
        N = int(str(array.dereference()))
        for i in range(1,N):
          elements += str(array[i]) + ' '
        return elements
      if v == 'oyVAL_DOUBLE_LIST':
        array = self.val['dbl_list']
        N = int(str(array.dereference()))
        for i in range(1,N):
          elements += str(array[i]) + ' '
        return elements
      if v == 'oyVAL_STRING_LIST':
        array = self.val['string_list']
        N = self.numStrElements()
        for i in range(0,N):
          elements += array[i].string() + '\n'
        return elements

    def numStrElements(self):
      i = 0
      array = self.val['string_list']
      while True:
        if str(array[i]) == "0x0":
          break
        else:
          i += 1

      return i

class Option:
    "Pretty Printer for oyOption_s"
    name__ = "oyOption_s"
    privName__ = "oyOption_s_"
    verbose__ = False

    def __init__(self, val):
        self.val = val
        self.priv = self.val.cast(gdb.lookup_type(Option.privName__))

    def to_string(self):
      if Option.verbose__:
        return self.to_string_verbose();
      else:
        return self.to_string_short();

    def to_string_short(self):
      message = "%s: [%s <= %s:value], [id:%d, registration:%s]"
      args = (Option.name__, self.val['type_'], self.priv['value_type'], self.priv['id'], self.priv['registration'].string())
      return message % args

    def to_string_verbose(self):
      message = self.to_string_short()
      message += '\n' + Value(self.priv['value'].dereference()).to_string_verbose()
      return message

class Options:
    "Pretty Printer for oyOptions_s"
    name__ = "oyOptions_s"
    privName__ = "oyOptions_s_"
    verbose__ = False

    def __init__(self, val):
        self.val = val
        self.priv = self.val.cast(gdb.lookup_type(Options.privName__))

    def to_string(self):
      if Options.verbose__:
        return self.to_string_verbose();
      else:
        return self.to_string_short();

    def to_string_short(self):
      oy = self.val['oy_'].dereference()
      list_ = self.priv['list_'].dereference().cast(gdb.lookup_type(StructList.privName__))
      message = "%s: [%s], [refcount: %s], [used/reserved] -> [%d/%d]"
      args = (Options.name__, self.val['type_'], oy['ref_'], list_.priv['n_'], list_.priv['n_reserved_'])
      return message % args

    def to_string_verbose(self):
      global indent
      list_ = self.priv['list_'].dereference().cast(gdb.lookup_type(StructList.privName__))

      indent += "   "
      message = '\n' + indent + self.to_string_short()
      message += indent + StructList(list_).to_string_verbose()
      indent = indent[:-3]

      return message

{% for class in classes %}
  {% if class.listOf %}
class {{ class.baseName }}:
    "Pretty Printer for {{ class.name }}"
    name__ = "{{ class.name }}"
    privName__ = "{{ class.privName }}"
    verbose__ = False

    def __init__(self, val):
        self.val = val
        self.priv = self.val.cast(gdb.lookup_type({{ class.baseName }}.privName__))

    def to_string(self):
      if {{ class.baseName }}.verbose__:
        return self.to_string_verbose();
      else:
        return self.to_string_short();

    def to_string_short(self):
      oy = self.val['oy_'].dereference()
      list_ = self.priv['list_'].dereference().cast(gdb.lookup_type(StructList.privName__))
      message = "%s: [%s], [refcount: %s], [used/reserved] -> [%d/%d]"
      args = ({{ class.baseName }}.name__, self.val['type_'], oy['ref_'], list_.priv['n_'], list_.priv['n_reserved_'])
      return message % args

    def to_string_verbose(self):
      global indent
      list_ = self.priv['list_'].dereference().cast(gdb.lookup_type(StructList.privName__))

      indent += "   "
      message = '\n' + indent + self.to_string_short()
      message += indent + StructList(list_).to_string_verbose()
      indent = indent[:-3]

      return message
  {% endif %}
{% endfor %}


oy_class_list = [ Struct, StructList, Option, Options ]

def setVerboseAll(state):
  for c in oy_class_list:
    c.verbose__ = state

class VerboseCommand(gdb.Command):
  """Set the verbosity of oyranos pretty printers
  
  Usage: oyverbose [True|False] [Class name1] [Class name2] ...
         oyverbose (the same as: oyverbose True)"""
  false = ["0", "False", "false", "no", "on"]
  true  = ["1", "True",  "true", "yes", "off"]

  def __init__ (self):
    super(VerboseCommand, self).__init__("oyverbose", gdb.COMMAND_SUPPORT)

  def invoke(self, arg, from_tty):
    argv = gdb.string_to_argv(arg)

    if len(argv) == 0:
      setVerboseAll(True)
    elif len(argv) == 1:
      self.setVerboseAllCheck(argv[0])
    else:
      state = self.stateCheck(argv.pop(0))
      if state != None:
        for c in oy_class_list:
          if c.name__ in argv or c.privName__ in argv:
            c.verbose__ = state

  def setVerboseAllCheck(self, arg):
    if arg in VerboseCommand.false:
      setVerboseAll(False)
    elif arg in VerboseCommand.true:
      setVerboseAll(True)
    else:
      print "Error: Wrong Argument: Expected <True|False>"

  def stateCheck(self, arg):
    if arg in VerboseCommand.false:
      return False
    elif arg in VerboseCommand.true:
      return True
    else:
      print "Error: Wrong Argument: Expected <True|False>"
      return None

VerboseCommand()

def lookup_function (val):
    "Look-up and return a pretty-printer that can print val."

    # Get the type.
    type = str(val.type);

    # Iterate over local dictionary of types to determine
    # if a printer is registered for that type.  Return an
    # instantiation of the printer if found.
    for function in pretty_printers_dict:
        if function.search(type):
            return pretty_printers_dict[function](val)

    #print '[%s] not found!' % (type)
    # Cannot find a pretty printer.  Return None.
    return None

def build_dictionary ():
    pretty_printers_dict[re.compile('^oyOption_s$')] = lambda val: Option(val)
    pretty_printers_dict[re.compile('^oyStruct_s$')] = lambda val: Struct(val)
    pretty_printers_dict[re.compile('^oyStructList_s$')] = lambda val: StructList(val)

pretty_printers_dict = {}

def register_oyranos_printers ():
    gdb.current_objfile().pretty_printers.append(lookup_function)

build_dictionary()

register_oyranos_printers()
