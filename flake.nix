# Contributed by TRONG

{
  description = "Build the HayBox firmware";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, ... }@inputs: inputs.utils.lib.eachSystem [
    "x86_64-linux" "i686-linux" "aarch64-linux" "x86_64-darwin"
  ] (system: let
    pkgs = import nixpkgs {
      inherit system;
    };
  in {
    devShells.default = pkgs.mkShell rec {
      name = "haybox";

      packages = with pkgs; [
        # Build Tools
        platformio-core
        pico-sdk
        avrdude
        openocd
        elfutils
        # Build Dependencies
        gcc-arm-embedded
        python3
        udev
      ];
    };

    packages.buildBox = pkgs.buildFHSUserEnv {
      name = "gcc-arm-env";
      targetPkgs = pkgs: [
        pkgs.platformio-core
        pkgs.pico-sdk
        pkgs.avrdude
        pkgs.openocd
        pkgs.elfutils
        pkgs.gcc-arm-embedded
        pkgs.python3
        pkgs.udev
      ];
    };
  });
}
