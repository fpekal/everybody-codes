{everybody-codes, ...}: {pkgs, ...}:
pkgs.mkShell {
  buildInputs = [pkgs.bashInteractive];

  shellHook = ''
    export SHELL="${pkgs.bashInteractive}/bin/bash";
  '';
  CPATH = "${everybody-codes}/include";
  LIBRARY_PATH = "${everybody-codes}/lib";
}
