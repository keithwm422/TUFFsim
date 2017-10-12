// From Andrew's macro, the input of the convolution of the A3 response (from Rotter) with the TUFF responses (from Oindree)
// is input as a AnalysisWavefrom, which will then be convoluted again with the icemc simulated signal to simulate a neutrino event with TUFFs. 
//-Keith McBride 10/11/17


// edits from Andrew's macro:
/*
Andrew had to convolve each response from A3 with the TUFF config per antenna. 
I have to edit this to then convolute the simulated signal with the appropriate response.
Need to know then:
1) Will the simulated signal be antenna independent in icemc? Probably
	if so, then which antenna input response is used? or is it an average? weighted evenly?

I can still write the code to grab an input from the directory of the config called for my program. 

To-do 10/12/17
	a) try a constant signal waveform (somehow?, maybe from a input file i make) 
	convoluted with one of the impulse responses from Andrew's macro (in the same dir as index.txt).

*/
//First test is to load the .imp from andrew's macros output to a waveform:
// path is /data/responses/TUFFs/notches_250_375_0 hwoever this needs to change once the macro is moved to icemc?
#include "TFile.h"
#include "TMath.h"
#include "TString.h"
#include "TGraph.h"
#include "AnalysisWaveform.h"
#include "FFTtools.h"
#include <stdio.h>
#include <sys/stat.h>

void convolveTUFF_and_sim(int ant=1, char tmb='B', char config='B', const char* outdir="")
{
        //keith added file outdir declared here for testing
        outdir=notches_250_375_0
//	mkdir(Form("data/responses/TUFFs/%s", outdir), 0777); from andrew code
	TString impnameH;
	TString impnameV;
	TString outNameH;
	TString outNameV;
	if(ant < 10)
	{
// from andrew code: impnameH = Form("%s/share/AnitaAnalysisFramework/responses/IndividualBRotter/0%d%cH.imp", getenv("ANITA_UTIL_INSTALL_DIR"), ant, tmb);
//		impnameV = Form("%s/share/AnitaAnalysisFramework/responses/IndividualBRotter/0%d%cV.imp", getenv("ANITA_UTIL_INSTALL_DIR"), ant, tmb);
		impnameH = Form("/home/keith/anitaBuildTool/components/AnitaAnalysisFramework/data/responses/TUFFs/%s/0%d%cH.imp", outdir, ant, tmb);
		impnameV = Form("/home/keith/anitaBuildTool/components/AnitaAnalysisFramework/data/responses/TUFFs/%s/0%d%cV.imp", outdir, ant, tmb);
		outNameH = Form("data/responses/TUFFs/%s/0%d%cH_test.imp", outdir, ant, tmb);
		outNameV = Form("data/responses/TUFFs/%s/0%d%cV_test.imp", outdir, ant, tmb);
	} 
	else
	{
//		impnameH = Form("%s/share/AnitaAnalysisFramework/responses/IndividualBRotter/%d%cH.imp", getenv("ANITA_UTIL_INSTALL_DIR"), ant, tmb);
//		impnameV = Form("%s/share/AnitaAnalysisFramework/responses/IndividualBRotter/%d%cV.imp", getenv("ANITA_UTIL_INSTALL_DIR"), ant, tmb);
		impnameH = Form("/home/keith/anitaBuildTool/components/AnitaAnalysisFramework/data/responses/TUFFs/%s/%d%cH.imp", outdir, ant, tmb);
		impnameV = Form("/home/keith/anitaBuildTool/components/AnitaAnalysisFramework/data/responses/TUFFs/%s/%d%cV.imp", outdir, ant, tmb);
		outNameH = Form("data/responses/TUFFs/%s/%d%cH_test.imp", outdir, ant, tmb);
		outNameV = Form("data/responses/TUFFs/%s/%d%cV_test.imp", outdir, ant, tmb);
	}

	TGraph tdH(impnameH);
	TGraph tdV(impnameV);
// Load the TUFF config into a waveform, for H and V seperately.
	AnalysisWaveform awfH(tdH.GetN(), tdH.GetY(), tdH.GetX()[1] - tdH.GetX()[0], 0);
	(void) awfH.freq();
	double dfH = awfH.deltaF();
	int NfreqH = awfH.Nfreq();
	FFTWComplex* freqH = awfH.updateFreq();
	AnalysisWaveform awfV(tdV.GetN(), tdV.GetY(), tdV.GetX()[1] - tdV.GetX()[0], 0);
	(void) awfV.freq();
	FFTWComplex* freqV = awfV.updateFreq();

//	TFile convfile(Form("%s/share/AnitaAnalysisFramework/tuffModels/config%c.root", getenv("ANITA_UTIL_INSTALL_DIR"), config));
// file from icemc that has trigger impulse response
	TFile convfile(Form("/home/keith/icemc/data/sumPicoImpulse.root"));
// Need to put"trigger impulse response" from icemc as the TGraph's here
	TGraph* gReal = (TGraph*) convfile.Get("grImpRespH");
	TGraph* gImag = (TGraph*) convfile.Get("gImag");

// This stays for the trigger imuplse response.
	double phaseShift = TMath::ATan2(gImag->Eval(0), gReal->Eval(0));

	//convolve w tuff response
	for(int i = 0; i < NfreqH; i++)
	{
		if(dfH*i > 1.5) continue;
		double reTemp = gReal->Eval(dfH*i);
		double imTemp = gImag->Eval(dfH*i);
		FFTWComplex temp(reTemp,imTemp);
		temp.setMagPhase(temp.getAbs(), temp.getPhase() - phaseShift);
		freqH[i] *= temp;
		freqV[i] *= temp;
	}
	
	TGraph* gOutH = new TGraph(awfH.even()->GetN(), awfH.even()->GetX(), awfH.even()->GetY());
	TGraph* gOutV = new TGraph(awfV.even()->GetN(), awfV.even()->GetX(), awfV.even()->GetY());
	
//
	ofstream outH(outNameH.Data());
	ofstream outV(outNameV.Data());

	for(int i = 0; i < gOutH->GetN(); i++)
	{
		if(.1*i < 10)
		{
			outH << to_string(double(.1*i)).substr(0,3) << " " << gOutH->GetY()[i] << "\n";
			outV << to_string(double(.1*i)).substr(0,3) << " " << gOutV->GetY()[i] << "\n";
			continue;
		}
		if(.1*i < 100)
		{
			outH << to_string(double(.1*i)).substr(0,4) << " " << gOutH->GetY()[i] << "\n";
			outV << to_string(double(.1*i)).substr(0,4) << " " << gOutV->GetY()[i] << "\n";
			continue;
		}
		outH << to_string(double(.1*i)).substr(0,5) << " " << gOutH->GetY()[i] << "\n";
		outV << to_string(double(.1*i)).substr(0,5) << " " << gOutV->GetY()[i] << "\n";
	}

	outH.close();
	outV.close();
}


