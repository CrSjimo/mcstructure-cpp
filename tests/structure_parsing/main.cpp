#include <iostream>
#include <fstream>

#include <Structure.h>
#include <io/stream_reader.h>
#include <io/stream_writer.h>

int main(int argc, char **argv) {
    std::ifstream structureFile("test.mcstructure", std::ios_base::in | std::ios_base::binary);
    assert(structureFile.is_open());
    nbt::io::stream_reader reader(structureFile, endian::little);
    auto comp = reader.read_compound().second;
    auto st = mcstructure::Structure::fromNBT(*comp);
    std::cout << std::get<mcstructure::BlockState>(st.getBlock({1, 0, 0})).name() << std::endl;
    std::cout << st.fillReplace({0, 0, 0}, {4, 4, 2}, mcstructure::BlockState("minecraft:cobblestone"), mcstructure::BlockState("minecraft:glowstone"));
    std::ofstream outputFile("output.mcstructure", std::ios_base::out | std::ios_base::binary);
    nbt::io::stream_writer writer(outputFile, endian::little);
    writer.write_tag("", st.toNBT());
    return 0;
}