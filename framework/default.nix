{ pkgs, utils, ... }:
pkgs.stdenv.mkDerivation {
  name = "everybody-codes-framework";
  version = "1.0.0";

  src = ./.;

  buildInputs = [ pkgs.nlohmann_json ];

  nativeBuildInputs = [ pkgs.pkg-config ];

  UTILS_DIR = "${utils}";
}
