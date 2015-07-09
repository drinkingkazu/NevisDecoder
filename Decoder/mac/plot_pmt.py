import sys
from ROOT import *
gSystem.Load("libDataFormat")

fout=TFile.Open("out.root","UPDATE")

ch=TChain("pmtfifo_tree")
ch.AddFile(sys.argv[1])

if not ch.GetEntries():
    print "No event found!"
    sys.exit(1)
c=TCanvas("c","",600,500)
c.SetRightMargin(0.08)
c.SetLeftMargin(0.13)
c.SetBottomMargin(0.13)
gStyle.SetOptStat(0)

for x in xrange(ch.GetEntries()):

    ch.GetEntry(x)

    wfs = ch.pmtfifo_branch
    print "Event %d ... %d channels found!" % (wfs.event_id(), wfs.size())

    for y in xrange(wfs.size()):

        wf = wfs.at(y)

        print "  Channel: %d" % wf.channel_number()
        print "  Sample,frame:",wf.readout_sample_number_RAW(),wf.readout_frame_number()

        h=TH1D("hWF_sn","Channel %d; Sample Number; ADC" % wf.channel_number(),
               wf.size(),
               wf.readout_sample_number_RAW() - 0.5, 
               wf.readout_sample_number_RAW() - 0.5 + wf.size())
        for z in xrange(wf.size()):
            h.SetBinContent(z+1,wf.at(z))

        c.cd()
        h.Draw()
        c.Update()
        cmd=sys.stdin.readline()
        if cmd == "s\n":
            fout.cd()
            h.Write()
            fout.Close()
            sys.exit(1)
        del h
    print
    print "done with event %d..." % wfs.event_id()
    print
