#pragma once

#include <random>
#include <vector>
#include <map>
#include <set>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <iterator>

class SlotTools {
public:
    // ======== RNG ========

    // Returns a random integer in [0, limit).
    static int RandInt(int limit) {
        static thread_local std::mt19937 rng{ Seed32() };
        std::uniform_int_distribution<int> dist(0, limit - 1);
        return dist(rng);
    }

    // Returns a random int64_t in [0, limit).
    static int64_t RandLongLong(int64_t limit) {
        static thread_local std::mt19937_64 rng64{ Seed64() };
        std::uniform_int_distribution<int64_t> dist(0, limit - 1);
        return dist(rng64);
    }

    // Returns a random double in [0.0, limit).
    static double RandDouble(double limit) {
        static thread_local std::mt19937 rng{ Seed32() };
        std::uniform_real_distribution<double> dist(0.0, limit);
        return dist(rng);
    }

    // Selects an index from weights proportionally (int weights).
    static int WeightedIndex(const std::vector<int>& weights) {
        static thread_local std::mt19937 rng{ Seed32() };
        int total = std::accumulate(weights.begin(), weights.end(), 0);
        if (total <= 0) return -1;
        std::uniform_int_distribution<int> dist(0, total - 1);
        int rnd = dist(rng);
        int sum = 0;
        for (int i = 0; i < weights.size(); ++i) {
            sum += weights[i];
            if (rnd < sum) return i;
        }
        return -1;
    }

    // Selects an index from weights proportionally (long long weights).
    static int WeightedIndex(const std::vector<long long>& weights) {
        static thread_local std::mt19937_64 rng64{ Seed64() };
        long long total = std::accumulate(weights.begin(), weights.end(), 0LL);
        if (total <= 0) return -1;
        std::uniform_int_distribution<long long> dist(0, total - 1);
        long long rnd = dist(rng64);
        long long sum = 0;
        for (int i = 0; i < weights.size(); ++i) {
            sum += weights[i];
            if (rnd < sum) return i;
        }
        return -1;
    }

    // ======== Vector & Shuffling ========

    // Shuffles the contents of the vector randomly.
    template <typename T>
    static void Shuffle(std::vector<T>& vec) {
        static thread_local std::mt19937 rng{ Seed32() };
        std::shuffle(vec.begin(), vec.end(), rng);
    }

    // Randomly selects n unique elements from the vector.
    template <typename T>
    static std::vector<T> Choose(std::vector<T> vec, int n) {
        static thread_local std::mt19937 rng{ Seed32() };
        int size = vec.size();
        if (n > size) n = size;
        for (int i = 0; i < n; ++i) {
            std::uniform_int_distribution<int> dist(i, size - 1);
            std::swap(vec[i], vec[dist(rng)]);
        }
        return std::vector<T>(vec.begin(), vec.begin() + n);
    }

    // Returns a random element from the vector without removing it.
    template <typename T>
    static const T& RandomElement(const std::vector<T>& vec) {
        static thread_local std::mt19937 rng{ Seed32() };
        std::uniform_int_distribution<int> dist(0, static_cast<int>(vec.size()) - 1);
        return vec[dist(rng)];
    }

    // Removes and returns a random element from the vector.
    template <typename T>
    static T PopRandomElement(std::vector<T>& vec) {
        static thread_local std::mt19937 rng{ Seed32() };
        std::uniform_int_distribution<int> dist(0, static_cast<int>(vec.size()) - 1);
        int index = dist(rng);
        T val = vec[index];
        vec.erase(vec.begin() + index);
        return val;
    }

    // ======== Math & Analysis ========

    // Returns the sum of all elements in the vector.
    template <typename T>
    static T Sum(const std::vector<T>& vec) {
        return std::accumulate(vec.begin(), vec.end(), T{});
    }

    // Returns the product of all elements in the vector.
    template <typename T>
    static T Product(const std::vector<T>& vec) {
        return std::accumulate(vec.begin(), vec.end(), T{ 1 }, std::multiplies<T>());
    }

    // Returns the minimum value in the vector.
    template <typename T>
    static T Min(const std::vector<T>& vec) {
        return *std::min_element(vec.begin(), vec.end());
    }

    // Returns the maximum value in the vector.
    template <typename T>
    static T Max(const std::vector<T>& vec) {
        return *std::max_element(vec.begin(), vec.end());
    }

