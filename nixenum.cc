#include <nix.hpp>

int main(int argc, char **argv) {

  nix::File file = nix::File::open("nixenum.h5", nix::FileMode::Overwrite);
  nix::Section section = file.createSection("s", "metadata");
  std::vector<nix::Value> boolValues = {nix::Value(true), nix::Value(false), nix::Value(true)};
  nix::Property pb = section.createProperty("boolProperty", boolValues);
  file.close();
}
