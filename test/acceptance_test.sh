#!/usr/bin/env bash

# shellcheck source=../server.inc
. "$(dirname "${BASH_SOURCE[0]}")/../agent.inc"

AGENT_STACKNAME=gocd-avm

_GO_PIPELINE_COUNTER=-${GO_PIPELINE_COUNTER:-0}

AGENT_STACKNAME=$AGENT_STACKNAME$_GO_PIPELINE_COUNTER

SHUNIT=$(which shunit)



function testCreateAgentCluster() {
  createAgentCluster "$AGENT_STACKNAME"
  assertEquals "createAgentCluster failed" 0 $?
  # check if volumes are mounted
  # check if volumes have content
  # check if server-cert has been created
  # check if aws cli has been installed
  # check if docker is running
  # check if docker parameters are ok
}



function testDestroyAgentCluster() {
  destroyAgentCluster "$AGENT_STACKNAME"
  assertEquals "destroySVM failed" 0 $?
}



# function oneTimeSetup()
# function oneTimeTearDown()
# function setup()
# function teardown()



# shellcheck source=/usr/local/bin/shunit
. $SHUNIT
