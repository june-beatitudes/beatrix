{
  description = "BEATRIX Build Environment";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs =
    {
      self,
      nixpkgs,
      ...
    }@inputs:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
      fhs = pkgs.buildFHSEnv {
        name = "bea-tooling-env";
        targetPkgs =
          pkgs:
          (with pkgs; [
            zstd
            zsh
            bear
            pandoc
            libz
            doxygen
            doxygen_gui
            uv
            git-lfs
            gdb
            gnumake
            unzip
            gcc
            gfortran
            blas
            lapack
          ]);
      };
    in
    {
      devShells.${system}.default = fhs.env;
    };
}