    // Clamps value between minVal and maxVal.
    template <typename T>
    static T Clamp(const T& value, const T& minVal, const T& maxVal) {
        if (value < minVal) return minVal;
        if (value > maxVal) return maxVal;
        return value;
    }

    // Returns the median value from a histogram map.
    static int Median(const std::map<int, int>& hist) {
        int total = 0;
        for (const auto& [value, count] : hist) total += count;
        int medianIndex = (total + 1) / 2;
        total = 0;
        for (const auto& [value, count] : hist) {
            total += count;
            if (total >= medianIndex) return value;
        }
        return -1;
    }

    // Returns the mean (average) value from a histogram map.
    static double Mean(const std::map<int, int>& hist) {
        int totalCount = 0;
        long long weightedSum = 0;
        for (const auto& [value, count] : hist) {
            weightedSum += static_cast<long long>(value) * count;
            totalCount += count;
        }
        return totalCount == 0 ? 0.0 : static_cast<double>(weightedSum) / totalCount;
    }

    // Returns the most frequently occurring value in the vector.
    static int Mode(const std::vector<int>& vec) {
        std::map<int, int> freq;
        for (int v : vec) freq[v]++;
        return std::max_element(freq.begin(), freq.end(),
            [](auto& a, auto& b) { return a.second < b.second; })->first;
    }

    // Returns how many times value occurs in the vector.
    template <typename T>
    static int CountOccurrences(const std::vector<T>& vec, const T& value) {
        return std::count(vec.begin(), vec.end(), value);
    }

    // Returns a frequency map of all element counts in the vector.
    template <typename T>
    static std::map<T, int> FrequencyMap(const std::vector<T>& vec) {
        std::map<T, int> freq;
        for (const auto& val : vec) freq[val]++;
        return freq;
    }

    // Returns true if all elements in the vector are equal.
    template <typename T>
    static bool AllEqual(const std::vector<T>& vec) {
        return std::adjacent_find(vec.begin(), vec.end(), std::not_equal_to<>()) == vec.end();
    }

    // ======== Base Conversion ========

    // Converts an integer to a vector of base digits (optionally reversed and padded).
    static std::vector<int> ToBase(int input, int base, int length = -1, bool reversed = false) {
        int quot = std::abs(input);
        int rem = 0;
        if (length <= 0)
            length = (quot == 0) ? 1 : static_cast<int>(std::log(quot) / std::log(base)) + 1;
        std::vector<int> baseVec(length, 0);
        for (int i = 0; i < length; ++i) {
            rem = quot % base;
            quot /= base;
            baseVec[reversed ? length - 1 - i : i] = rem;
        }
        return baseVec;
    }

    // Converts a base digit vector back to an integer.
    static int FromBase(const std::vector<int>& digits, int base, bool reversed = false) {
        int value = 0;
        int power = 1;
        if (reversed) {
            for (int i = 0; i < digits.size(); ++i) {
                value += digits[i] * power;
                power *= base;
            }
        }
        else {
            for (int i = digits.size() - 1; i >= 0; --i) {
                value += digits[i] * power;
                power *= base;
            }
        }
        return value;
    }

    // ======== Set Utilities ========
 
    // Returns the set intersection of a and b.
    template <typename T>
    static std::set<T> SetIntersect(const std::set<T>& a, const std::set<T>& b) {
        std::set<T> result;
        std::set_intersection(a.begin(), a.end(), b.begin(), b.end(),
            std::inserter(result, result.begin()));
        return result;
    }

    // Returns the set union of a and b.
    template <typename T>
    static std::set<T> SetUnion(const std::set<T>& a, const std::set<T>& b) {
        std::set<T> result;
        std::set_union(a.begin(), a.end(), b.begin(), b.end(),
            std::inserter(result, result.begin()));
        return result;
    }

    // Returns true if a is a subset of b.
    template <typename T>
    static bool IsSubset(const std::set<T>& a, const std::set<T>& b) {
        return std::includes(b.begin(), b.end(), a.begin(), a.end());
    }

private:
    static uint32_t Seed32() {
        std::random_device rd;
        return rd();
    }

    static uint64_t Seed64() {
        std::random_device rd;
        return (uint64_t(rd()) << 32) | rd();
    }
};
