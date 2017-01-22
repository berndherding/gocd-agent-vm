#!/usr/bin/env bash

# shellcheck source=agent.inc
. "$(dirname "${BASH_SOURCE[0]}")/agent.inc"

ENVLABEL=${1:-live}

createAgentCluster "$ENVLABEL"
