//
// Created by TaxMachine on 2024-05-21.
//

#include "class_loader.hpp"

#include <iostream>
#include <utility>

#include "obfusheader.h"

JVM create_java_vm() {
    const char* argv[] = {OBF("-Djava.compiler=NONE"),
                          OBF("-Djava.class.path=.")}; //"-verbose:jni"
    const int argc = static_cast<int>(sizeof(argv) / sizeof(argv[0]));

    JavaVMInitArgs jvm_args;
    JavaVMOption options[argc];

    for (int i = 0; i < argc; ++i) {
        options[i].optionString = const_cast<char*>(argv[i]);
    }

    JavaVM* vm = nullptr;
    JNIEnv* env = nullptr;
    JNI_GetDefaultJavaVMInitArgs(&jvm_args);

    jvm_args.version = JNI_VERSION_10;
    jvm_args.nOptions = argc;
    jvm_args.options = options;
    jvm_args.ignoreUnrecognized = false;

    if (JNI_CreateJavaVM(&vm, reinterpret_cast<void**>(&env), &jvm_args) != JNI_OK) {
        return {vm, env};
    }
    return {vm, env};
}

std::string string_replace_all(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
    }
    return str;
};

jobject byte_array_input_stream(JNIEnv* env, const std::vector<std::uint8_t> &buffer) {
    jbyteArray arr = env->NewByteArray(static_cast<jsize>(buffer.size()));
    if (!arr) return nullptr;
    env->SetByteArrayRegion(arr, 0, static_cast<jsize>(buffer.size()), reinterpret_cast<const jbyte*>(&buffer[0]));
    jclass cls = env->FindClass(OBF("java/io/ByteArrayInputStream"));
    if (cls) {
        jmethodID method = env->GetMethodID(cls, OBF("<init>"), OBF("([B)V"));
        if (method) {
            jobject result = env->NewObject(cls, method, arr);
            if (result) {
                env->DeleteLocalRef(std::exchange(result, env->NewGlobalRef(result)));
                env->DeleteLocalRef(cls);
                env->DeleteLocalRef(arr);
                return result;
            }
        }
        env->DeleteLocalRef(cls);
    }
    env->DeleteLocalRef(arr);
    return nullptr;
};

jobject jar_input_stream(JNIEnv* env, jobject input_stream) {
    jclass cls = env->FindClass(OBF("java/util/jar/JarInputStream"));
    if (cls) {
        jmethodID method = env->GetMethodID(cls, OBF("<init>"), OBF("(Ljava/io/InputStream;)V"));
        if (method) {
            jobject result = env->NewObject(cls, method, input_stream);
            if (result) {
                env->DeleteLocalRef(std::exchange(result, env->NewGlobalRef(result)));
                env->DeleteLocalRef(cls);
                return result;
            }
        }
        env->DeleteLocalRef(cls);
    }
    return nullptr;
};

jint input_stream_read(JNIEnv* env, jobject input_stream) {
    jclass cls = env->GetObjectClass(input_stream);
    if (cls) {
        jmethodID method = env->GetMethodID(cls, OBF("read"), OBF("()I"));
        if (method) {
            jint result = env->CallIntMethod(input_stream, method);
            env->DeleteLocalRef(cls);
            return result;
        }

        env->DeleteLocalRef(cls);
    }
    return -1;
};

jobject byte_array_output_stream(JNIEnv* env) {
    jclass cls = env->FindClass(OBF("java/io/ByteArrayOutputStream"));
    if (cls) {
        jmethodID method = env->GetMethodID(cls, OBF("<init>"), OBF("()V"));
        if (method) {
            jobject result = env->NewObject(cls, method);
            if (result) {
                env->DeleteLocalRef(std::exchange(result, env->NewGlobalRef(result)));
                env->DeleteLocalRef(cls);
                return result;
            }
        }
        env->DeleteLocalRef(cls);
    }
    return nullptr;
};

void output_stream_write(JNIEnv* env, jobject output_stream, jint value) {
    jclass cls = env->GetObjectClass(output_stream);
    if (cls) {
        jmethodID method = env->GetMethodID(cls, OBF("write"), OBF("(I)V"));
        if (method) {
            env->CallVoidMethod(output_stream, method, value);
        }
        env->DeleteLocalRef(cls);
    }
};

