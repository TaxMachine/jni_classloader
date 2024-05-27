#include <iostream>

#include "obfusheader.h"
#include "stub.hpp"
#include "class_loader.hpp"

extern "C" jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    xor_decrypt(jar, sizeof(jar), key, sizeof(key));

    JNIEnv *env;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_10) != JNI_OK) {
        return OBF(1);
    }

    std::vector<std::uint8_t> some_jar_as_bytes(jar, jar + sizeof(jar));
    const char *method_name = OBF("main");
    const char *method_signature = OBF("([Ljava/lang/String;)V");

    if (!load_jar(env, some_jar_as_bytes, method_name, method_signature)) {
        return OBF(1);
    }
    return OBF(0);
}

/*
int main() {
    xor_decrypt(jar, sizeof(jar), key, sizeof(key));

    JVM jvm = create_java_vm();
    if (!jvm.vm) {
        return OBF(0);
    }

    std::vector<std::uint8_t> some_jar_as_bytes(jar, jar + sizeof(jar));
    const char* method_name = OBF("main");
    const char* method_signature = OBF("([Ljava/lang/String;)V");
    try {
        if (!load_jar(jvm.env, some_jar_as_bytes, method_name, method_signature)) {
            return OBF(1);
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return OBF(1);
    }

    jvm.vm->DestroyJavaVM();
    return OBF(0);
}*/