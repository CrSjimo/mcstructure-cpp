#include "Structure.h"

#include <algorithm>
#include <iterator>

#include <tag_list.h>
#include <tag_string.h>

namespace mcstructure {
    Structure::Structure(const Size &size) : m_size(size), m_worldOrigin(0, 0, 0) {
        m_blockIndices.resize(m_size.volume());
        m_secondaryBlockIndices.resize(m_size.volume());
        std::fill(m_blockIndices.begin(), m_blockIndices.end(), m_blockPalette.end());
        std::fill(m_secondaryBlockIndices.begin(), m_secondaryBlockIndices.end(), m_blockPalette.end());
    }

    int Structure::fill(const Coordinate &from, const Coordinate &to, const Structure::BlockType &block,
                        bool isSecondaryLayer) {
        int count = 0;
        forEach(from, to, [=, &count](const Coordinate &point) {
            if (setBlock(point, block, isSecondaryLayer))
                count++;
        });
        return count;
    }

    int Structure::fillOutline(const Coordinate &from, const Coordinate &to, const Structure::BlockType &block,
                               bool isSecondaryLayer) {
        int count = 0;
        forEach(from, to, [=, &count](const Coordinate &point) {
            if (point.x == from.x || point.y == from.y || point.z == from.z || point.x == to.x || point.y == to.y || point.z == to.z)
                if (setBlock(point, block, isSecondaryLayer))
                    count++;
        });
        return count;
    }

    int Structure::fillReplace(const Coordinate &from, const Coordinate &to, const Structure::BlockType &block,
                               const Structure::BlockType &oldBlock, bool isSecondaryLayer) {
        int count = 0;
        forEach(from, to, [=, &count](const BlockType &currentBlock, const Coordinate &point) {
            if (currentBlock == oldBlock)
                if (setBlock(point, block, isSecondaryLayer))
                    count++;
        }, isSecondaryLayer);
        return count;
    }

    Structure::BlockType Structure::getBlock(const Coordinate &point, bool isSecondaryLayer) const {
        auto pointIndex = point.toIndex(m_size);
        assert(pointIndex >= 0 && pointIndex < m_size.volume());
        auto &indices = isSecondaryLayer ? m_secondaryBlockIndices : m_blockIndices;
        auto it = indices[pointIndex];
        if (it == m_blockPalette.end())
            return StructureVoid;
        else
            return it->first;
    }

    bool Structure::setBlock(const Coordinate &point, const Structure::BlockType &block, bool isSecondaryLayer) {
        auto pointIndex = point.toIndex(m_size);
        assert(pointIndex >= 0 && pointIndex < m_size.volume());
        auto &indices = isSecondaryLayer ? m_secondaryBlockIndices : m_blockIndices;
        auto &itRef = indices[pointIndex];
        if (std::holds_alternative<SpecialBlockValue>(block)) {
            if (itRef != m_blockPalette.end()) {
                itRef->second--;
                if (itRef->second == 0)
                    m_blockPalette.erase(itRef);
            } else {
                return false;
            }
            itRef = m_blockPalette.end();
        } else {
            if (itRef != m_blockPalette.end()) {
                if (std::get<BlockState>(block) == itRef->first)
                    return false;
                itRef->second--;
                if (itRef->second == 0)
                    m_blockPalette.erase(itRef);
            }
            itRef = m_blockPalette.insert({std::get<BlockState>(block), 0}).first;
            itRef->second++;
        }
        return true;
    }

    void Structure::forEach(const Coordinate &from, const Coordinate &to,
                            const std::function<void(const Coordinate &)> &callback) {
        for (int x = from.x; x <= to.x; x += (from.x < to.x) - (from.x > to.x))
            for (int y = from.y; y <= to.y; y += (from.y < to.y) - (from.y > to.y))
                for (int z = from.z; z <= to.z; z += (from.z < to.z) - (from.z > to.z))
                    callback({x, y, z});
    }

    void Structure::forEach(const Coordinate &from, const Coordinate &to,
                            const std::function<void(const BlockType &, const Coordinate &)> &callback,
                            bool isSecondaryLayer) const {
        forEach(from, to, [=](const Coordinate &point) {
            callback(getBlock(point, isSecondaryLayer), point);
        });
    }

    bool Structure::existsInPalette(const BlockState &block) const {
        return m_blockPalette.contains(block);
    }

    std::optional<nbt::tag_compound> Structure::blockEntityData(const Coordinate &point) const {
        auto pointIndex = point.toIndex(m_size);
        assert(pointIndex >= 0 && pointIndex < m_size.volume());
        auto it = m_blockPositionData.find(pointIndex);
        if (it == m_blockPositionData.end())
            return std::nullopt;
        return it->second;
    }

