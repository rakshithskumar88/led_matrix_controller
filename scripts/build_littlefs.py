Import("env")
import os

def before_build(source, target, env):
    print("Building LittleFS image...")
    env.Execute("$BUILD_DIR/mklittlefs -c $PROJECT_DIR/data -p 256 -b 8192 -s 1048576 $BUILD_DIR/littlefs.bin")
    print("LittleFS image built successfully")

env.AddPreAction("buildprog", before_build)
