from ycp2 import YCPSymbol as Symbol
from ycp2 import YCPList as List
from ycp2 import YCPString as String
from ycp2 import YCPTerm as Term
from ycp2 import YCPInteger as Integer
from ycp2 import YCPBoolean as Boolean
from ycp2 import YCPFloat as Float
from ycp2 import ycp_to_pyval as c_ycp_to_pyval

def pytval_to_ycp(item):
    if type(item) is list:
        sl = List()
        for si in item:
            sl.push_back(pytval_to_ycp(si))
        return sl
    elif type(item) is str:
        return String(item)
    elif type(item) is int:
        return Integer(item)
    elif type(item) is bool:
        return Boolean(item)
    elif type(item) is float:
        return Float(item)
    elif type(item) in [Term, Symbol, String, Integer, Boolean, Float, List]:
        return item
    else:
        raise SyntaxError, 'Type of value "%s" unrecognized, %s' % (item, str(type(item)))

def ycp_to_pyval(item):
    val = c_ycp_to_pyval(item)
    return val if val is not None else item

