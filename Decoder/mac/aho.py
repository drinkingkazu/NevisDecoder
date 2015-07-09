import ROOT,sys
ROOT.gSystem.Load("libDataFormat")
from ROOT import *


ch=TChain("pmtfifo_tree")
ch.AddFile(sys.argv[1])
c=TCanvas("c","",600,500)

gFrameDiffEventTrig=TGraph(ch.GetEntries())
hFrameDiffEventDisc=TH1D("hFrameDiffEventDisc","",15,-7.5,7.5)

for x in xrange(ch.GetEntries()):

    ch.GetEntry(x)

    wfs=ch.pmtfifo_branch

    for y in xrange(wfs.size()):
        
        hFrameDiffEventDisc.Fill(wfs.at(y).readout_frame_number() - wfs.event_frame_number())

    gFrameDiffEventTrig.SetPoint(x,x,wfs.event_frame_number() - wfs.fem_trig_frame_number())

gFrameDiffEventTrig.SetMarkerStyle(20)
gFrameDiffEventTrig.Draw("AP")
c.Update()
sys.stdin.readline()

hFrameDiffEventDisc.Draw()
c.Update()
sys.stdin.readline()    

