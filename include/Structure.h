#ifndef MCSTRUCTURE_STRUCTURE_H
#define MCSTRUCTURE_STRUCTURE_H

#include "BlockState.h"
#include "Coordinate.h"
#include "Size.h"

#include <optional>
#include <functional>

namespace mcstructure {

    class Structure {
    public:
        enum SpecialBlockValue {
            StructureVoid,
        };
        using BlockType = std::variant<SpecialBlockValue, BlockState>;

        explicit Structure(const Size &size);

        void fill(const Coordinate &from, const Coordinate &to, const BlockType &block, bool isSecondaryLayer = false);
        void fillOutline(const Coordinate &from, const Coordinate &to, const BlockType &block, bool isSecondaryLayer = false);
        void fillReplace(const Coordinate &from, const Coordinate &to, const BlockType &block, const BlockType &oldBlock, bool isSecondaryLayer = false);

        BlockType getBlock(const Coordinate &point, bool isSecondaryLayer = false);
        void setBlock(const Coordinate &point, const BlockType &block, bool isSecondaryLayer = false);

        void forEach(const Coordinate &from, const Coordinate &to, const std::function<void (const BlockType &, const Coordinate &)> &callback, bool isSecondaryLayer = false);

        bool existsInPalette(const BlockState &block);
        bool replaceInPalette(const BlockState &block, const BlockState &oldBlock);

        std::optional<nbt::tag_compound> blockEntityData(const Coordinate &point);
        void setBlockEntityData(const Coordinate &point, const nbt::tag_compound &data);
        std::optional<int> tickQueueData(const Coordinate &point);
        void setTickQueueData(const Coordinate &point, int tickDelay);

        bool existsBlockPositionData(const Coordinate &point);
        bool removeBlockPositionData(const Coordinate &point);

        Size size() const;

        Coordinate worldOrigin() const;
        void setWorldOrigin(const Coordinate &point);

        static Structure fromNBT(const nbt::tag_compound &data);
        std::unique_ptr<nbt::tag_compound> toNBT() const;

    };

} // mcstructure

#endif //MCSTRUCTURE_STRUCTURE_H
