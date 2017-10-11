// This is the root script to plot on a TProfile the configurations from the csv files. 
#include <fstream>
#include <iostream>
#include <TProfile.h>
#include <TCanvas.h>
using namespace std;


void plot_tuff_config()
{
// string for the configuration.
//     string config_name = "config";
//     string config_name_file = arg_1;

// Declare the file to open for the tuff configuration
     ifstream config_infile;
     config_infile.open("configAdb.csv");

     TCanvas *c1= new TCanvas("testing TUFF config plots", "mutlipads", 1000, 1000);
     TProfile *config_dbr = new TProfile ("TUFF config dbr", "TUFF configuration dbr", 500, 0, 1.6e+09);
//     TProfile *config_dbi = new TProfile ("TUFF config dbi", "TUFF configuration dbi", 100, 0, 10);
//     TProfile *config_p_r = new TProfile ("TUFF config phase r", "TUFF configuration phase r", 100, 0, 10);
//     TProfile *config_p_i = new TProfile ("TUFF config phase i", "TUFF configuration phase i", 100, 0, 10);

     double freq = 0;
     double dbr = 0;
     double dbi = 0;
     double p_r = 0;
     double p_i = 0;
//     string x , y, z;
//     config_infile >> x >> y >> z; 
//     cout << "y = " << y << " z=" << z << endl;
     for (int i = 0; i < 499; i++)
     {
//          config_infile >> x;
//          cout << x << endl;
          config_infile >> freq >> dbr >> dbi;
          config_dbr->Fill(freq,dbr);
          cout << freq << " " << dbr << endl;
//          config_dbi->Fill(freq,dbi);
//          config_p_r->Fill(freq,p_r);
//          config_p_i->Fill(freq,p_i);

     }

config_infile.close();

// Draw the plots
     config_dbr->Draw();
     c1->SaveAs("test_configAdb.png");
}
