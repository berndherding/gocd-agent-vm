#!/usr/bin/env bash

# shellcheck source=agent.inc
. "$(dirname "${BASH_SOURCE[0]}")/agent.inc"

ENV=${1:-live}
INSTANCE_TYPE=${2:-t2.small}

createAgentCluster "$ENV" "$INSTANCE_TYPE" || exit $?
