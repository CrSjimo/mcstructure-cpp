#ifndef MCSTRUCTURE_BLOCKSTATE_H
#define MCSTRUCTURE_BLOCKSTATE_H

#include <cassert>
#include <string>
#include <vector>
#include <variant>
#include <map>
#include <memory>

#include <tag_compound.h>

namespace mcstructure {

    class BlockState {
    public:
        class Value {
        public:
            enum Type {
                Boolean,
                Integer,
                String,
            };
            Value(bool b);
            Value(int i);
            Value(std::string s);


            bool operator==(const Value &other) const;

            [[nodiscard]] Type type() const;

            template<typename T>
            T get() const {
                static_assert(std::is_same<T, int>::value || std::is_same<T, bool>::value || std::is_same<T, std::string>::value, "Unsupported block state value type");
            }

            template<>
            [[nodiscard]] bool get<bool>() const {
                return std::get<bool>(p);
            }

            template<>
            [[nodiscard]] int get<int>() const {
                return std::get<int>(p);
            }

            template<>
            [[nodiscard]] std::string get<std::string>() const {
                return std::get<std::string>(p);
            }

        private:
            std::variant<bool, int, std::string> p;
        };

        explicit BlockState(std::string_view name, std::initializer_list<std::pair<const std::string, Value>> states = {}, int version = COMPATIBILITY_VERSION);

        template<typename Iterator>
        explicit BlockState(std::string_view name, Iterator first, Iterator last, int version = COMPATIBILITY_VERSION) : m_name(name), m_version(version), m_states(first, last) {
        }

        [[nodiscard]] std::string name() const;

        [[nodiscard]] int version() const;

        [[nodiscard]] Value value(const std::string &key) const;

        using const_iterator = std::map<std::string, Value>::const_iterator;
        using const_reverse_iterator = std::map<std::string, Value>::const_reverse_iterator;
        [[nodiscard]] const_iterator begin() const { return m_states.begin(); }
        [[nodiscard]] const_iterator end() const { return m_states.end(); }
        [[nodiscard]] const_iterator cbegin() const { return m_states.cbegin(); }
        [[nodiscard]] const_iterator cend() const { return m_states.cend(); }
        [[nodiscard]] const_reverse_iterator rbegin() const { return m_states.rbegin(); }
        [[nodiscard]] const_reverse_iterator rend() const { return m_states.rend(); }
        [[nodiscard]] const_reverse_iterator crbegin() const { return m_states.crbegin(); }
        [[nodiscard]] const_reverse_iterator crend() const { return m_states.crend(); }

        [[nodiscard]] std::unique_ptr<nbt::tag_compound> toNBT() const;
        static BlockState fromNBT(const nbt::tag_compound &data);

        bool operator==(const BlockState &other) const {
            return m_name == other.m_name && m_version == other.m_version && m_states == other.m_states;
        }

        static const int COMPATIBILITY_VERSION = 17959425;

    private:
        explicit BlockState(std::string_view name, std::map<std::string, Value> &&states, int version) : m_name(name), m_version(version), m_states(states) {
        }

        std::string m_name;
        std::map<std::string, Value> m_states;
        int m_version;
    };

} // mcstructure

#endif //MCSTRUCTURE_BLOCKSTATE_H
