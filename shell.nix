{ everybody-codes, ... }:
{ pkgs, ... }:
pkgs.mkShell {
  CPATH = "${everybody-codes}/include";
  LIBRARY_PATH = "${everybody-codes}/lib";
}
