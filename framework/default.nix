{pkgs ? import <nixpkgs> {}, ...}:
pkgs.stdenv.mkDerivation {
  name = "everybody-codes-framework";
  version = "1.0.0";

  src = ./.;
}
