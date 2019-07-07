#!/bin/bash
export TOOL_DIR=$(dirname $0)
source TOOL_DIR/env-setup.sh
$TOOL_DIR/mt76x7-flash-tool
