import sys
from ROOT import gSystem
gSystem.Load("libNevisDecoder_FileIO")
from ROOT import sample

try:

    print "PyROOT recognized your class %s" % str(sample)

except NameError:

    print "Failed importing FileIO..."

sys.exit(0)

