#include "yast.h"

static Y2Namespace * getNs(const char * ns_name)
{
    Import import(ns_name); // has a static cache
    Y2Namespace *ns = import.nameSpace();
    if (ns != NULL)
        ns->initialize();
    return ns;
}

void SetYCPVariable(const string & namespace_name, const string & variable_name, YCPValue value)
{
    Y2Namespace *ns = getNs(namespace_name.c_str());

    if (ns == NULL) {
        y2error ("Creating namespace fault.");
        return;
    }

    TableEntry *sym_te = ns->table ()->find (variable_name.c_str());

    if (sym_te == NULL) {
        y2error ("No such symbol %s::%s", namespace_name, variable_name);
        return;
    }

    SymbolEntryPtr sym_entry = sym_te->sentry();
    sym_entry->setValue(value);
}

YCPValue GetYCPVariable(const string & namespace_name, const string & variable_name)
{
    Y2Namespace *ns = getNs(namespace_name.c_str());

    if (ns == NULL) {
        y2error ("Creating namespace fault.");
        return YCPNull();
    }

    TableEntry *sym_te = ns->table ()->find (variable_name.c_str());

    if (sym_te == NULL) {
        y2error ("No such symbol %s::%s", namespace_name, variable_name);
        return YCPNull();
    }

    SymbolEntryPtr sym_entry = sym_te->sentry();
    return sym_entry->value();
}

//YCPList * ycpListFunctions;
vector<string> * ycpListFunctions;
//YCPList * ycpListVariables;
vector<string> * ycpListVariables;

static bool HandleSymbolTable (const SymbolEntry & se)
{
    if (se.isFunction ()) {
        //ycpListFunctions->add(YCPString(se.name()));
        ycpListFunctions->push_back(se.name());
    } else if (se.isVariable ()) {
        //ycpListVariables->add(YCPString(se.name()));
        ycpListVariables->push_back(se.name());
    }
    return true;
}

vector<string> module_symbols(const string & ns_name)
{
    Y2Namespace *ns = getNs(ns_name.c_str());
    //ycpListFunctions = new YCPList();
    //ycpListVariables = new YCPList();
    vector<string> funcs;
    vector<string> vars;
    ycpListFunctions = &funcs;
    ycpListVariables = &vars;

    ns->table()->forEach (&HandleSymbolTable);

    return funcs;

//    delete ycpListFunctions;
//    delete ycpListVariables;
}

PyObject *ycp_to_pyval(YCPValue val)
{
    if (val->isString())
        return PyString_FromString(val->asString()->value().c_str());
    else if (val->isInteger())
        return PyInt_FromLong(val->asInteger()->value());
    else if (val->isBoolean())
        return PyBool_FromLong(val->asBoolean()->value());
    else if (val->isVoid())
        Py_RETURN_NONE;
    else if (val->isFloat())
        return PyFloat_FromDouble(val->asFloat()->value());
    else if (val->isSymbol())
        return PyString_FromString(val->asSymbol()->symbol().c_str());
    else
        Py_RETURN_NONE;
}

YCPValue CallYCPFunction(const string & namespace_name, const string & function_name, ...)
{
    va_list args;
    va_start(args, function_name);
    YCPValue * ycpArg = NULL;
	YCPValue ycpRetValue = YCPNull ();

    // create namespace
    Y2Namespace *ns = getNs(namespace_name.c_str());

    if (ns == NULL) {
        y2error ("Creating namespace fault.");
        return YCPNull();
    }

    TableEntry *sym_te = ns->table ()->find (function_name.c_str());

    if (sym_te == NULL) {
        y2error ("No such symbol %s::%s", namespace_name, function_name);
        return YCPNull();
    }

    SymbolEntryPtr sym_entry = sym_te->sentry();
    if (sym_entry->isVariable()) {
        y2error("Cannot execute a variable");
        return YCPNull();
    }
    constFunctionTypePtr fun_type = (constFunctionTypePtr)sym_entry->type();
    Y2Function *func_call = ns->createFunctionCall (function_name, NULL);

    if (func_call == NULL) {
        y2error ("No such function %s::%s", namespace_name, function_name);
        return YCPNull();
    }

    for (int i=0; i < fun_type->parameterCount(); i++) {
        ycpArg = va_arg(args, YCPValue*);
        if (ycpArg->isNull())
            *ycpArg = YCPVoid();

        if (!func_call->appendParameter(*ycpArg)) {
            y2error ("Problem with adding arguments of function %s", function_name);
            return YCPNull();
        }
    }
    if (!func_call->finishParameters()) {
        y2error ("Problem with finishing arguments for adding arguments of function %s", function_name);
        return YCPNull();
    }

    va_end(args);

    ycpRetValue = func_call->evaluateCall();
    delete func_call;
    if (ycpRetValue.isNull()) {
        y2error ("Return value of function %s is NULL", function_name);
        return YCPNull();
    }
    return ycpRetValue;
}

static bool init_ui(const string & ui_name)
{
    Y2Component *c = YUIComponent::uiComponent ();

    if (c == 0)
    {
        y2debug ("UI component not created yet, creating %s", ui_name.c_str());
        c = Y2ComponentBroker::createServer (ui_name.c_str());

        if (c == 0)
        {
            y2error ("Cannot create component %s", ui_name.c_str());
            return false;
        }

        if (YUIComponent::uiComponent () == 0)
        {
            y2error ("Component %s is not a UI", ui_name.c_str());
            return false;
        } else {
            // got it - initialize
            c->setServerOptions (0, NULL);
        }
    } else {
        y2debug ("UI component already present: %s", c->name ().c_str ());
    }
    return true;
}

void startup_yuicomponent()
{
    YUILoader::loadUI();
    init_ui(YSettings::loadedUI());
}

void shutdown_yuicomponent()
{
    YUIComponent *c = YUIComponent::uiComponent();
    if (c)
        c->result(YCPVoid());
}

