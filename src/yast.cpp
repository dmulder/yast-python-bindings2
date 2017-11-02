#include "yast.h"

YCPList * ycpListFunctions;
YCPList * ycpListVariables;

YCPTerm Id(string id)
{
    auto l = YCPList();
    l.push_back(YCPSymbol(id));
    return YCPTerm("id", l);
}

YCPTerm Opt(char * opt, ...)
{
    va_list args;
    va_start(args, opt);
    auto l = YCPList();
    char * tmp = NULL;
    l.push_back(YCPSymbol(opt));

    for (int i = 0; i < 25; i++) {
        tmp = va_arg(args, char*);
        if (tmp != NULL)
            l.push_back(YCPSymbol(tmp));
    }

    va_end(args);

    return YCPTerm("opt", l);
}

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

/**
 * This is needed for importing new module from ycp.
 */
static PyMethodDef new_module_methods[] =
{
    {NULL, NULL, 0, NULL}
};

/**
 * Register functions and variables from namespace to python module
 * @param char *NameSpace - name of namespace
 * @param YCPList list_functions - names of functions
 * @param YCPList list_variables - names of variables
 * @return true on success
 */
static bool RegFunctions(const string & NameSpace, YCPList list_functions, YCPList list_variables)
{
    // Init new module with name NameSpace and method __run (see new_module_methods)
    PyObject *new_module = Py_InitModule(NameSpace.c_str(), new_module_methods);
    if (new_module == NULL) return false;

    // Dictionary of new_module - there will be registered all functions
    PyObject *new_module_dict = PyModule_GetDict(new_module);
    if (new_module_dict == NULL) return false;

    PyObject *code;
    auto g = PyDict_New();
    if (!g)
        return NULL;
    PyDict_SetItemString(g, "__builtins__", PyEval_GetBuiltins());

    // register functions from ycp to python module 
    for (int i=0; i<list_functions.size();i++) {
        string function = list_functions->value(i)->asString()->value();
        stringstream func_def;
        func_def << "def " << function << "(*args):" << endl;
        func_def << "\tfrom ycp2 import CallYCPFunction" << endl;
        func_def << "\tfrom ytypes import pytval_to_ycp" << endl;
        func_def << "\treturn CallYCPFunction(\"" + string(NameSpace) + "\", \"" + function + "\", pytval_to_ycp(list(args)))" << endl;

        // Register function into dictionary of new module. Returns new reference - must be decremented
        code = PyRun_String(func_def.str().c_str(), Py_single_input, g, new_module_dict);
        Py_XDECREF(code);
    }

    // adding variables like function from ycp to module
    for (int i=0; i<list_variables.size();i++) {
        string function = list_variables->value(i)->asString()->value();
        stringstream func_def;
        func_def << "def " << function << "(val=None):" << endl;
        func_def << "\tfrom ycp2 import GetYCPVariable, SetYCPVariable" << endl;
        func_def << "\tif val:" << endl;
        func_def << "\t\tSetYCPVariable(\"" + string(NameSpace) + "\", \"" + function + "\", pytval_to_ycp(val))" << endl;
        func_def << "\telse:" << endl;
        func_def << "\t\treturn GetYCPVariable(\"" + string(NameSpace) + "\", \"" + function + "\")" << endl;

        // Register function into dictionary of new module. Returns new reference - must be decremented
        code = PyRun_String(func_def.str().c_str(), Py_single_input, g, new_module_dict);
        Py_XDECREF(code);
    }

    return true;
}

/**
 * Function check SymbolEntry and add name
 * to ycpListFunctions if it is function or
 * add it to ycpListVariables if it is variable
 * @param const SymbolEntry for analyse
 * @return bool always return true
 */
static bool HandleSymbolTable (const SymbolEntry & se)
{
    if (se.isFunction ()) {
        ycpListFunctions->add(YCPString(se.name()));
    } else if (se.isVariable ()) {
        ycpListVariables->add(YCPString(se.name()));
    }
    return true;
}

/**
 * Function import module written in YCP.
 * It means that create module into namespace of python module ycp
 * @param PyObject *args - string - include name of module written in YCP 
 * @return PyObject * true on success
 */
bool import_module(const string & ns_name)
{
    Y2Namespace *ns = getNs(ns_name.c_str());
    ycpListFunctions = new YCPList();
    ycpListVariables = new YCPList();

    ns->table()->forEach (&HandleSymbolTable);
    RegFunctions(ns_name, *ycpListFunctions, *ycpListVariables);

    delete ycpListFunctions;
    delete ycpListVariables;

    return true;
}

/**
 * Function handles calling ycp function from python
 * @param const string & namespace
 * @param const string & name of function
 * @param ... args for function
 * @return YCPValue return result of running function
 */
YCPValue CallYCPFunction(const char * namespace_name, const char * function_name, YCPList args)
{
    YCPValue ycpArg = YCPNull ();
	YCPValue ycpRetValue = YCPNull ();


    // create namespace
    Y2Namespace *ns = getNs(namespace_name);

    if (ns == NULL) {
        y2error ("Creating namespace fault.");
        return YCPNull();
    }

    TableEntry *sym_te = ns->table ()->find (function_name);

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

    for (int i = 0; i < args.size(); i++) {
        ycpArg = args->value(i);

        if (fun_type->parameterType(i)->isSymbol() && ycpArg->isString()) {
            ycpArg = YCPSymbol(ycpArg->asString()->value());
        }
        if (ycpArg.isNull())
            ycpArg = YCPVoid();

        if (!func_call->appendParameter(ycpArg)) {
            y2error ("Problem with adding arguments of function %s", function_name);
            return YCPNull();
        }
    }
    if (!func_call->finishParameters()) {
        y2error ("Problem with finishing arguments for adding arguments of function %s", function_name);
        return YCPNull();
    }


    ycpRetValue = func_call->evaluateCall();
    delete func_call;
    if (ycpRetValue.isNull()) {
        y2error ("Return value of function %s is NULL", function_name);
        return YCPNull();
    }
    return ycpRetValue;
}

void startup_yuicomponent()
{
    Y2Component *c = YUIComponent::uiComponent ();

    if (c == 0)
    {
        YUILoader::loadUI();
        const string & ui_name = YSettings::loadedUI();

        y2debug ("UI component not created yet, creating %s", ui_name.c_str());

        c = Y2ComponentBroker::createServer (ui_name.c_str());

        if (c == 0)
        {
            y2error ("Cannot create component %s", ui_name.c_str());
            return;
        }

        if (YUIComponent::uiComponent () == 0)
        {
            y2error ("Component %s is not a UI", ui_name.c_str());
            return;
        } else {
            // got it - initialize
            c->setServerOptions (0, NULL);
        }
    } else {
        y2debug ("UI component already present: %s", c->name ().c_str ());
    }
}

void shutdown_yuicomponent()
{
    YUIComponent *c = YUIComponent::uiComponent();
    if (c)
        c->result(YCPVoid());
}

