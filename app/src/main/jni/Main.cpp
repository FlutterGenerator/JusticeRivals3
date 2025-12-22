#include <list>
#include <vector>
#include <string.h>
#include <pthread.h>
#include <cstring>
#include <jni.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <dlfcn.h>
#include "Includes/Logger.h"
#include "Includes/obfuscate.h"
#include "Includes/Utils.h"

#include "KittyMemory/MemoryPatch.h"
//#include "And64InlineHook/And64InlineHook.hpp"
#include "Includes/Chams.h"
#include "Menu.h"

//Target lib here
#define targetLibName OBFUSCATE("libil2cpp.so")

#include "Includes/Macros.h"

struct My_Patches {
 MemoryPatch Unliammo, UnliMONEY, InfinyMAGAZINE;
} hexPatches;

bool Unliammo, UnliMONEY, InfinyMAGAZINE, chams, shading, wireframe, glow, outline, rainbow = false;

/*bool Ismod = false;

bool (*old_mod)(void *instance);
bool mod(void *instance) {
    if (instance != NULL && Ismod) {
          return false;
    }
    return old_mod(instance);
}*/

void *hack_thread(void *) {
    ProcMap il2cppMap;
    do {
        il2cppMap = KittyMemory::getLibraryMap(targetLibName);
        sleep(1);
    } while (!il2cppMap.isValid() && mlovinit());
    setShader("_MainTex");
    LogShaders();
    Wallhack();
    
    //Check if target lib is loaded
    do {
        sleep(1);
    } while (!isLibraryLoaded(targetLibName));
    
    LOGI(OBFUSCATE("%s has been loaded"), (const char *) targetLibName);
    
#if defined(__aarch64__)

   // A64HookFunction((void *)getAbsoluteAddress("libil2cpp.so", Hook), (void *)mod, (void **) &old_mod);
    
    LOGI(OBFUSCATE("Done"));
#endif

    hexPatches.Unliammo = MemoryPatch::createWithHex("libil2cpp.so", 0x11B3164, "00000CE3790444E31EFF2FE1");
    hexPatches.UnliMONEY = MemoryPatch::createWithHex("libil2cpp.so", 0x11A2BCC, "0201E0E31EFF2FE1");
    hexPatches.InfinyMAGAZINE = MemoryPatch::createWithHex("libil2cpp.so", 0x11B3B90, "E70300E31EFF2FE1");
    
    return NULL;
}

extern "C" {

JNIEXPORT jobjectArray
JNICALL
Java_uk_lgl_modmenu_FloatingModMenuService_getFeatureList(JNIEnv *env, jobject context) {
    jobjectArray ret;

    //Toasts added here so it's harder to remove it
    MakeToast(env, context, OBFUSCATE("Modded by tojik_proof_93"), Toast::LENGTH_LONG);

    const char *features[] = {
            OBFUSCATE("Category_All Mode"), //Not counted
        //  OBFUSCATE("1_Toggle_The Toggle"), //1 Case
            OBFUSCATE("1_ButtonOnOff_UNLIMITED AMMO"), //1 Case
            OBFUSCATE("2_ButtonOnOff_UNLIMITED MONEY"), //2 Case
            OBFUSCATE("3_ButtonOnOff_INFINY MAGAZINE"), //3 Case
            OBFUSCATE("Category_Chams Menu"), //Not Counted
            OBFUSCATE("11_ButtonOnOff_Default Chams"), //11 Case
            OBFUSCATE("12_ButtonOnOff_Wireframe Chams"), //12 Case
            OBFUSCATE("13_ButtonOnOff_Glow Chams"), //13 Case
            OBFUSCATE("14_ButtonOnOff_Outline Chams"), //14 Case
            OBFUSCATE("15_ButtonOnOff_Rainbow Chams"), //15 Case
    };

    //Now you dont have to manually update the number everytime;
    int Total_Feature = (sizeof features / sizeof features[0]);
    ret = (jobjectArray)
            env->NewObjectArray(Total_Feature, env->FindClass(OBFUSCATE("java/lang/String")),
                                env->NewStringUTF(""));

    for (int i = 0; i < Total_Feature; i++)
        env->SetObjectArrayElement(ret, i, env->NewStringUTF(features[i]));

    pthread_t ptid;
    pthread_create(&ptid, NULL, antiLeech, NULL);

    return (ret);
}

JNIEXPORT void JNICALL
Java_uk_lgl_modmenu_Preferences_Changes(JNIEnv *env, jclass clazz, jobject obj,
                                        jint featNum, jstring featName, jint value,
                                        jboolean boolean, jstring str) {

    LOGD(OBFUSCATE("Feature name: %d - %s | Value: = %d | Bool: = %d | Text: = %s"), featNum,
         env->GetStringUTFChars(featName, 0), value,
         boolean, str != NULL ? env->GetStringUTFChars(str, 0) : "");

    //BE CAREFUL NOT TO ACCIDENTLY REMOVE break;

    switch (featNum) {
 /* case 1:
    Ismod = boolean;
    break;*/
    case 1:
    Unliammo = boolean;
    if (Unliammo) {
        hexPatches.Unliammo.Modify();
    } else {
        hexPatches.Unliammo.Restore();
    } break;
    case 2:
    UnliMONEY = boolean;
    if (UnliMONEY) {
        hexPatches.UnliMONEY.Modify();
    } else {
        hexPatches.UnliMONEY.Restore();
    } break;
    case 3:
    InfinyMAGAZINE = boolean;
    if (InfinyMAGAZINE) {
        hexPatches.InfinyMAGAZINE.Modify();
    } else {
        hexPatches.InfinyMAGAZINE.Restore();
    } break;
    case 11:
    SetWallhack(boolean);
      break;
    case 12:
    SetWallhackW(boolean);
      break;
    case 13:
    SetWallhackG(boolean);
      break;
    case 14:
    SetWallhackO(boolean);
      break;
    case 15:
    SetRainbow(boolean);
      break;
    }
}
}

//No need to use JNI_OnLoad, since we don't use JNIEnv
//We do this to hide OnLoad from disassembler
__attribute__((constructor))
void lib_main() {
    // Create a new thread so it does not block the main thread, means the game would not freeze
    pthread_t ptid;
    pthread_create(&ptid, NULL, hack_thread, NULL);
}

/*
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *globalEnv;
    vm->GetEnv((void **) &globalEnv, JNI_VERSION_1_6);
    return JNI_VERSION_1_6;
}
 */