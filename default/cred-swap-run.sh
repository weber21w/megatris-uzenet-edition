#!/bin/bash
cd $(dirname $0)

ROM_NAME="$1"
if [[ -z "$ROM_NAME" ]]; then ROM_NAME="Megatris.uze"; fi

MAX_CREDS="99"

for (( i=1; i<$MAX_CREDS; i++ )) #find the highest number credentials
do

	if [[ -f "eeprom${i}.bin" ]]; then echo "found [eeprom${i}.bin]"; continue; fi

	if [[ "$i" -eq "1" ]]; then echo "ERROR must have more than 1 eeprom.bin, like eeprom.bin, eeprom1.bin, eeprom2.bin..."; exit 1; fi
	((i--))
	HIGHEST="$i"
	break;
done
echo "Highest: $HIGHEST"
echo "moving [eeprom${HIGHEST}.bin to eepromT.bin]"
mv eeprom${HIGHEST}.bin eepromT.bin #temporarily move this, so we can move the rest up
((HIGHEST--))

for (( j=$HIGHEST; j>0; j-- ))
do
	if [[ ! -f "eeprom${j}.bin" ]]; then echo "didn't find [eeprom${j}.bin]"; exit; fi
	NEXT="$j"
	((NEXT++))
	echo "renaming [eeprom${j}.bin to eeprom${NEXT}.bin]"
	mv eeprom${j}.bin eeprom${NEXT}.bin 
done

((HIGHEST++))
echo "moving [eeprom.bin to eeprom1.bin]"
mv eeprom.bin eeprom1.bin
echo "moving [eepromT.bin to eeprom.bin]"
mv eepromT.bin eeprom.bin

./cuzebox "$ROM_NAME"
