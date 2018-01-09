// This will plot the diode channel in passing 
// and not passing events for Tuffs on and tuffs off case respecitively.
///////////////////////////////////////////////////////////////////////// 
// In order to pass the right channel from the branches used here 
// (i.e fDiodeOutputs[][]) to plot, need to find the channel number 
// from the AnitaGeomTool. Use the command 
// AnitaGeomTool::getChanIndexFromRingPhiPol(AnitaRing::kTopRing, 14, AnitaPol::kHorizontal)
// in root to get the channel number corresponding to:
// Top ring, phi sector 15, and Horizontal polarization antenna. 

#include <iostream>
using namespace std;

void plot_diode_chan_same(int chan)
{
  TCanvas *c1 = new TCanvas ("C1","c1", 800,800);
  TFile * _file0= TFile::Open("/home/keith/anitaBuildTool/components/anitaMagicDisplay/macros/12_20_17/min_bias/run3/SimulatedAnitaTruthFile3.root");
  TFile * _file1= TFile::Open("/home/keith/anitaBuildTool/components/anitaMagicDisplay/macros/12_20_17/tuffs/run3/SimulatedAnitaTruthFile3.root");
  _file0->cd();
  TTree *t_not_pass=(TTree*)_file0->Get("truthAnitaTree");
  t_not_pass->SetMarkerStyle(8);
  t_not_pass->SetMarkerSize(1);
  _file1->cd();
  TTree *t_pass=(TTree*)_file1->Get("truthAnitaTree");
  t_pass->SetMarkerStyle(8);
  t_pass->SetMarkerSize(1);
  t_pass->SetMarkerColor(2);
  TString vals;
  vals = Form("fDiodeOutput[%i][]:fTimes[%i][]",chan, chan);
  t_pass->Draw(vals,"eventNumber==600723");
  t_not_pass->Draw(vals,"eventNumber==600723","SAME");

  leg = new TLegend(0.1,0.7,0.48,0.9);
  leg->AddEntry(t_not_pass,"Not passing");
  leg->AddEntry(t_pass,"Passing");
  leg->Draw();
  c1->Draw();
  TString filename = Form("diode_output_channel_%i_event_723_both.png",chan);
  c1->SaveAs(filename);

}
