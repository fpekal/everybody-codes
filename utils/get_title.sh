SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)

DECRYPT_AES() {
	echo -n "$1" |
		xxd -r -p |
		openssl enc -aes-256-cbc -d \
			-K $(echo -n "$2" | xxd -p -c 256) \
			-iv $(echo -n "$3" | xxd -p -c 256)
}

SECRETS=$($SCRIPT_DIR/get_secrets.sh)
KEY=$(echo $SECRETS | jq -r .key1)
IV=${KEY:0:16}

ENC_TITLE=$($SCRIPT_DIR/get_encrypted_description.sh | jq -r .title)

if [ "$KEY" != "null" ]; then
	TITLE=$(DECRYPT_AES $ENC_TITLE $KEY $IV)
fi

echo -n $TITLE
