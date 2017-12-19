#!/bin/bash

channel=$(grep 'int CHAN' "Settings.h" | awk -F'[=;]' '{print $2}')
output=$(grep 'output_folder' "Settings.h" | awk -F'[=;]' '{print $2}' | tr -d '"')
analysis=$(grep 'analysis' "Settings.h" | awk -F'[=;]' '{print $2}' | tr -d '"')
dc_path=$(grep 'DC_folder' "Settings.h" | awk -F'[=;]' '{print $2}' | tr -d '"')
echo $HOME/$output
echo $dc_path

sed s/user=\"whoami\"/user=\"$USER\"/g Settings.h >/tmp/Settings$USER.h
yes | mv /tmp/Settings$USER.h Settings.h
sed s/user=whoami/user=$USER/g BuildStructure.sh >/tmp/BuildStructure$USER.sh
yes | mv /tmp/BuildStructure$USER.sh BuildStructure.sh
sed s/fftype=fftype/fftype=$analysis/g BuildStructure.sh >/tmp/BuildStructure$USER.sh
yes | mv /tmp/BuildStructure$USER.sh BuildStructure.sh

echo "Compiling the framework"
sh BuildStructure.sh
cd ../
make -B

#./Preselection
./SRHisto
./CRHisto
./steerFF
./fitFakeFactors
/calcCorrections
python plotCorrections.py --channel $channel
./convert_inputs
python cpTDHaftPublic.py --destination $HOME/$output --channel $channel
python producePublicFakeFactors.py --input $HOME/$output --channel $channel
python cpPublicFFtoDC.py --source $HOME/$output --destination $dc_path --channel $channel

rm $HOME/$output/constant.root
