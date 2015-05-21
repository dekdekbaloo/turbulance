#ifndef COORD_H_INCLUDED
#define COORD_H_INCLUDED

struct coord {
    public:
    float x, y, z;

    bool operator==(const coord &o) const {
        return x == o.x && y == o.y && z == o.z;
    }

    bool operator<(const coord &o) const {
        return x < o.x || (x == o.x && y < o.y) || (x == o.x && y == o.y && z < o.z);
    }
    coord(float a, float b, float c) : x(a) , y(b) , z(c) {}
};

#endif // COORD_H_INCLUDED
