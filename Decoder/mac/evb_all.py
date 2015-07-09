import sys,os
from ROOT import *
gSystem.Load("libEventBuilder")
from ROOT import larlight as fmwk

evb = fmwk.event_builder()

evb.add_input(fmwk.DATA.Trigger,sys.argv[1])
evb.add_input(fmwk.DATA.PMTFIFO,sys.argv[2])
evb.add_input(fmwk.DATA.TPCFIFO,sys.argv[3])
evb.set_master_stream(fmwk.DATA.Trigger)


out_dir = os.environ['PWD']
if sys.argv[1].find("/")>=0:
    out_dir = sys.argv[1][0:sys.argv[1].rfind("/")+1]

evb.set_ana_output_filename("%s/ana_evb.root" % out_dir)

evb.set_output_filename("%s/built.root" % out_dir)

#evb.set_verbosity(fmwk.MSG.DEBUG)
#evb.set_verbosity(fmwk.MSG.INFO)
evb.set_verbosity(fmwk.MSG.NORMAL)

evb.debug_mode(True)

evb.run()
