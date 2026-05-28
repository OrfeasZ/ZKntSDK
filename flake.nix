{
  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = { self, nixpkgs }:
    let
      systems = [ "x86_64-linux" ];
      forEach = nixpkgs.lib.genAttrs systems;
    in {
      devShells = forEach (system:
        let
          pkgs = import nixpkgs { inherit system; };
          llvm = pkgs.llvmPackages;
        in {
          default = pkgs.mkShell {
            packages = [
              pkgs.cmake
              pkgs.ninja
              pkgs.pkg-config
              pkgs.vcpkg-tool
              pkgs.xwin
              llvm.clang-unwrapped
              pkgs.lld
              llvm.llvm
              pkgs.glibc.static
              pkgs.git
              pkgs.curl
              pkgs.zip
              pkgs.unzip
              pkgs.gnutar
              pkgs.zstd
              pkgs.gdb
            ];

            shellHook = ''
              export VCPKG_ROOT="$PWD/External/vcpkg"
              export VCPKG_FORCE_SYSTEM_BINARIES=1
              export VCPKG_DISABLE_METRICS=1
              export XWIN_SPLAT_DIR="$PWD/.xwin/splat"
              ln -sfn "$(command -v vcpkg)" "$VCPKG_ROOT/vcpkg" 2>/dev/null || true
            '';
          };
        });
    };
}
