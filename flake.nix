{
  inputs = {nixpkgs.url = "github:nixos/nixpkgs";};

  outputs = {
    self,
    nixpkgs,
    ...
  }: let
    pkgs = nixpkgs.legacyPackages.x86_64-linux;
  in {
    packages.x86_64-linux = {
      utils = pkgs.callPackage ./utils/default.nix {};
      everybody-codes = pkgs.callPackage ./framework/default.nix {
        inherit (self.packages.x86_64-linux) utils;
      };
      default = self.packages.x86_64-linux.everybody-codes;
    };

    devShells.x86_64-linux.default =
      pkgs.callPackage (import ./shell.nix self.packages.x86_64-linux) {};
  };
}
