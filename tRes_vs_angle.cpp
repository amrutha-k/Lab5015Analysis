#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <time.h>
#include <stdio.h>
#include <sys/stat.h>

#include "TFile.h"
#include "TChain.h"
#include "TH1F.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TLine.h"
#include "TRandom3.h"
#include "TLegend.h"
#include "TError.h"

void tRes_vs_angle()
{
  gStyle -> SetOptStat(0);
  gStyle -> SetOptFit(0);
  gErrorIgnoreLevel = kWarning; // to suppress Info messages 

  std::string plotDir = "/eos/user/a/amkrishn/www/BTL-MTDTB_FNAL_Feb2020/conf10/tRes_vs_angle";
  system(Form("mkdir -p %s",plotDir.c_str()));

  TFile* inFile = TFile::Open("fitfuncs_deltaT_allAngles.root","READ");

  std::vector<std::string> VovLabels;
  std::vector<std::string> thLabels;
  std::vector<std::string> angleLabels;
  std::vector<std::string> chPairs;

  TList* list = inFile -> GetListOfKeys();
  TIter next(list);
  TObject* object = 0;
  while( (object = next()) )
    {
      std::string name(object->GetName());
      std::size_t found = name.find("f1_deltaT_energyCorr");
      if( found!=std::string::npos )
	{
	  std::stringstream ss(name);
	  std::string token;
	  std::vector<std::string> tokens;
	  while( std::getline(ss, token, '_') )
	    {
	      tokens.push_back(token);
	    }
	  std::string VovLabel = tokens[4];
	  VovLabels.push_back(VovLabel);

	  std::string thLabel = tokens[5];
          thLabels.push_back(thLabel);

	  std::string angleLabel = tokens[6];
          angleLabels.push_back(angleLabel);

	  std::string chPair = tokens[3];
          chPairs.push_back(chPair);
	  
	}
    }
  std::sort(VovLabels.begin(),VovLabels.end());
  VovLabels.erase(std::unique(VovLabels.begin(),VovLabels.end()),VovLabels.end());

  std::sort(thLabels.begin(),thLabels.end());
  thLabels.erase(std::unique(thLabels.begin(),thLabels.end()),thLabels.end());

  std::sort(angleLabels.begin(),angleLabels.end());
  angleLabels.erase(std::unique(angleLabels.begin(),angleLabels.end()),angleLabels.end());

  std::sort(chPairs.begin(),chPairs.end());
  chPairs.erase(std::unique(chPairs.begin(),chPairs.end()),chPairs.end());

  TCanvas* c;
  TLatex* latex;
  TLegend* legend;
  TF1* func;

  int angle;
  //float angleErr;
  std::map<std::string,TGraphErrors*> g_tRes_energyCorr_bestTh_vs_angle;
  std::map<std::string,TGraphErrors*> g_tRes_raw_bestTh_vs_angle;
  std::map<std::string,TGraphErrors*> g_tRes_energyPosCorr_bestTh_vs_angle;
  std::map<std::string,std::map<int,float> > tRes_energyCorr_bestTh;
  std::map<std::string,std::map<int,double> > tResErr_energyCorr_bestTh;
  std::map<std::string,std::map<int,float> > tRes_raw_bestTh;
  std::map<std::string,std::map<int,double> > tResErr_raw_bestTh;
  std::map<std::string,std::map<int,float> > tRes_energyPosCorr_bestTh;
  std::map<std::string,std::map<int,double> > tResErr_energyPosCorr_bestTh;
  std::map<int,double> angleErr;

  for(auto VovLabel : VovLabels)
    {
      for(auto chPair : chPairs)
	{
	  for(auto angleLabel : angleLabels)
	    {
	      std::string string_angle = angleLabel;
	      if (string_angle == "0deg")
		{
		  string_angle.erase(1,3);
		  angle = atoi(string_angle.c_str());
		  angleErr[angle]=5; 
		}
	      else
		{
		  string_angle.erase(2,4);
                  angle = atoi(string_angle.c_str());
		  if(angle == 10 || angle == 20) { angleErr[angle] = 5; }
                  if(angle == 30 || angle == 40) { angleErr[angle] = 2.5; }
                  if(angle == 60 || angle == 70 || angle == 80) { angleErr[angle]= 1; }
		}
	      
	      for(auto thLabel : thLabels)
		{
		  
		  std::string label12 = Form("%s_%s_%s_%s",chPair.c_str(),VovLabel.c_str(),thLabel.c_str(),angleLabel.c_str());
		  
		  func = (TF1*)( inFile->Get(Form("f1_deltaT_energyCorr_%s",label12.c_str())) );
		  if(func == NULL) continue;

		  std::string label(Form("%s_%s_bestTh",chPair.c_str(),VovLabel.c_str()));
		  if( g_tRes_energyCorr_bestTh_vs_angle[label] == NULL )
		    {
		      g_tRes_energyCorr_bestTh_vs_angle[label] = new TGraphErrors();
		     
		    }
		  if( ( tRes_energyCorr_bestTh[label][angle] != 0. && func->GetParameter(2) < tRes_energyCorr_bestTh[label][angle] ) ||
		      tRes_energyCorr_bestTh[label][angle] == 0 )
		    {
		      tRes_energyCorr_bestTh[label][angle] = func->GetParameter(2);
		      tResErr_energyCorr_bestTh[label][angle] = func->GetParError(2);

		    }



		  //------------ raw deltaT

		  func = (TF1*)( inFile->Get(Form("f1_deltaT_raw_%s",label12.c_str())) );
                  //if(func == NULL) continue;

		  if( g_tRes_raw_bestTh_vs_angle[label] == NULL )
                    {
                      g_tRes_raw_bestTh_vs_angle[label] = new TGraphErrors();
		      
		    }   
                  if( ( tRes_raw_bestTh[label][angle] != 0. && func->GetParameter(2) < tRes_raw_bestTh[label][angle] ) ||
                      tRes_raw_bestTh[label][angle] == 0 )
                    {
                      tRes_raw_bestTh[label][angle] = func->GetParameter(2);
                      tResErr_raw_bestTh[label][angle] = func->GetParError(2);

                    }
		
		  
		  //------------ energy-position corrected deltaT                                                                                                                                                                 

                  func = (TF1*)( inFile->Get(Form("f1_deltaT_energyPositionCorr_%s",label12.c_str())) );
                  //if(func == NULL) continue;

                  if( g_tRes_energyPosCorr_bestTh_vs_angle[label] == NULL )
                    {
                      g_tRes_energyPosCorr_bestTh_vs_angle[label] = new TGraphErrors();

                    }
                  if( ( tRes_energyPosCorr_bestTh[label][angle] != 0. && func->GetParameter(2) < tRes_energyPosCorr_bestTh[label][angle] ) ||
                      tRes_energyPosCorr_bestTh[label][angle] == 0 )
                    {
                      tRes_energyPosCorr_bestTh[label][angle] = func->GetParameter(2);
                      tResErr_energyPosCorr_bestTh[label][angle] = func->GetParError(2);
                    }
		
		}
	    }
	}
    }
  

  for(auto VovLabel : VovLabels)
    {
      for(auto chPair : chPairs)
        {
	  std::string label(Form("%s_%s_bestTh",chPair.c_str(),VovLabel.c_str()));

	  for(auto mapIt : tRes_energyCorr_bestTh[label])
	    {
	      g_tRes_raw_bestTh_vs_angle[label] -> SetPoint(g_tRes_raw_bestTh_vs_angle[label]->GetN(),mapIt.first,tRes_raw_bestTh[label][mapIt.first]);
	      g_tRes_raw_bestTh_vs_angle[label] -> SetPointError(g_tRes_raw_bestTh_vs_angle[label]->GetN()-1,angleErr[mapIt.first],tResErr_raw_bestTh[label][mapIt.first]);

	      g_tRes_energyCorr_bestTh_vs_angle[label] -> SetPoint(g_tRes_energyCorr_bestTh_vs_angle[label]->GetN(),mapIt.first,tRes_energyCorr_bestTh[label][mapIt.first]);
	      g_tRes_energyCorr_bestTh_vs_angle[label] -> SetPointError(g_tRes_energyCorr_bestTh_vs_angle[label]->GetN()-1,angleErr[mapIt.first],tResErr_energyCorr_bestTh[label][mapIt.first]);

	      g_tRes_energyPosCorr_bestTh_vs_angle[label] -> SetPoint(g_tRes_energyPosCorr_bestTh_vs_angle[label]->GetN(),mapIt.first,tRes_energyPosCorr_bestTh[label][mapIt.first]);
	      g_tRes_energyPosCorr_bestTh_vs_angle[label] -> SetPointError(g_tRes_energyPosCorr_bestTh_vs_angle[label]->GetN()-1,angleErr[mapIt.first],tResErr_energyPosCorr_bestTh[label][mapIt.first]);
	    }
	}
    }

  //fitting -------
  for(auto VovLabel : VovLabels)
    {
      for(auto chPair : chPairs)
        {

	  std::string label(Form("%s_%s_bestTh",chPair.c_str(),VovLabel.c_str()));
	  if(label.find("pixel3x3-pixel2x2")!=std::string::npos) continue;
	  TF1* fitfunc = new TF1(Form("fitfunc_energyPosCorr_%s",label.c_str()),"sqrt([0]*[0]+[1]*[1]*pow(cos(x*TMath::Pi()/180.),[2])*pow(cos(x*TMath::Pi()/180.),[2]))",0,80);
	  fitfunc -> SetParLimits(0,0,85);
	  fitfunc -> SetParameter(1,1);
	  fitfunc -> SetParLimits(2,0.4,0.6);
	  g_tRes_energyPosCorr_bestTh_vs_angle[label] -> Fit(fitfunc,"QR","");
	}
    }

  //---------- plot

  for(auto VovLabel : VovLabels)
    {
      for(auto chPair : chPairs)
	{
	  c = new TCanvas(Form("c_tRes_bestTh_vs_angle_%s_%s",chPair.c_str(),VovLabel.c_str()),Form("c_tRes_bestTh_vs_angle_%s_%s",chPair.c_str(),VovLabel.c_str()));

	  legend = new TLegend(0.60,0.75,0.85,0.88);
	  legend -> SetFillColor(0);
	  legend -> SetFillStyle(1000);
	  legend -> SetTextFont(42);

	  TH1F* hPad = (TH1F*)( gPad->DrawFrame(-10,30,90,300) );
	  hPad -> SetTitle(";angle [degree];#sigma_{t_{diff}} [ps]");

	  std::string label(Form("%s_%s_bestTh",chPair.c_str(),VovLabel.c_str()));
	  //g_tRes_raw_bestTh_vs_angle[label] -> SetLineColor(1);
	  g_tRes_raw_bestTh_vs_angle[label] -> SetMarkerColor(1);
	  g_tRes_raw_bestTh_vs_angle[label] -> SetMarkerStyle(25);
	  g_tRes_raw_bestTh_vs_angle[label] -> Draw("P,same"); 

	  g_tRes_energyCorr_bestTh_vs_angle[label] -> SetLineColor(1);
	  g_tRes_energyCorr_bestTh_vs_angle[label] -> SetMarkerColor(1);
	  g_tRes_energyCorr_bestTh_vs_angle[label] -> SetMarkerStyle(26);
	  g_tRes_energyCorr_bestTh_vs_angle[label] -> Draw("P,same");

	  g_tRes_energyPosCorr_bestTh_vs_angle[label] -> SetLineColor(1);
          g_tRes_energyPosCorr_bestTh_vs_angle[label] -> SetMarkerColor(1);
          g_tRes_energyPosCorr_bestTh_vs_angle[label] -> SetMarkerStyle(20);
          g_tRes_energyPosCorr_bestTh_vs_angle[label] -> Draw("P,same");

       	  latex = new TLatex(0.65,0.70,Form("%s_best th.",VovLabel.c_str()));
	  latex -> SetNDC();
	  latex -> SetTextFont(42);
	  latex -> SetTextSize(0.04);
	  latex -> SetTextColor(kBlack);
	  latex -> Draw("same");
	  
	  legend->AddEntry(g_tRes_raw_bestTh_vs_angle[label],"raw","P");
	  legend->AddEntry(g_tRes_energyCorr_bestTh_vs_angle[label],"energy corrected","P");
          legend->AddEntry(g_tRes_energyPosCorr_bestTh_vs_angle[label],"energy-position corrected","P");

	  legend->Draw("same"); 

	  c -> Print(Form("%s/c_tRes_bestTh_vs_angle__%s_%s.png",plotDir.c_str(),chPair.c_str(),VovLabel.c_str()));
	  c -> Print(Form("%s/c_tRes_bestTh_vs_angle__%s_%s.pdf",plotDir.c_str(),chPair.c_str(),VovLabel.c_str()));
	  delete c;
	}
    }

}
