#include <cstdlib>
#include <iostream>
#include <fstream>
#include <map>
#include <string>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TTreeFormula.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"

#include "TMVA/TMVAGui.C"

#if not defined(__CINT__) || defined(__MAKECINT__)
// needs to be included when makecint runs (ACLIC)
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCategory.h"
#include "TMVA/Tools.h"
#endif

void applyBDT(std::string iName="/mnt/hscratch/dabercro/skims2/BDT_Signal.root",
              TString inputVariables = "trainingVars.txt",
              TString outName = "/scratch/dabercro/Output.root",
              std::string iWeightFile="weights/TMVAClassificationCategory_BDT_simple_alpha.weights.xml") { 
  TMVA::Tools::Instance();
  TMVA::Reader *reader = new TMVA::Reader( "!Color:!Silent" );    

  TString BDTName;
  ifstream configFile;
  configFile.open(inputVariables.Data());
  TString tempFormula;

  std::vector<float> Evaluated;
  std::vector<TString> Strings;
  TTreeFormula *Formulas[40];
  
  configFile >> BDTName;  // Is the name of the BDT

  while(!configFile.eof()){
    configFile >> tempFormula;
    if(tempFormula != ""){
      Evaluated.push_back(0.);
      Strings.push_back(tempFormula);
    }
  }
  
  TFile *lFile = new TFile(iName.c_str());
  TTree *lTree = (TTree*) lFile->FindObjectAny("DMSTree");

  for(unsigned int i0 = 0;i0 < Strings.size();i0++){
    reader->AddVariable(Strings[i0],&Evaluated[i0]);
    Formulas[i0] = new TTreeFormula(Strings[i0],Strings[i0],lTree);
  }

  std::string lJetName = "BDT";
  reader->BookMVA(lJetName .c_str(),iWeightFile.c_str());
  
  int lNEvents = lTree->GetEntries();
  TFile *lOFile = new TFile(outName,"RECREATE");
  TTree *lOTree = new TTree("DMSTree","DMSTree");
  float lMVA    = 0; lOTree->Branch(BDTName,&lMVA ,BDTName+TString("/F"));
  for (Long64_t i0=0; i0<lNEvents;i0++) {
    if (i0 % 10000 == 0) std::cout << "--- ... Processing event: " << double(i0)/double(lNEvents) << std::endl;
    lTree->GetEntry(i0);
    for(unsigned int i1 = 0;i1 < Strings.size();i1++){
      Evaluated[i1] = Formulas[i1]->EvalInstance();
    }

    lMVA      = float(reader->EvaluateMVA(lJetName.c_str()));
    lOTree->Fill();
  }
  lOTree->Write();
  lOFile->Close();
  delete reader;
}
