#include <y2/Y2Namespace.h>
#include <y2/Y2Component.h>
#include <y2/Y2ComponentCreator.h>
#include <ycp/y2log.h>
#include <ycp/YBlock.h>
#include <ycp/YExpression.h>
#include <ycp/YStatement.h>
#include <ycp/Import.h>
#include <ycp-ui/YUIComponent.h>
#include <wfm/Y2WFMComponent.h>
#include <ycp/Parser.h>
#include <ycp/YCPMap.h>
#include <ycp/YCPList.h>
#include <ycp/YCPPath.h>
#include <ycp/YCPTerm.h>
#include <ycp/YCPString.h>
#include <ycp/YCPVoid.h>
#include <ycp/YCPFloat.h>
#include <ycp/YCPBoolean.h>
#include <ycp/SymbolTable.h>
#include <yui/YUILoader.h>
#include <yui/YSettings.h>
#include <Python.h>
#include "YPythonCode.h"

YCPValue pyval_to_ycp(PyObject *input);
PyObject *ycp_to_pyval(YCPValue val);

