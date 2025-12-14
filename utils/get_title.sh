DECRYPT_AES() {
	echo -n "$1" |
		xxd -r -p |
		openssl enc -aes-256-cbc -d \
			-K $(echo -n "$2" | xxd -p -c 256) \
			-iv $(echo -n "$3" | xxd -p -c 256)
}

SECRETS=$(./get_secrets.sh)
KEY=$(echo $SECRETS | jq -r .key1)
IV=${KEY:0:16}

ENC_TITLE=$(./get_encrypted_description.sh | jq -r .title)

if [ "$KEY" != "null" ]; then
	TITLE=$(DECRYPT_AES $ENC_TITLE $KEY $IV)
fi

echo -n $TITLE
