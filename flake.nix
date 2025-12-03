{
  description = "C Flake using flake parts";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-parts.url = "github:hercules-ci/flake-parts";
  };

  outputs = inputs @ {
    self,
    nixpkgs,
    flake-parts,
    ...
  }: let
    lib = nixpkgs.lib;
  in
    flake-parts.lib.mkFlake {inherit inputs;} {
      imports = [
        # To import an internal flake module: ./other.nix
        # To import an external flake module:
        #   1. Add foo to inputs
        #   2. Add foo as a parameter to the outputs function
        #   3. Add here: foo.flakeModule
      ];
      systems = ["x86_64-linux" "aarch64-linux" "aarch64-darwin" "x86_64-darwin"];
      perSystem = {
        pkgs,
        self',
        system,
        ...
      }: let
        aocPrefix = "aoc2025-day";
        aocPrefixLength = lib.strings.stringLength aocPrefix;
        buildDayPackage = {dayString}:
          pkgs.stdenv.mkDerivation {
            pname = "${aocPrefix}${dayString}";
            version = "1.0.0";

            src = ./.;

            nativeBuildInputs = [pkgs.clang];

            buildPhase = ''
              mkdir -p dist
              clang -I. -lm src/day${dayString}.c -o dist/day${dayString}
            '';

            installPhase = ''
              mkdir -p $out/bin
              cp dist/day${dayString} $out/bin/${aocPrefix}${dayString}
            '';
          };
      in {
        # Per-system attributes can be defined here. The self' and inputs'
        # module parameters provide easy access to attributes of the same
        # system.
        _module.args.pkgs = import self.inputs.nixpkgs {
          inherit system;
          config.allowUnfree = true;
          config.cudaSupport.enable = true;
        };

        packages =
          (
            lib.genAttrs
            (map (n: lib.strings.concatStrings [aocPrefix (lib.strings.fixedWidthString 2 "0" (toString n))]) (lib.range 1 12))
            (
              aocString: let
                dayString = lib.strings.substring aocPrefixLength (lib.strings.stringLength aocString - aocPrefixLength) aocString;
              in
                buildDayPackage {inherit dayString;}
            )
          )
          // {
            aoc2025 = pkgs.writeShellApplication {
              name = "aoc2025";
              runtimeInputs = [
                self'.packages."${aocPrefix}01"
                self'.packages."${aocPrefix}02"
                self'.packages."${aocPrefix}03"
              ];
              text =
                /*
                bash
                */
                ''
                  set +o errexit
                  set +o pipefail

                  Color_Off='\033[0m'

                  IRed='\033[0;91m'
                  IGreen='\033[0;92m'
                  IYellow='\033[0;93m'

                  BIGreen='\033[1;92m'

                  echo -e "$BIGreen""Advent of Code 2025$Color_Off"

                  star="\e[5;33m*\e[0m\e[1;32m"
                  o="\e[0m\e[1;31mo\e[0m\e[1;32m"

                  echo -e "
                          $star
                         /.\\
                        /$o..\\
                        /..$o\\
                       /.$o..$o\\
                       /...$o.\\
                      /..$o....\\
                      ^^^[_]^^^
                  "

                  echo -e "$IRed""--- Day 1: Secret Entrance ---""$Color_Off"
                  ${aocPrefix}01 < ./input/day01.input

                  echo -e "$IGreen""--- Day 2: Gift Shop ---""$Color_Off"
                  ${aocPrefix}02 < ./input/day02.input

                  echo -e "$IYellow""--- Day 3: Lobby ---""$Color_Off"
                  ${aocPrefix}03 < ./input/day03.input
                '';
            };
            aoc2025-get = pkgs.writeShellApplication {
              name = "aoc2025-get";
              runtimeInputs = [pkgs.curl];
              text =
                /*
                bash
                */
                ''
                  set +o errexit
                  set +o pipefail
                  set +o nounset

                  usage() {
                      echo "Usage: $0 [-h | --help] [--year <year>] <day>"
                      echo
                      echo "Options:"
                      echo "  -h, --help      Show this help message and exit."
                      echo "  --year <year>   Choose the year to download the input for; default 2025."
                      echo "  day             Download the given day."
                      echo
                      echo "Environment:"
                      echo "  AOC_SESSION=... The Set-Cookie from the AoC Website"
                  }

                  day=0
                  year=2025
                  while [[ $# -gt 0 ]]; do
                      case "$1" in
                          -h|--help)
                              usage
                              exit 0
                              ;;
                          --year)
                              if [ -z "$2" ]; then
                                  echo "$1"
                                  echo "$2"
                                  usage
                                  exit 1
                              fi
                              year="$2"
                              shift
                              shift
                              ;;
                          *)
                              day="$1"
                              shift
                              ;;
                      esac
                  done

                  if [ -z "$day" ]; then
                      usage
                      exit 1
                  fi

                  if [ -z "$AOC_SESSION" ]; then
                      usage
                      exit 1
                  fi

                  mkdir -p input
                  filename=input/day$(printf "%02d" "$day").input

                  curl -sS -o "$filename" -b "session=$AOC_SESSION" https://adventofcode.com/"$year"/day/"$day"/input
                '';
            };
          };

        devShells.default = pkgs.mkShell {
          packages = [
            pkgs.clang
          ];
        };
      };
      flake = {
        # The usual flake attributes can be defined here, including system-
        # agnostic ones like nixosModule and system-enumerating ones, although
        # those are more easily expressed in perSystem.
      };
    };
}
