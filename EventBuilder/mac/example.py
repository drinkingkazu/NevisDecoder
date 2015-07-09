import sys
from ROOT import gSystem
gSystem.Load("libNevisDecoder_EventBuilder")
from ROOT import sample

try:

    print "PyROOT recognized your class %s" % str(sample)

except NameError:

    print "Failed importing EventBuilder..."

sys.exit(0)

