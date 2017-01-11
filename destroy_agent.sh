#!/usr/bin/env bash

# shellcheck source=agent.inc
. "$(dirname "${BASH_SOURCE[0]}")/agent.inc"

env=${1:-test}

gocd_avm="$(getStackname "gocd-avm" "$env")"

destroyAgentCluster "$gocd_avm"
