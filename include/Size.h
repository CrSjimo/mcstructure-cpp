#ifndef MCSTRUCTURE_SIZE_H
#define MCSTRUCTURE_SIZE_H

namespace mcstructure {

    struct Size {
        Size(int x, int y, int z) : x(x), y(x), z(z) {
        }

        int x;
        int y;
        int z;

        [[nodiscard]] inline int volume() const {
            return x * y * z;
        }
    };

} // mcstructure

#endif //MCSTRUCTURE_SIZE_H
