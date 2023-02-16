#pragma once

// Interface of snapshots.
class ISnapshot {
    public:
        virtual void update(int v, int id) = 0;
        virtual int *scan() = 0;
        virtual ~ISnapshot() {}
};
