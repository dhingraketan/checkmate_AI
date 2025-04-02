#!/bin/bash

# Setup environment for Zephyr SDK
export ZEPHYR_SDK_INSTALL_DIR="$HOME/zephyrproject/zephyr-sdk-0.17.0"
export ZEPHYR_TOOLCHAIN_VARIANT=zephyr
export CMAKE_PREFIX_PATH="$ZEPHYR_SDK_INSTALL_DIR/cmake"

echo "✅ Zephyr SDK Environment Configured:"
echo "  ZEPHYR_SDK_INSTALL_DIR = $ZEPHYR_SDK_INSTALL_DIR"
echo "  ZEPHYR_TOOLCHAIN_VARIANT = $ZEPHYR_TOOLCHAIN_VARIANT"
echo "  CMAKE_PREFIX_PATH = $CMAKE_PREFIX_PATH"
