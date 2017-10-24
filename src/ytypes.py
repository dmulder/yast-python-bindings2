from ycp2 import YCPSymbol as Symbol
from ycp2 import YCPList as List
from ycp2 import YCPString as String
from ycp2 import YCPTerm as Term
from ycp2 import YCPInteger as Integer
from ycp2 import YCPBoolean as Boolean
from ycp2 import YCPFloat as Float

def ytype(item):
    if type(item) is list:
        sl = List()
        for si in item:
            sl.push_back(ytype(si))
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