void convolveTUFFS_and_sim()
{
	for(int i = 1; i < 17; i++)
	{
		convolveTUFF(i, 'B', 'B', "notches_260_375_0");
		convolveTUFF(i, 'M', 'B', "notches_260_375_0");
		convolveTUFF(i, 'T', 'B', "notches_260_375_0");
	}
	for(int i = 1; i < 17; i++)
	{
		convolveTUFF(i, 'B', 'J', "notches_250_375_0");
		convolveTUFF(i, 'M', 'J', "notches_250_375_0");
		convolveTUFF(i, 'T', 'J', "notches_250_375_0");
	}
	for(int i = 1; i < 17; i++)
	{
		convolveTUFF(i, 'B', 'C', "notches_260_0_460");
		convolveTUFF(i, 'M', 'C', "notches_260_0_460");
		convolveTUFF(i, 'T', 'C', "notches_260_0_460");
	}
	for(int i = 1; i < 17; i++)
	{
		convolveTUFF(i, 'B', 'A', "notches_260_0_0");
		convolveTUFF(i, 'M', 'A', "notches_260_0_0");
		convolveTUFF(i, 'T', 'A', "notches_260_0_0");
	}
	for(int i = 1; i < 17; i++)
	{
		convolveTUFF(i, 'B', 'O', "notches_260_365_0");
		convolveTUFF(i, 'M', 'O', "notches_260_365_0");
		convolveTUFF(i, 'T', 'O', "notches_260_365_0");
	}
	for(int i = 1; i < 17; i++)
	{
		convolveTUFF(i, 'B', 'G', "notches_260_385_0");
		convolveTUFF(i, 'M', 'G', "notches_260_385_0");
		convolveTUFF(i, 'T', 'G', "notches_260_385_0");
	}
	for(int i = 1; i < 17; i++)
	{
		convolveTUFF(i, 'B', 'P', "notches_260_375_460");
		convolveTUFF(i, 'M', 'P', "notches_260_375_460");
		convolveTUFF(i, 'T', 'P', "notches_260_375_460");
	}
}
