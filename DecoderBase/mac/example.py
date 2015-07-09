import sys
from ROOT import gSystem
gSystem.Load("libNevisDecoder_DecoderBase")
from ROOT import sample

try:

    print "PyROOT recognized your class %s" % str(sample)

except NameError:

    print "Failed importing DecoderBase..."

sys.exit(0)

