#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <TObject.h>
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH1D.h"
#include "TMath.h"
#include "TTreeFormula.h"

float deltaPhi(float iPhi1,float iPhi2) { 
  float lDPhi = fabs(iPhi1-iPhi2); 
  if(lDPhi > 2*TMath::Pi()-lDPhi) lDPhi = 2*TMath::Pi()-lDPhi;
  return lDPhi;
}
float deltaR  (float iEta1,float iPhi1,float iEta2,float iPhi2) { 
  float lDEta = fabs(iEta1-iEta2);
  float lDPhi = deltaPhi(iPhi1,iPhi2);
  return sqrt(lDEta*lDEta+lDPhi*lDPhi);
}

float SF(float x){
  return TMath::Exp(0.159 -0.00141*x);
}
float topWeight(float pt1,float pt2){
  return TMath::Sqrt(SF(pt1)*SF(pt2));
}

void skim(TString configFileName = "trainingFiles.txt",TString outDir = "scratch/",TString Version = "boostedv-v9",int SelectionOpt = 0){

  // Default Selection is for MET samples
  // Selection = 1 is the ttbar selection

  std::vector<TString> sampleName;
  std::vector<TString> SigBack;
  std::vector<TString> inFileNames;
  std::vector<float> xSections;

  TString selection = "((njets==1 || (njets==2&&(deltaPhi(jet1.Phi(),jet2.Phi()) < 2.5)))&&(nlep==0)&&(ntaus==0)&&(nphotons==0)&&(jet1.Pt()>110.)&&(fjet1.Pt()>250)&&(jet1.Eta()<2.5)&&(jet1.Eta()>-2.5)&&((trigger&(1<<0))||(trigger&(1<<1)))&&(preselWord&(1<<3))&&(HLTmatch&(1<<0))&&(metRaw>200))";

  if(SelectionOpt == 1){
    selection = "(((preselWord & (1<<0)) && (trigger & (1<<2))) && (lep1.Pt() > 30 && abs(lep1.Eta()) < 2.1 && abs(lid1) > 12 && nlep == 1) && (abs(fjet1.Eta()) < 2.5 && nbjets > 1) && (fjet1.Pt() > 250 && abs(lid1) == 13 && deltaR(jet1.Eta(),jet1.Phi(),lep1.Eta(),lep1.Phi()) > 0.3))";
  }

  ifstream configFile;
  configFile.open(configFileName.Data());
  TString tempSB;
  TString tempName;
  float tempXSec;

  while(!configFile.eof()){
    configFile >> tempSB >> tempName >> tempXSec;
    if(tempSB == "Signal" || tempSB == "Background"){
      sampleName.push_back(tempName);
      tempName = TString("/mnt/hscratch/dimatteo/") + Version + TString("/merged/") + Version + TString("_") + tempName + TString("_noskim_flatntuple.root");
      SigBack.push_back(tempSB);
      inFileNames.push_back(tempName);
      xSections.push_back(tempXSec);
    }
  }

  for(unsigned int i0 = 0;i0 < inFileNames.size();i0++){

    cout << "Skimming " << inFileNames[i0] << endl;
    if(inFileNames[i0] == "/afs/cern.ch/work/d/dabercro/public/Fall14/samples/_noskim_flatntuple.root") continue;
    TFile *inFile = new TFile(inFileNames[i0]);
    TTree *inTree = (TTree*) inFile->FindObjectAny("DMSTree");
    TH1D *totHist = (TH1D*) inFile->FindObjectAny("hDAllEvents");
    float XSweight = xSections[i0]/totHist->GetEntries();

    TString outName = outDir;
    if(SigBack[i0] == "Signal"){
      outName = outName + TString("Signal_");
    }
    else if(SigBack[i0] == "Background"){
      outName = outName + TString("Back_");
    }
    else continue;
    outName = outName + sampleName[i0] + TString(".root");
    TFile *outFile = new TFile(outName,"RECREATE");
    TTree *outTree = inTree->CopyTree(selection);
    float weight = 0.;
    TBranch *weightBr = outTree->Branch("weight",&weight,"weight/F");
    float puweight = 0.;
    TTreeFormula *puweightForm = new TTreeFormula("puweight","puweight",outTree);
    
    for(int i1 = 0;i1 < outTree->GetEntries();i1++){
      outTree->GetEntry(i1);
      puweight = puweightForm->EvalInstance();
      weight = XSweight * puweight;
      weightBr->Fill();
    }
    outTree->Write();
    outFile->Write();
    outFile->Close();
    inFile->Close();

  }
}
