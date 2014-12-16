#include <cstdlib>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"

#include "TMVA/TMVAGui.C"

#if not defined(__CINT__) || defined(__MAKECINT__)
// needs to be included when makecint runs (ACLIC)
#include "TMVA/Tools.h"
#include "TMVA/Factory.h"
#include "TMVA/MethodCategory.h"
#include "TMVA/Tools.h"
#endif

void classifyBDT(TString inputVariables = "trainingVars.txt",
                 TString signalName = "/mnt/hscratch/dabercro/skims2/BDT_Signal.root",
                 TString backName = "/mnt/hscratch/dabercro/skims2/BDT_Background.root") {
  TMVA::Tools::Instance();
  std::cout << "==> Start TMVAClassification" << std::endl;

   // Create a ROOT output file where TMVA will store ntuples, histograms, etc.
   TString outfileName( "TMVA/TMVA.root" );
   TFile* outputFile = TFile::Open( outfileName, "RECREATE" );
   TMVA::Factory *factory = new TMVA::Factory( "TMVAClassificationCategory", outputFile,
					       "!V:!Silent:Color:DrawProgressBar:Transformations=I;N" );

   // A very simple MVA (feel free to uncomment and comment what you like) => as a rule of thumb 10-20 variables is where people start to get worried about total number

   ifstream configFile;
   configFile.open(inputVariables.Data());
   TString tempFormula;

   configFile >> tempFormula;  // Is the name of the BDT
   while(!configFile.eof()){
     configFile >> tempFormula;
     if(tempFormula != ""){
       factory->AddVariable(tempFormula,'F');
     }
   }

   TString lVars;

   // TCut lCut   = "jet1qg2<2.&&jet1pt>250.&&jet1pullAngle>-5.";// < 10 && jet1mass_m2 > 60 && jet1mass_m2 < 120";
   // TCut lCut = "passZ > 3  && fjet1pt > 250 && fjet1MassPruned < 120 && fatjetid < 2";
   TCut lCut   = "abs(fjet1PartonId)!=24&&abs(fjet1PartonId)!=23";
   // std::string lEventCut = "event % 2 == 1";
   // lCut += lEventCut.c_str();

   // TCut lSCut = "passT > 0   && fjet1pt > 250 && fjet1MassPruned < 120 && abs(fjet1PartonId) == 24&& fatjetid < 2";
   TCut lSCut   = "abs(fjet1PartonId)==24||abs(fjet1PartonId)==23";
   // lSCut += lEventCut.c_str();

   TCut cleanCut = "fjet1QGtagSub2 > -10 && fjet1PullAngle > -4 && abs(fjet1pt/fjet1MassTrimmed)<200 && abs(fjet1pt/fjet1MassPruned)<200";

   TFile *lSAInput = TFile::Open(signalName);
   TTree   *lSASignal    = (TTree*)lSAInput    ->Get("DMSTree"); 
   TFile *lSBInput = TFile::Open(backName);
   TTree   *lSBSignal    = (TTree*)lSBInput    ->Get("DMSTree"); 
   
   Double_t lSWeight = 1.0;
   Double_t lBWeight = 1.0;
   gROOT->cd( outfileName+TString(":/") );   
   factory->AddSignalTree    ( lSASignal, lSWeight );
   
   gROOT->cd( outfileName+TString(":/") );   
   factory->AddBackgroundTree( lSBSignal, lBWeight );
   
   factory->SetWeightExpression("weight");
   std::stringstream pSignal,pBackground;
   pSignal << "nTrain_Signal="<< lSASignal->GetEntries() << ":nTrain_Background=" << lSBSignal->GetEntries();
   // factory->PrepareTrainingAndTestTree( lSCut, lCut,(pSignal.str()+":SplitMode=Block:NormMode=NumEvents:!V").c_str() );
   factory->PrepareTrainingAndTestTree(lSCut&&cleanCut,lCut&&cleanCut,"nTrain_Signal=0:nTrain_Background=0:SplitMode=Alternate:NormMode=NumEvents:!V");
   std::string lName = "alpha_VBF";
   TString lBDTDef   = "!H:!V:NTrees=400:BoostType=Grad:Shrinkage=0.1:UseBaggedGrad=F:nCuts=2000:NNodesMax=10000:MaxDepth=5:UseYesNoLeaf=F:nEventsMin=200";
   factory->BookMethod(TMVA::Types::kBDT,"BDT_simple_alpha",lBDTDef);   
   factory->TrainAllMethods();
   factory->TestAllMethods();
   factory->EvaluateAllMethods();
   outputFile->Close();

   std::cout << "==> Wrote root file: " << outputFile->GetName() << std::endl;
   std::cout << "==> TMVAClassification is done!" << std::endl;
   delete factory;
   //if (!gROOT->IsBatch()) TMVAGui( outfileName );
   //TString lBDTDef   = "!H:!V:NTrees=100:BoostType=Grad:Shrinkage=0.10:UseBaggedGrad=F:nCuts=2000:NNodesMax=10000:MaxDepth=3:SeparationType=GiniIndex";
}
