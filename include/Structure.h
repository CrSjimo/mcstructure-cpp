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
        ~Structure() = default;

        int fill(const Coordinate &from, const Coordinate &to, const BlockType &block, bool isSecondaryLayer = false);
        int fillOutline(const Coordinate &from, const Coordinate &to, const BlockType &block, bool isSecondaryLayer = false);
        int fillReplace(const Coordinate &from, const Coordinate &to, const BlockType &block, const BlockType &oldBlock, bool isSecondaryLayer = false);

        BlockType getBlock(const Coordinate &point, bool isSecondaryLayer = false) const;
        bool setBlock(const Coordinate &point, const BlockType &block, bool isSecondaryLayer = false);

        static void forEach(const Coordinate &from, const Coordinate &to, const std::function<void (const Coordinate &)> &callback) ;
        void forEach(const Coordinate &from, const Coordinate &to, const std::function<void (const BlockType &, const Coordinate &)> &callback, bool isSecondaryLayer = false) const;

        bool existsInPalette(const BlockState &block) const;

        std::optional<nbt::tag_compound> blockEntityData(const Coordinate &point) const;
        void setBlockEntityData(const Coordinate &point, const nbt::tag_compound &data);
        bool existsBlockPositionData(const Coordinate &point) const;
        bool removeBlockPositionData(const Coordinate &point);

        Size size() const;

        Coordinate worldOrigin() const;
        void setWorldOrigin(const Coordinate &point);

        static Structure fromNBT(const nbt::tag_compound &data);
        nbt::tag_compound toNBT() const;

    private:
        Size m_size;

        std::vector<std::map<BlockState, int>::iterator> m_blockIndices;
        std::vector<std::map<BlockState, int>::iterator> m_secondaryBlockIndices;
        std::map<BlockState, int> m_blockPalette;

        std::vector<nbt::tag_compound> m_entities;

        std::map<int, nbt::tag_compound> m_blockPositionData;

        Coordinate m_worldOrigin;

    };

} // mcstructure

#endif //MCSTRUCTURE_STRUCTURE_H
