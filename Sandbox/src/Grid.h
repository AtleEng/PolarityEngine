#pragma once
#include <unordered_map>
#include <array>
#include <cmath>

template<typename T, int CHUNK_SIZE = 16>
class Grid {
public:
    // -------- Coordinate Types --------
    struct ChunkCoord {
        int x, y;
        bool operator==(const ChunkCoord& o) const { return x == o.x && y == o.y; }
    };

    struct ChunkCoordHash {
        std::size_t operator()(const ChunkCoord& c) const {
            // Hash 2 ints into 1
            return std::hash<long long>()(((long long)c.x << 32) ^ (long long)c.y);
        }
    };

    struct Chunk {
        std::array<std::array<T, CHUNK_SIZE>, CHUNK_SIZE> cells{};
        bool initialized = false;  // Optional! to detect whether chunk is used
    };

    // -------- API --------

    // Set a cell (auto-creates chunk)
    void set(int x, int y, const T& value) {
        auto [cc, lx, ly] = getChunkAndLocal(x, y);
        Chunk& chunk = chunks[cc];
        chunk.initialized = true;
        chunk.cells[lx][ly] = value;
    }

    // Get a cell (returns defaultValue if chunk not found)
    T get(int x, int y, const T& defaultValue = T()) const {
        auto [cc, lx, ly] = getChunkAndLocal(x, y);

        auto it = chunks.find(cc);
        if (it == chunks.end()) return defaultValue;

        const Chunk& chunk = it->second;
        return chunk.cells[lx][ly];
    }

    // Check if a cell exists
    bool has(int x, int y) const {
        auto [cc, lx, ly] = getChunkAndLocal(x, y);

        auto it = chunks.find(cc);
        return it != chunks.end();
    }

    // Remove a chunk entirely
    void removeChunk(int chunkX, int chunkY) {
        chunks.erase(ChunkCoord{ chunkX, chunkY });
    }

    // Clear all data
    void clear() {
        chunks.clear();
    }

private:
    std::unordered_map<ChunkCoord, Chunk, ChunkCoordHash> chunks;

    // -------- Coordinate Conversion --------
    static int floorDiv(int a, int b) {
        return (a >= 0) ? (a / b) : ((a - b + 1) / b);
    }

    struct ChunkLookup {
        ChunkCoord cc;
        int lx, ly;
    };

    // Convert world coords to chunk + local coordinates
    static ChunkLookup getChunkAndLocal(int x, int y) {
        int cx = floorDiv(x, CHUNK_SIZE);
        int cy = floorDiv(y, CHUNK_SIZE);

        int lx = (x % CHUNK_SIZE + CHUNK_SIZE) % CHUNK_SIZE;
        int ly = (y % CHUNK_SIZE + CHUNK_SIZE) % CHUNK_SIZE;

        return { ChunkCoord{cx, cy}, lx, ly };
    }
};