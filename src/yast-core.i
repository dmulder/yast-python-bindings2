%module ycp2

%feature("autodoc", "3");

%include std_string.i
%include std_map.i
%inline %{
using namespace std;
%}

%{
/* #include <YCP_UI.h> */
#include <ycp/YCPFloat.h>
#include "wrap.h"
#include "yast.h"
%}

%catches(std::runtime_error ...) UI::QueryWidget(const string & widgetId, const string & property);
%catches(std::runtime_error ...) UI::UserInput();
%catches(std::runtime_error ...) UI::WaitForEvent(const int & timeout);

%feature("valuewrapper") YCPBoolean;
class YCPBoolean;
%feature("valuewrapper") YCPInteger;
class YCPInteger;
%feature("valuewrapper") YCPString;
class YCPString;

/* %rename(UI) YCP_UI; */
/* %include <YCP_UI.h> */

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
%varargs(25, char * opt = NULL) Opt;
%template() std::map<std::string, std::string>;
%include "wrap.h"
%varargs(25, YCPValue * val = NULL) CallYCPFunction;
%include "yast.h"

