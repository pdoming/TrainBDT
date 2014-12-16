#! /bin/bash

NtupleVersion="boostedv-v8"

ScratchDir="/home/dabercro/cms/root/TrainBDT/scratch"
SkimDir="/mnt/hscratch/"$USER"/skims2"
BDTDir="/mnt/hscratch/"$USER"/TrainBDT"

TrainingSamples="trainingFiles.txt"
VariableNames="trainingVars.txt"
BDTName="bdt_test"

if [ ! -d $ScratchDir ]; then
    mkdir $ScratchDir
fi
if [ ! -d $SkimDir ]; then
    mkdir $SkimDir
fi
if [ ! -d $BDTDir ]; then
    mkdir $BDTDir
fi

root -l -q -b skim.C+\(\"$TrainingSamples\",\"$ScratchDir\",\"$NtupleVersion\"\)

hadd -f $ScratchDir/BDT_Signal.root $ScratchDir/BDT_Signal_*
hadd -f $ScratchDir/BDT_Background.root $ScratchDir/BDT_Back_*

cp $ScratchDir/BDT_Signal.root $SkimDir/BDT_Signal.root
cp $ScratchDir/BDT_Background.root $SkimDir/BDT_Background.root

rm $ScratchDir/BDT_Signal*
rm $ScratchDir/BDT_Back*

Samples=(`ls /mnt/hscratch/dimatteo/$NtupleVersion/merged/$NtupleVersion*_flatntuple.root`)

Vars=('2*fjet1QGtagSub2+fjet1QGtagSub1' 'fjet1QGtagSub1' 'fjet1QGtagSub2' 'fjet1QGtag' 'fjet1PullAngle' 'fjet1Pull' 'fjet1MassTrimmed' 'fjet1MassPruned' 'fjet1MassSDbm1' 'fjet1MassSDb2' 'fjet1MassSDb0' 'fjet1QJetVol' 'fjet1C2b2' 'fjet1C2b1' 'fjet1C2b0p5' 'fjet1C2b0p2' 'fjet1C2b0' 'fjet1Tau2' 'fjet1Tau1' 'fjet1Tau2/fjet1Tau1')

for Count1 in `seq 0 1 $((${#Vars[@]}-1))`; do
    Var=${Vars[$Count1]}
    echo $Var
#    cp ../weights_$Var/TMVA* weights/.
#    root -q -b -l classify1.C+\(\"$Var\"\)
#    root -q -b -l apply1.C+\(\"../skimmed/background_roc.root\",\"$Var\"\)
#    root -q -b -l applyAll.C+\(\"../skimmed/background_roc.root\",\"$Var\"\)
#    root -q -b -l merge1.C+\(\"../skimmed/background_roc.root\",\"$Var\"\)
#    root -q -b -l applyAll.C+\(\"../scratch/background_word.root\",\"$Var\"\)
#    root -q -b -l merge1.C+\(\"../scratch/background_word.root\",\"$Var\"\)
#    root -q -b -l apply1.C+\(\"../skimmed/signal_roc.root\",\"$Var\"\)
#    root -q -b -l applyAll.C+\(\"../skimmed/signal_roc.root\",\"$Var\"\)
#    root -q -b -l merge1.C+\(\"../skimmed/signal_roc.root\",\"$Var\"\)
#    root -q -b -l applyAll.C+\(\"../scratch/signal_word.root\",\"$Var\"\)
#    root -q -b -l merge1.C+\(\"../scratch/signal_word.root\",\"$Var\"\)
done
