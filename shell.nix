{
  pkgs ? import <nixpkgs> { },
}:
(pkgs.buildFHSEnv {
  name = "tooling-env";
  multiPkgs =
    pkgs:
    (with pkgs; [
      ncurses
      zstd
      libtinfo
      fish
    ]);
  runScript = "fish";
}).env
