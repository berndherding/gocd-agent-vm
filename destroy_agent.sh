#!/usr/bin/env bash

# shellcheck source=agent.inc
. "$(dirname "${BASH_SOURCE[0]}")/agent.inc"

STACKNAME=gocd-avm

_GO_PIPELINE_COUNTER=-${GO_PIPELINE_COUNTER:-0}

STACKNAME=$STACKNAME$_GO_PIPELINE_COUNTER

destroyAgentCluster "${1:-$STACKNAME}"
