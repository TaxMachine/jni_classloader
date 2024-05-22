//
// Created by TaxMachine on 2024-05-21.
//

#ifndef RPC_CLASS_LOADER_HPP
#define RPC_CLASS_LOADER_HPP

#include <jni.h>
#include <vector>
#include <cstdint>

struct JVM {
    JavaVM* vm;
    JNIEnv* env;
};

JVM create_java_vm();
bool load_jar(JNIEnv* env, const std::vector<std::uint8_t> &jar_data, bool ignore_exceptions = false);

#endif //RPC_CLASS_LOADER_HPP
