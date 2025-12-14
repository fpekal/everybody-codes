DECRYPT_AES() {
	echo -n "$1" |
		xxd -r -p |
		openssl enc -aes-256-cbc -d \
			-K $(echo -n "$2" | xxd -p -c 256) \
			-iv $(echo -n "$3" | xxd -p -c 256)
}

SECRETS=$(./get_secrets.sh)
KEY1=$(echo $SECRETS | jq -r .key1)
IV1=${KEY1:0:16}

KEY2=$(echo $SECRETS | jq -r .key2)
IV2=${KEY2:0:16}

KEY3=$(echo $SECRETS | jq -r .key3)
IV3=${KEY3:0:16}

ENC_INPUTS=$(./get_encrypted_inputs.sh)

if [ "$KEY1" != "null" ]; then
	PART1=$(DECRYPT_AES $(echo -n $ENC_INPUTS | jq -r '.["1"]') $KEY1 $IV1)
fi
if [ "$KEY2" != "null" ]; then
	PART2=$(DECRYPT_AES $(echo -n $ENC_INPUTS | jq -r '.["2"]') $KEY2 $IV2)
fi
if [ "$KEY3" != "null" ]; then
	PART3=$(DECRYPT_AES $(echo -n $ENC_INPUTS | jq -r '.["3"]') $KEY3 $IV3)
fi

echo -n '{'
echo -n '"1":"'
echo -n $PART1
echo -n '",'
echo -n '"2":"'
echo -n $PART2
echo -n '",'
echo -n '"3":"'
echo -n $PART3
echo -n '"'
echo -n '}'
