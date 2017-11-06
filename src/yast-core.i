%module ycp2

%feature("autodoc", "3");

%include std_string.i
%include std_map.i
%inline %{
using namespace std;
%}

%{
#include <ycp/YCPFloat.h>
#include "yast.h"
#include "ytypes.h"
#include "YPythonCode.h"
#include <ycp/YCPCode.h>
#include <ycp/YCPMap.h>
%}

%feature("valuewrapper") YCPBoolean;
class YCPBoolean;
%feature("valuewrapper") YCPInteger;
class YCPInteger;
%feature("valuewrapper") YCPString;
class YCPString;

%ignore YCPTermRep;
%include <ycp/YCPTerm.h>
%ignore YCPListRep;
%include <ycp/YCPList.h>
%ignore YCPStringRep;
%include <ycp/YCPString.h>
%ignore YCPSymbolRep;
%include <ycp/YCPSymbol.h>
%ignore YCPIntegerRep;
%include <ycp/YCPInteger.h>
%ignore YCPBooleanRep;
%include <ycp/YCPBoolean.h>
%ignore YCPFloatRep;
%include <ycp/YCPFloat.h>
%ignore YCPMapRep;
%ignore YCPMapIterator;
%include "YCPMap.h"

%typemap(in) YCodePtr {
    $1 = new YPythonCode($input);
}
%typemap(typecheck,precedence=5000) YCodePtr {
    $1 = PyFunction_Check($input);
}

%ignore YCPCodeRep;
%ignore YCPEntryRep;
%ignore YCPEntry;
%include <ycp/YCPCode.h>

%varargs(25, char * opt = NULL) Opt;

%typemap(out) YCPValue {
    $result = ycp_to_pyval($1);
}

%include "yast.h"

