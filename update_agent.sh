#!/usr/bin/env bash

# shellcheck source=agent.inc
. "$(dirname "$BASH_SOURCE")/agent.inc"

ENV=${1:-live}

updateAgentCluster "$ENV" || exit $?