std::vector<std::uint8_t> byte_array_output_stream_to_byte_array(JNIEnv* env, jobject output_stream) {
    jclass cls = env->GetObjectClass(output_stream);
    if (cls) {
        jmethodID method = env->GetMethodID(cls, OBF("toByteArray"), OBF("()[B"));
        if (method) {
            auto bytes = reinterpret_cast<jbyteArray>(env->CallObjectMethod(output_stream, method));
            if (bytes) {
                void* arr = env->GetPrimitiveArrayCritical(bytes, nullptr);
                if (arr) {
                    jsize size = env->GetArrayLength(bytes);
                    auto result = std::vector<std::uint8_t>(static_cast<std::uint8_t*>(arr),
                                                            static_cast<std::uint8_t*>(arr) + size);
                    env->ReleasePrimitiveArrayCritical(bytes, arr, 0);
                    env->DeleteLocalRef(bytes);
                    env->DeleteLocalRef(cls);
                    return result;
                }
            }
        }
        env->DeleteLocalRef(cls);
    }
    return {};
};

jobject get_next_jar_entry(JNIEnv* env, jobject jar_input_stream) {
    jclass cls = env->GetObjectClass(jar_input_stream);
    if (cls) {
        jmethodID method = env->GetMethodID(cls, OBF("getNextJarEntry"), OBF("()Ljava/util/jar/JarEntry;"));
        if (method) {
            jobject result = env->CallObjectMethod(jar_input_stream, method);
            env->DeleteLocalRef(std::exchange(result, env->NewGlobalRef(result)));
            env->DeleteLocalRef(cls);
            return result;
        }
        env->DeleteLocalRef(cls);
    }
    return nullptr;
};

std::string jar_entry_get_name(JNIEnv* env, jobject jar_entry) {
    jclass cls = env->FindClass(OBF("java/util/jar/JarEntry"));
    if (cls) {
        jmethodID method = env->GetMethodID(cls, OBF("getName"), OBF("()Ljava/lang/String;"));
        if (method) {
            auto jstr = reinterpret_cast<jstring>(env->CallObjectMethod(jar_entry, method));
            const char *name_string = env->GetStringUTFChars(jstr, 0);
            std::string result = std::string(name_string);
            env->ReleaseStringUTFChars(jstr, name_string);
            env->DeleteLocalRef(jstr);
            env->DeleteLocalRef(cls);
            return result;
        }
        env->DeleteLocalRef(cls);
    }
    return {};
};

bool load_jar(JNIEnv* env, const std::vector<std::uint8_t> &jar_data, bool ignore_exceptions) {
    jobject bais = byte_array_input_stream(env, jar_data);
    if (!bais) {
        std::cerr<<"Failed to open ByteArrayInputStream\n";
        return false;
    }

    jobject jis = jar_input_stream(env, bais);
    if (!jis) {
        env->DeleteGlobalRef(bais);
        std::cerr<<"Failed to open JarInputStream\n";
        return false;
    }

    jobject jar_entry = nullptr;
    const std::string extension = OBF(".class");

    while ((jar_entry = get_next_jar_entry(env, jis))) {
        std::string name = jar_entry_get_name(env, jar_entry);
        if ((name.length() > extension.length()) && (name.rfind(extension) == name.length() - extension.length())) {
            std::cout << "Loading: " << name << std::endl;
            jobject baos = byte_array_output_stream(env);
            if (!baos) {
                if (!ignore_exceptions) {
                    env->DeleteGlobalRef(jar_entry);
                    env->DeleteGlobalRef(jis);
                    env->DeleteGlobalRef(bais);
                    return false;
                }
            }

            jint value = -1;
            while ((value = input_stream_read(env, jis)) != -1) {
                output_stream_write(env, baos, value);
            }

            std::vector<std::uint8_t> bytes = byte_array_output_stream_to_byte_array(env, baos);
            std::cout << "Loaded: " << name << std::endl;
            printf("Size: %zu\n", bytes.size());
            printf("First bytes: %d %d %d %d\n", bytes[0], bytes[1], bytes[2], bytes[3]);

            std::string canonicalName = string_replace_all(string_replace_all(name, OBF("/"), OBF(".")),
                                                           OBF(".class"),
                                                           std::string());

            jclass cls = env->DefineClass(string_replace_all(name, OBF(".class"), OBF("")).c_str(),
                                          nullptr, reinterpret_cast<jbyte*>(bytes.data()),
                                          static_cast<jint>(bytes.size()));

            if (cls) {
                jmethodID init = env->GetMethodID(cls, OBF("<init>"), OBF("()V"));
                if (init) {
                    env->CallStaticVoidMethod(cls, init);
                }
                env->DeleteLocalRef(cls);
            } else {
                if (env->ExceptionCheck()) {
                    env->ExceptionDescribe();
                    env->ExceptionClear();

                    if (!ignore_exceptions) {
                        env->DeleteGlobalRef(jar_entry);
                        env->DeleteGlobalRef(jis);
                        env->DeleteGlobalRef(bais);
                        return false;
                    }
                }
            }
        }

        env->DeleteGlobalRef(jar_entry);
    }
    env->DeleteGlobalRef(jis);
    env->DeleteGlobalRef(bais);
    return true;
}