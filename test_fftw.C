#include "TFile.h"
#include "TMath.h"
#include "TString.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "AnalysisWaveform.h"
#include "FFTtools.h"
#include <stdio.h>
#include <sys/stat.h>

void test_fftw()
{
     TCanvas *c1 = new TCanvas("testing fftw","multipads", 1000,1000);
     TGraph tdH("/home/keith/anitaBuildTool/Utils/share/AnitaAnalysisFramework/responses/IndividualBRotter/01BH.imp");
     AnalysisWaveform awfH(tdH.GetN(), tdH.GetY(), tdH.GetX()[1] - tdH.GetX()[0], 0);
     AnalysisWaveform awfH_1(tdH.GetN(), tdH.GetY(), tdH.GetX()[1] - tdH.GetX()[0], 0);

     (void) awfH.freq();
     double dfH = awfH.deltaF();
     int NfreqH = awfH.Nfreq();


     FFTWComplex* freqH = awfH.updateFreq();
     TGraph* gOutH_1 = new TGraph(awfH_1.even()->GetN(), awfH_1.even()->GetX(), awfH_1.even()->GetY());
//     gOutH->Draw();

     TFile convfile(Form("/home/keith/anitaBuildTool/Utils/share/AnitaAnalysisFramework/tuffModels/configA.root"));
     TGraph* gReal = (TGraph*) convfile.Get("gReal");
     TGraph* gImag = (TGraph*) convfile.Get("gImag");

     double phaseShift = TMath::ATan2(gImag->Eval(0), gReal->Eval(0));

     for(int i = 0; i < NfreqH; i++)
        {
                if(dfH*i > 1.5) continue;
                double reTemp = gReal->Eval(dfH*i);
                double imTemp = gImag->Eval(dfH*i);
                FFTWComplex temp(reTemp,imTemp);
                temp.setMagPhase(temp.getAbs(), temp.getPhase() - phaseShift);
                freqH[i] *= temp;
        }


     TGraph* gOutH = new TGraph(awfH.even()->GetN(), awfH.even()->GetX(), awfH.even()->GetY());
     gOutH->Draw();
//     c1->SaveAs("test_fftw_changed.png");
     gOutH_1->Draw("same");
     c1->SaveAs("test_fftw_original.png");





}
