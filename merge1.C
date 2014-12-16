#include <iostream>
#include <string>
#include "TFile.h"
#include "TTree.h"
#include "TString.h"

void merge1(std::string iFile0="/scratch/dabercro/BDT_Signal.root",TString varsFile="trainingVars.txt",std::string iFile1="/scratch/dabercro/Output.root") { 
  TFile *lFile0 = new TFile(iFile0.c_str(),"UPDATE");
  TTree *lTree0 = (TTree*) lFile0->FindObjectAny("DMSTree");

  TFile *lFile1 = new TFile(iFile1.c_str());
  TTree *lTree1 = (TTree*) lFile1->FindObjectAny("DMSTree");
  
  ifstream configFile;
  configFile.open(varsFile.Data());
  TString iName;
  configFile >> iName;  // Is the name of the BDT
  // cout << " Test : " << iName0 << endl;

  float lBDT0 = 0; lTree1->SetBranchAddress(iName,&lBDT0); 
  TBranch* lBranch = lTree1->GetBranch(iName);
  
  float lBDT  = 0; 
  TBranch* lBDTBranch = lTree0->Branch(iName,&lBDT,iName+TString("/F"));
  //lTree0->Branch(iName.c_str(),&lBDT);
  //TBranch* lBDTBranch = lTree0->GetBranch(iName.c_str());
  int lNEntries = lTree0->GetEntriesFast();

  for(int i0 = 0; i0 < lNEntries; i0++) { 
    lBranch->GetEntry(i0); 
    lBDT = lBDT0;
    //lTree0->GetEntry(i0); 
    lBDTBranch->Fill();
  }
  lFile0->cd();
  lTree0->Write();
}
