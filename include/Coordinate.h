#ifndef MCSTRUCTURE_COORDINATE_H
#define MCSTRUCTURE_COORDINATE_H

#include "Size.h"

namespace mcstructure {

    struct Coordinate {
        Coordinate(int x, int y, int z) : x(x), y(y), z(z) {
        }

        explicit Coordinate(int index, const Size &size) : x(index /size.z / size.y), y(index / size.z % size.y), z(index % size.z) {
        }

        [[nodiscard]] int toIndex(const Size &size) const {
            return size.z * size.y * x + size.z * y + z;
        }

        int x;
        int y;
        int z;
    };

} // mcstructure

#endif //MCSTRUCTURE_COORDINATE_H
