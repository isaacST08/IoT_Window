{pkgs ? import <nixpkgs> {}}: let
  local-pico-sdk = pkgs.pico-sdk.override {
    withSubmodules = true;
  };
  arm-none-eabi = pkgs.gcc-arm-embedded;
in
  pkgs.mkShell {
    buildInputs = with pkgs; [
      arm-none-eabi
      local-pico-sdk
      picotool

      cmake
      python3 # build requirements for pico-sdk
      udisks # Interact with bootloader filesystem
      tio # terminal program to interface with serial
    ];
    shellHook =
      # bash
      ''
        export PICO_SDK_PATH=${local-pico-sdk}/lib/pico-sdk
        export ARM_NONE_EABI_PATH="${arm-none-eabi}/arm-none-eabi/include"
      '';
  }
