Microcode:
  lines*=Line
;

Comment:
  /\/\/.*$/
;

PreprocessorDirective:
  "#" LineNumber=INT Filename=STRING Flags*=INT[' ']
;

Line:
  PreprocessorDirective | (Directive ';') | LabelAnchor | (MicroInstruction ';')
;

LabelAnchor:
  label=ID ':'
;

MicroInstruction:
  fields+=Field[',']
;

Field:
  name=ID arguments*=Argument
;

Directive:
  '.' directive=ID arguments*=Argument
;

Argument:
  ID | Integer
;

Integer:
  value=/0x[0-9a-fA-F]+/ | /\d+/
;
