#!/usr/bin/env bash

# shellcheck source=agent.inc
. "$(dirname "${BASH_SOURCE[0]}")/agent.inc"

STACK_SUFFIX=${1:-test}

GOCD_AVM="$(getStackname "gocd-avm" "$STACK_SUFFIX")"

destroyAgentCluster "$GOCD_AVM"
