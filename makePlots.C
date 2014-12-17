#include <iostream>
#include "TH1F.h"
#include "TString.h"
#include "TCut.h"
#include "TFile.h"
#include "TTree.h"
#include "TObject.h"
#include "TMath.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TLegend.h"

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

TCut selection = "((njets==1 || (njets==2&&(deltaPhi(jet1.Phi(),jet2.Phi()) < 2.5)))&&(nlep==0)&&(ntaus==0)&&(nphotons==0)&&(jet1.Pt()>110.)&&(fjet1.Pt()>250)&&(jet1.Eta()<2.5)&&(jet1.Eta()>-2.5)&&((trigger&(1<<0))||(trigger&(1<<1)))&&(preselWord&(1<<3))&&(HLTmatch&(1<<0))&&(metRaw>250))";
TCut bdtCut = "bdt_noMass > 0.6";

void makePlots(){

  TString NameBase = "bdt_noMass_Cut";

  gStyle->SetOptStat(0);

  TFile *file1 = new TFile("/scratch/dabercro/boostedv-v9/skims/BDT_Signal.root");
  TFile *file2 = new TFile("/scratch/dabercro/boostedv-v9/skims/BDT_Background.root");

  TTree *tree1 = (TTree*) file1->FindObjectAny("DMSTree");
  TTree *tree2 = (TTree*) file2->FindObjectAny("DMSTree");

  TString AxisTitle = ";Pruned Mass [GeV];Normalized Distribution";

  int numBins = 40;
  float histMin = 0;
  float histMax = 200;

  TH1F *hist1 = new TH1F("hist1",AxisTitle,numBins,histMin,histMax);
  TH1F *hist2 = new TH1F("hist2",AxisTitle,numBins,histMin,histMax);

  hist1->SetLineColor(kBlack);
  hist2->SetLineColor(kRed);

  hist1->SetLineWidth(2);
  hist2->SetLineWidth(2);

  TLegend *theLegend = new TLegend(0.55,0.55,0.85,0.85);
  theLegend->SetMargin(0.2);
  theLegend->SetFillColor(0);
  theLegend->SetBorderSize(0);

  theLegend->AddEntry(hist1,"Signal (VH)","l");
  theLegend->AddEntry(hist2,"Background (Z+jets)","l");

  tree1->Draw("fjet1MassPruned>>hist1","weight"*("(abs(fjet1PartonId)==23||abs(fjet1PartonId)==24)"&&bdtCut));
  tree2->Draw("fjet1MassPruned>>hist2","weight"*("(abs(fjet1PartonId)!=23&&abs(fjet1PartonId)!=24)"&&bdtCut));

  TFile *theFile = new TFile(NameBase+TString(".root"),"RECREATE");

  TCanvas *c1 = new TCanvas();

  hist2->DrawNormalized();
  hist1->DrawNormalized("SAME");

  theLegend->Draw();

  hist1->Write();
  hist2->Write();

  c1->Write();
  c1->SaveAs(NameBase+TString(".png"));
  c1->SaveAs(NameBase+TString(".pdf"));

  theFile->Close();

}