    void Structure::setBlockEntityData(const Coordinate &point, const nbt::tag_compound &data) {
        auto pointIndex = point.toIndex(m_size);
        assert(pointIndex >= 0 && pointIndex < m_size.volume());
        auto [it, isInserted] = m_blockPositionData.insert({pointIndex, data});
        if (!isInserted)
            it->second = data;
    }

    bool Structure::existsBlockPositionData(const Coordinate &point) const {
        auto pointIndex = point.toIndex(m_size);
        assert(pointIndex >= 0 && pointIndex < m_size.volume());
        return m_blockPositionData.contains(pointIndex);
    }

    bool Structure::removeBlockPositionData(const Coordinate &point) {
        auto pointIndex = point.toIndex(m_size);
        assert(pointIndex >= 0 && pointIndex < m_size.volume());
        return m_blockPositionData.erase(pointIndex);
    }

    Size Structure::size() const {
        return m_size;
    }

    Coordinate Structure::worldOrigin() const {
        return m_worldOrigin;
    }

    void Structure::setWorldOrigin(const Coordinate &point) {
        m_worldOrigin = point;
    }

    Structure Structure::fromNBT(const nbt::tag_compound &data) {

        // size
        if (!data.has_key("size", nbt::tag_type::List))
            throw std::exception("Invalid tag: 'size'");
        auto nbtSizeList = data.at("size").as<nbt::tag_list>();
        if (nbtSizeList.size() != 3 || nbtSizeList.el_type() != nbt::tag_type::Int)
            throw std::exception("Invalid value type of list: 'size'");
        Structure structure({int(nbtSizeList[0]), int(nbtSizeList[1]), int(nbtSizeList[2])});

        //structure_world_origin
        {
            if (!data.has_key("structure_world_origin", nbt::tag_type::List))
                throw std::exception("Invalid tag: 'structure_world_origin'");
            auto nbtStructureWorldOriginList = data.at("structure_world_origin").as<nbt::tag_list>();
            if (nbtStructureWorldOriginList.size() != 3 || nbtStructureWorldOriginList.el_type() != nbt::tag_type::Int)
                throw std::exception("Invalid value type of list: 'structure_world_origin'");
            structure.setWorldOrigin({int(nbtStructureWorldOriginList[0]), int(nbtStructureWorldOriginList[1]), int(nbtStructureWorldOriginList[2])});
        }

        // structure
        if (!data.has_key("structure", nbt::tag_type::Compound))
            throw std::exception("Invalid tag: 'structure'");
        auto nbtStructureComp = data.at("structure").as<nbt::tag_compound>();

        // structure.palette
        if (!nbtStructureComp.has_key("palette", nbt::tag_type::Compound))
            throw std::exception("Invalid tag: 'palette'");
        auto nbtPaletteRootComp = nbtStructureComp.at("palette").as<nbt::tag_compound>();

        // structure.palette.default
        if (!nbtPaletteRootComp.has_key("default", nbt::tag_type::Compound))
            throw std::exception("Invalid tag: 'default'");
        auto nbtPaletteComp = nbtPaletteRootComp.at("default").as<nbt::tag_compound>();

        // structure.palette.default.block_palette
        std::vector<decltype(m_blockPalette)::iterator> itList;
        {
            if (!nbtPaletteComp.has_key("block_palette", nbt::tag_type::List))
                throw std::exception("Invalid tag: 'block_palette'");
            auto nbtBlockPaletteList = nbtPaletteComp.at("block_palette").as<nbt::tag_list>();
            for (const auto &nbtBlockStateValue: nbtBlockPaletteList) {
                if (nbtBlockStateValue.get_type() != nbt::tag_type::Compound && nbtBlockStateValue.get_type() != nbt::tag_type::Null)
                    throw std::exception("Invalid value type of list: 'block_palette'");
                auto block = BlockState::fromNBT(nbtBlockStateValue.as<nbt::tag_compound>());
                itList.push_back(structure.m_blockPalette.insert({block, 0}).first);
            }
        }

        // structure.block_indices
        {
            if (!nbtStructureComp.has_key("block_indices", nbt::tag_type::List))
                throw std::exception("Invalid tag: 'block_indices'");
            auto nbtBlockIndicesList = nbtStructureComp.at("block_indices").as<nbt::tag_list>();
            if (nbtBlockIndicesList.size() != 2 || nbtBlockIndicesList.el_type() != nbt::tag_type::List)
                throw std::exception("Invalid value type of list: 'block_indices'");
            {
                auto nbtPrimaryList = nbtBlockIndicesList[0].as<nbt::tag_list>();
                if (nbtPrimaryList.size() != structure.m_size.volume() ||
                        (nbtPrimaryList.el_type() != nbt::tag_type::Int && nbtPrimaryList.el_type() != nbt::tag_type::Null))
                    throw std::exception("Invalid value type of list: 'block_indices[0]'");
                for (int i = 0; i < nbtPrimaryList.size(); i++) {
                    auto index = int(nbtPrimaryList[i]);
                    if (index >= 0) {
                        structure.m_blockIndices[i] = itList[index];
                        itList[index]->second++;
                    } else {
                        structure.m_blockIndices[i] = structure.m_blockPalette.end();
                    }
                }
            }
            {
                auto nbtSecondaryList = nbtBlockIndicesList[1].as<nbt::tag_list>();
                if (nbtSecondaryList.size() != structure.m_size.volume() ||
                    nbtSecondaryList.el_type() != nbt::tag_type::Int)
                    throw std::exception("Invalid value type of list: 'block_indices[1]'");
                for (int i = 0; i < nbtSecondaryList.size(); i++) {
                    auto index = int(nbtSecondaryList[i]);
                    if (index >= 0) {
                        structure.m_secondaryBlockIndices[i] = itList[index];
                        itList[index]->second++;
                    } else {
                        structure.m_secondaryBlockIndices[i] = structure.m_blockPalette.end();
                    }
                }
            }
        }

        // structure.palette.default.block_position_data
        {
            if (!nbtPaletteComp.has_key("block_position_data", nbt::tag_type::Compound))
                throw std::exception("Invalid tag: 'block_position_data'");
            auto nbtBlockPositionDataComp = nbtPaletteComp.at("block_position_data").as<nbt::tag_compound>();
            for (auto &[indexStr, blockData]: nbtBlockPositionDataComp) {
                int index = std::stoi(indexStr);
                if (blockData.get_type() != nbt::tag_type::Compound)
                    continue;
                if (!blockData.as<nbt::tag_compound>().has_key("block_entity_data", nbt::tag_type::Compound))
                    continue;
                structure.m_blockPositionData[index] = blockData.at("block_entity_data").as<nbt::tag_compound>();

                // TODO tick_queue_data
            }
        }

        // structure.entities
        {
            if (!nbtStructureComp.has_key("entities", nbt::tag_type::List))
                throw std::exception("Invalid tag: 'entities'");
            auto nbtEntityList = nbtStructureComp.at("entities").as<nbt::tag_list>();
            if (nbtEntityList.el_type() != nbt::tag_type::Compound && nbtEntityList.el_type() != nbt::tag_type::Null)
                throw std::exception("Invalid value type of list: 'entities'");
            for (auto &entityValue: nbtEntityList) {
                structure.m_entities.push_back(entityValue.as<nbt::tag_compound>());
            }
        }

        return structure;
    }

