#! /bin/bash

NtupleVersion="boostedv-v9"

ScratchDir="/scratch/"$USER
ls $ScratchDir
SkimDir="/scratch/"$USER/$NtupleVersion"/skims"
BDTDir="/scratch/"$USER/$NtupleVersion"/WithBDT"

TrainingSamples="trainingFiles.txt"
#VariableNames=("trainingVars.txt" "Vars1.txt" "Vars2.txt")
VariableNames=("Vars2.txt")

if [ ! -d /scratch/$USER/$NtupleVersion ]; then
    mkdir /scratch/$USER/$NtupleVersion
fi
if [ ! -d $ScratchDir ]; then
    mkdir $ScratchDir
fi
if [ ! -d $SkimDir ]; then
    mkdir $SkimDir
fi
if [ ! -d $BDTDir ]; then
    mkdir $BDTDir
fi

cd /mnt/hscratch/dimatteo/$NtupleVersion/merged
Samples=(`ls $NtupleVersion*_flatntuple.root`)
cd -

for i0 in `seq 0 1 $((${#Samples[@]}-1))`; do
    if [ ! -f $BDTDir"/"${Samples[$i0]} ]; then
        echo cp "/mnt/hscratch/dimatteo/$NtupleVersion/merged/"${Samples[$i0]} $BDTDir"/"${Samples[$i0]}
        cp "/mnt/hscratch/dimatteo/$NtupleVersion/merged/"${Samples[$i0]} $BDTDir"/"${Samples[$i0]}
    fi
done

root -l -q -b skim.C+\(\"$TrainingSamples\",\"$SkimDir\",\"$NtupleVersion\"\)

hadd -f $SkimDir/BDT_Signal.root $SkimDir/BDT_Signal_*
hadd -f $SkimDir/BDT_Background.root $SkimDir/BDT_Back_*

root -l -q -b addPT.C+\(\"$SkimDir"/BDT_Signal.root"\"\)
root -l -q -b addPT.C+\(\"$SkimDir"/BDT_Background.root"\"\)

cd $BDTDir
files=(`ls $NtupleVersion*`)
cd -
for i1 in `seq 0 1 $((${#files[@]}-1))`; do
    root -l -q -b addPT.C+\(\"$BDTDir/${files[$i1]}\"\)
done

for i0 in `seq 0 1 $((${#VariableNames[@]}-1))`; do
    root -l -q -b classifyBDT.C+\(\"${VariableNames[$i0]}\",\"$SkimDir"/BDT_Signal.root"\",\"$SkimDir"/BDT_Background.root"\"\)
    cp TMVA/TMVA.root TMVA/TMVA_$i0.root
    cp -r weights weights_$i0
    for i1 in `seq 0 1 $((${#files[@]}-1))`; do
        echo ${files[$i1]}
        root -l -q -b applyBDT.C+\(\"$BDTDir/${files[$i1]}\,\"${VariableNames[$i0]}\",\"$ScratchDir"/Output.root"\"\)
        root -l -q -b merge1.C+\(\"$BDTDir/${files[$i1]}\"\,\"${VariableNames[$i0]}\",\"$ScratchDir"/Output.root"\"\)
    done
done

