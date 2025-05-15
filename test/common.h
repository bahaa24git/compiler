// filepath: /mnt/driveE/D/FCAI-H/3rd Junior/2nd term/Compilers/project/test/common.h
//
// Created by USER on 5/14/2025.
// Modified for functional programming on 5/15/2025.
//

#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <functional>

using namespace std;

// Immutable token structure
struct Token
{
    int lineNumber;
    string text;
    string type;
};

// Result type for operations that may fail
template <typename T>
struct Result
{
    bool success;
    T value;
    string errorMessage;

    static Result<T> ok(const T &val)
    {
        return {true, val, ""};
    }

    static Result<T> err(const string &message)
    {
        return {false, T{}, message};
    }

    // Functional composition for results
    template <typename U>
    Result<U> then(function<Result<U>(const T &)> fn) const
    {
        if (success)
        {
            return fn(value);
        }
        return Result<U>{false, U{}, errorMessage};
    }

    // Map operation for transforming results
    template <typename U>
    Result<U> map(function<U(const T &)> fn) const
    {
        if (success)
        {
            return Result<U>::ok(fn(value));
        }
        return Result<U>{false, U{}, errorMessage};
    }
};

// Type alias for a collection of tokens
using TokenCollection = vector<Token>;

// Type for parse results with additional diagnostics
struct ParseResult
{
    bool success;
    string rule;
    int errorCount;
    vector<string> errors;
};

#endif // COMMON_H
