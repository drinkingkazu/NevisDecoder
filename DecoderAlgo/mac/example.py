import sys
from ROOT import gSystem
gSystem.Load("libNevisDecoder_DecoderAlgo")
from ROOT import sample

try:

    print "PyROOT recognized your class %s" % str(sample)

except NameError:

    print "Failed importing DecoderAlgo..."

sys.exit(0)

