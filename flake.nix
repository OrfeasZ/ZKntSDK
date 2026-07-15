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
              pkgs.wine64Packages.stable
            ];

            shellHook = ''
              export VCPKG_ROOT="$PWD/External/vcpkg"
              export VCPKG_FORCE_SYSTEM_BINARIES=1
              export VCPKG_DISABLE_METRICS=1
              export XWIN_SPLAT_DIR="$PWD/.xwin/splat"
              export WINEPREFIX="$PWD/.wine"
              export WINEDEBUG=-all

              if [ ! -d "$WINEPREFIX" ]; then
                env -u NIX_CFLAGS_COMPILE -u NIX_LDFLAGS wineboot --init >/dev/null 2>&1 || true
              fi

              # Regenerate the vcpkg overlay ports
              bash cmake/scripts/sync-vcpkg-overlays.sh >/dev/null \
                || echo "WARNING: sync-vcpkg-overlays.sh failed" >&2
              ln -sfn "$(command -v vcpkg)" "$VCPKG_ROOT/vcpkg" 2>/dev/null || true
            '';
          };
        });
    };
}
