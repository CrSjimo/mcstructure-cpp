#include "BlockState.h"

#include <algorithm>
#include <utility>

#include <tag_string.h>

namespace mcstructure {

    BlockState::BlockState(std::string_view name, std::initializer_list<std::pair<const std::string, Value>> states,
                           int version) : m_name(name), m_version(version), m_states(states) {
    }

    std::string BlockState::name() const {
        return m_name;
    }

    int BlockState::version() const {
        return m_version;
    }

    BlockState::Value BlockState::value(const std::string &key) const {
        return m_states.at(key);
    }

    nbt::tag_compound BlockState::toNBT() const {
        nbt::tag_compound states;
        for (const auto &[key, value]: *this) {
            switch (value.type()) {
                case BlockState::Value::Boolean:
                    states.insert(key, static_cast<char>(value.get<bool>()));
                    break;
                case BlockState::Value::Integer:
                    states.insert(key, value.get<int>());
                    break;
                case BlockState::Value::String:
                    states.insert(key, value.get<std::string>());
                    break;
            }
        }
        return nbt::tag_compound({
                                         {"name", m_name},
                                         {"states", std::move(states)},
                                         {"version", m_version}
                                 });
    }

    bool BlockState::contains(const std::string &key) const {
        return m_states.contains(key);
    }

    BlockState BlockState::fromNBT(const nbt::tag_compound &data) {
        if (!data.has_key("name", nbt::tag_type::String))
            throw std::exception("Invalid tag: name");
        std::string name = data.at("name").as<nbt::tag_string>().get();
        if (!data.has_key("states", nbt::tag_type::Compound))
            throw std::exception("Invalid tag: states");
        std::map<std::string, Value> states;
        auto nbtStates = data.at("states").as<nbt::tag_compound>();
        for (const auto &[key, nbtValue]: nbtStates) {
            switch (nbtValue.get_type()) {
                case nbt::tag_type::Byte:
                    states.insert({key, static_cast<bool>(int8_t(nbtValue))});
                    break;
                case nbt::tag_type::Int:
                    states.insert({key, int(nbtValue)});
                    break;
                case nbt::tag_type::String:
                    states.insert({key, nbtValue.as<nbt::tag_string>().get()});
                    break;
                default:
                    throw std::exception("Invalid tag type in states");
            }
        }
        if (!data.has_key("version", nbt::tag_type::Int))
            throw std::exception("Invalid tag: version");
        auto version = int(data.at("version"));
        return BlockState(name, std::move(states), version);
    }

} // mcstructure