{
  description = "BEATRIX Build Environment";

  inputs = {
    # AS OF 2025-07-04: using 24.11 because libtinfo is broken on 25.05 and unstable
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-24.11";
  };

  outputs =
    { self, nixpkgs, ... }@inputs:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
      fhs = pkgs.buildFHSEnv {
        name = "bea-tooling-env";
        targetPkgs =
          pkgs:
          (with pkgs; [
            zstd
            ncurses
            libtinfo
            zsh
            bear
          ]);
      };
    in
    {
      devShells.${system}.default = fhs.env;
    };
}
