#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>

#include "obfusheader.h"
#include "stub.hpp"
#include "class_loader.hpp"

/*
extern "C" JNIEXPORT void JNICALL dev_taxmachine_testrpc_Main_init_rpc(JNIEnv *env, jobject obj) {
    const char* text = OBF("Hello from C++!");
    DYNAMIC_HIDE_CALL(&printf, text);
}*/

int main() {
    xor_decrypt(jar, sizeof(jar), key, sizeof(key));

    JVM jvm = create_java_vm();
    if (!jvm.vm)
    {
        std::cerr<<"Failed to Create JavaVM\n";
        return 0;
    }

    try {

        std::vector<std::uint8_t> some_jar_as_bytes(jar, jar + sizeof(jar));

        if (load_jar(jvm.env, some_jar_as_bytes, false)) {
            std::cout << "Jar loaded successfully\n";
        } else {
            std::cerr << "Failed to load Jar\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    jvm.vm->DestroyJavaVM();
    return 0;
}