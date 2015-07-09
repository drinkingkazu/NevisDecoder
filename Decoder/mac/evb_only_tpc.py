import sys
from ROOT import *
gSystem.Load("libEventBuilder")
from ROOT import larlight as fmwk

evb = fmwk.event_builder()

for x in xrange(len(sys.argv)-1):
    evb.add_input(fmwk.DATA.TPCFIFO,sys.argv[x+1])

evb.set_master_stream(fmwk.DATA.TPCFIFO)

evb.set_output_filename("evb_out.root")

#evb.set_verbosity(fmwk.MSG.DEBUG)
#evb.set_verbosity(fmwk.MSG.INFO)
evb.set_verbosity(fmwk.MSG.NORMAL)

evb.run()
