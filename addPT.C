#include <iostream>
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TTreeFormula.h"
#include "TBranch.h"

void addPT(TString inFile = "/scratch/dabercro/appending.root"){
  TFile *lFile0 = new TFile(inFile,"UPDATE");
  TTree *lTree0 = (TTree*) lFile0->FindObjectAny("DMSTree");

  TString iName = "fjet1pt";
  TBranch *checking = lTree0->GetBranch(iName);
  if (checking == NULL){
    float lBDT  = 0; 
    TBranch* lBDTBranch = lTree0->Branch(iName,&lBDT,iName+TString("/F"));
    int lNEntries = lTree0->GetEntriesFast();
    TTreeFormula *formula = new TTreeFormula("fjet1.Pt()","fjet1.Pt()",lTree0);
    for(int i0 = 0; i0 < lNEntries; i0++) { 
      lTree0->GetEntry(i0);
      lBDT = formula->EvalInstance();
      lBDTBranch->Fill();
    }
    lFile0->cd();
    lTree0->Write();
  }
  lFile0->Close();
}
