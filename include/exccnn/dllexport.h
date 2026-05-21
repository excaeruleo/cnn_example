/**
 * @file dllexport.h
 * @author Derek Huang
 * @brief C/C++ header for exccnn symbol import/export visibility
 * @copyright MIT License
 */

#ifndef EXCCNN_DLLEXPORT_H_
#define EXCCNN_DLLEXPORT_H_

// if using as shared EXCCNN_DLL should be defined and if building as shared
// then EXCCNN_BUILD_DLL should be defined. for convenience, EXCCNN_BUILD_DLL
// implies EXCCNN_DLL if it has not already been defined
#if !defined(EXCCNN_DLL) && defined(EXCCNN_BUILD_DLL)
#define EXCCNN_DLL
#endif  // !defined(EXCCNN_DLL) && defined(EXCCNN_BUILD_DLL)

// if building shared
#if defined(EXCCNN_BUILD_DLL)
// for MSVC use __declspec(dllexport)
#if defined(_MSC_VER)
#define EXCCNN_PUBLIC __declspec(dllexport)
// for GCC/Clang use __attribute__((visibility("default")))
#elif defined(__GNUC__)
#define EXCCNN_PUBLIC __attribute__((visibility("default")))
#endif  // !defined(_MSC_VER) && !defined(__GNUC__)
#elif defined(EXCCNN_DLL)
// for MSVC use __declspec(dllimport)
#if defined(_MSC_VER)
#define EXCCNN_PUBLIC __declspec(dllimport)
// for GCC/Clang use __attribute__((visibility("default")))
#elif defined(__GNUC__)
#define EXCCNN_PUBLIC __attribute__((visibility("default")))
#endif  // !defined(_MSC_VER) && !defined(__GNUC__)
#endif  // !defined(EXCCNN_BUILD_DLL) && !defined(EXCCNN_DLL)

// otherwise define as empty
#ifndef EXCCNN_PUBLIC
#define EXCCNN_PUBLIC
#endif  // EXCCNN_PUBLIC

#endif  // EXCCNN_DLLEXPORT_H_
