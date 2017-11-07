BUILT_SOURCES = ycp2.py
ycp2.py yast-core_wrap.cxx: yast-core.i ytypes.i $(libpy2UI)
	$(SWIG) $(AX_SWIG_PYTHON_OPT) -c++ -I/usr/include/YaST2 -o $@ $<

python_PYTHON = ycp2.py yast.py

pyexec_LTLIBRARIES = _ycp2.la

_ycp2_la_SOURCES = yast-core_wrap.cxx yast.cpp yast-core.i ytypes.i

_ycp2_la_LDFLAGS = -module ${PYTHON_LDFLAGS} -Wl,-rpath=$(py2langdir)

_ycp2_la_LIBADD = -L$(py2langdir) -lpy2UI

_ycp2_la_CPPFLAGS = -std=c++11 -I/usr/include/YaST2 ${PYTHON_CPPFLAGS}

py2lang_LTLIBRARIES = libpy2lang_python.la

libpy2lang_python_la_SOURCES = Y2PythonClientComponent.cc Y2CCPythonClient.cc

libpy2lang_python_la_LDFLAGS = -lpython2.7 ${PYTHON_LDFLAGS} -Wl,-rpath=$(pyexecdir)

libpy2lang_python_la_CPPFLAGS = -std=c++11 -I/usr/include/YaST2 ${PYTHON_CPPFLAGS}

AM_CXXFLAGS = -DY2LOG=\"Python\"

CLEANFILES = ycp2.py yast-core_wrap.cxx
