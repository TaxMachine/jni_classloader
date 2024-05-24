#include <iostream>

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
    if (!jvm.vm) {
        return OBF(0);
    }

    std::vector<std::uint8_t> some_jar_as_bytes(jar, jar + sizeof(jar));
    const char* method_name = OBF("main");
    const char* method_signature = OBF("([Ljava/lang/String;)V");

    if (!load_jar(jvm.env, some_jar_as_bytes, method_name, method_signature)) {
        return OBF(1);
    }

    jvm.vm->DestroyJavaVM();
    return OBF(0);
}