#! /bin/bash

NtupleVersion="boostedv-v8"

ScratchDir="/scratch/"$USER
ls $ScratchDir
SkimDir="/mnt/hscratch/"$USER"/skims2"
BDTDir="/mnt/hscratch/"$USER"/TrainBDT"

TrainingSamples="trainingFiles.txt"
#VariableNames=("trainingVars.txt" "Vars1.txt" "Vars2.txt")
VariableNames=("Vars1.txt")

cd /mnt/hscratch/dimatteo/$NtupleVersion/merged
Samples=(`ls $NtupleVersion*_flatntuple.root`)
cd -

for i0 in `seq 0 1 $((${#Samples[@]}-1))`; do
    if [ ! -f $BDTDir"/"${Samples[$i0]} ]; then
        echo cp "/mnt/hscratch/dimatteo/$NtupleVersion/merged/"${Samples[$i0]} $BDTDir"/"${Samples[$i0]}
        cp "/mnt/hscratch/dimatteo/$NtupleVersion/merged/"${Samples[$i0]} $BDTDir"/"${Samples[$i0]}
    fi
done

if [ ! -d $ScratchDir ]; then
    mkdir $ScratchDir
fi
if [ ! -d $SkimDir ]; then
    mkdir $SkimDir
fi
if [ ! -d $BDTDir ]; then
    mkdir $BDTDir
fi

#root -l -q -b skim.C+\(\"$TrainingSamples\",\"$ScratchDir\",\"$NtupleVersion\"\)

#hadd -f $ScratchDir/BDT_Signal.root $ScratchDir/BDT_Signal_*
#hadd -f $ScratchDir/BDT_Background.root $ScratchDir/BDT_Back_*

#root -l -q -b addPT.C+\(\"$ScratchDir"/BDT_Signal.root"\"\)
#root -l -q -b addPT.C+\(\"$ScratchDir"/BDT_Background.root"\"\)

#cp $ScratchDir/BDT_Signal.root $SkimDir/BDT_Signal.root
#cp $ScratchDir/BDT_Background.root $SkimDir/BDT_Background.root

#rm $ScratchDir/BDT_Signal*
#rm $ScratchDir/BDT_Back*

for i0 in `seq 0 1 $((${#VariableNames[@]}-1))`; do
#    root -l -q -b classifyBDT.C+\(\"${VariableNames[$i0]}\",\"$SkimDir"/BDT_Signal.root"\",\"$SkimDir"/BDT_Background.root"\"\)
    cp -r weights weights_$i0
    cd $BDTDir
    files=(`ls $NtupleVersion*`)
    cd -
    for i1 in `seq 0 1 $((${#files[@]}-1))`; do
        echo ${files[$i1]}
        cp $BDTDir/${files[$i1]} $ScratchDir"/appending.root"
        root -l -q -b addPT.C+\(\"$ScratchDir"/appending.root"\"\)
        root -l -q -b applyBDT.C+\(\"$ScratchDir"/appending.root"\"\,\"${VariableNames[$i0]}\",\"$ScratchDir"/Output.root"\"\)
        root -l -q -b merge1.C+\(\"$ScratchDir"/appending.root"\"\,\"${VariableNames[$i0]}\",\"$ScratchDir"/Output.root"\"\)
        cp $ScratchDir"/appending.root" $BDTDir/${files[$i1]}
    done
done
