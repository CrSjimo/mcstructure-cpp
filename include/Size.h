#ifndef MCSTRUCTURE_SIZE_H
#define MCSTRUCTURE_SIZE_H

namespace mcstructure {

    struct Size {
        Size(int x, int y, int z) : x(x), y(x), z(z) {
        }

        int x;
        int y;
        int z;
    };

} // mcstructure

#endif //MCSTRUCTURE_SIZE_H
