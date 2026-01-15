{ pkgs, ... }:
pkgs.stdenv.mkDerivation {
  pname = "utils";
  version = "1.0.0";

  src = ./.;

  nativeBuildInputs = [ pkgs.makeWrapper ];

  buildPhase = "true";
  installPhase = ''
    mkdir $out
    cp *.sh $out

    wrapProgram $out/get_inputs.sh \
      --suffix PATH : "${
        pkgs.lib.makeBinPath [ pkgs.jq pkgs.openssl pkgs.xxd ]
      }"

    wrapProgram $out/get_title.sh \
      --suffix PATH : "${
        pkgs.lib.makeBinPath [ pkgs.jq pkgs.openssl pkgs.xxd ]
      }"
  '';
}