    nbt::tag_compound Structure::toNBT() const {
        nbt::tag_compound nbtRootComp;
        nbtRootComp["format_version"] = 1;
        nbtRootComp["size"] = nbt::tag_list({m_size.x, m_size.y, m_size.z});
        nbtRootComp["structure_world_origin"] = nbt::tag_list({m_worldOrigin.x, m_worldOrigin.y, m_worldOrigin.z});
        nbt::tag_list nbtBlockIndicesList[2];
        for (auto blockIt : m_blockIndices) {
            int paletteIndex = 0;
            int valueToPush = -1;
            for (auto it = m_blockPalette.begin(); it != m_blockPalette.end(); paletteIndex++, it++) {
                if (blockIt == it) {
                    valueToPush = paletteIndex;
                    break;
                }
            }
            nbtBlockIndicesList[0].push_back(valueToPush);
        }
        for (auto blockIt : m_secondaryBlockIndices) {
            int paletteIndex = 0;
            int valueToPush = -1;
            for (auto it = m_blockPalette.begin(); it != m_blockPalette.end(); paletteIndex++, it++) {
                if (blockIt == it) {
                    valueToPush = paletteIndex;
                    break;
                }
            }
            nbtBlockIndicesList[1].push_back(valueToPush);
        }
        nbt::tag_list nbtEntityList;
        for (auto entity: m_entities) {
            nbtEntityList.push_back(nbt::value(std::move(entity)));
        }
        nbt::tag_list nbtBlockPaletteList;
        for (const auto &[block, _]: m_blockPalette) {
            nbtBlockPaletteList.push_back(nbt::value(block.toNBT()));
        }
        nbt::tag_compound nbtBlockPositionDataComp;
        for (auto [index, data]: m_blockPositionData) {
            nbtBlockPositionDataComp[std::to_string(index)] = nbt::tag_compound({{"block_entity_data", nbt::value(std::move(data))}});
        }
        nbtRootComp["structure"] = nbt::tag_compound({
            {"block_indices", nbt::tag_list({nbtBlockIndicesList[0], nbtBlockIndicesList[1]})},
            {"entities", std::move(nbtEntityList)},
            {"palette", nbt::tag_compound({
                {"default", nbt::tag_compound({
                    {"block_palette", std::move(nbtBlockPaletteList)},
                    {"block_position_data", std::move(nbtBlockPositionDataComp)}
                })}
            })}
        });
        return nbtRootComp;
    }
} // mcstructure