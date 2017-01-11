#!/usr/bin/env bash

# shellcheck source=agent.inc
. "$(dirname "${BASH_SOURCE[0]}")/agent.inc"

env=test

AGENT_STACKNAME="$(getStackname "gocd-avm" "$env")"

SHUNIT=$(which shunit)



function testCreateAgentCluster() {
  createAgentCluster "$AGENT_STACKNAME"
  assertEquals "createAgentCluster failed" 0 $?
}



# check if aws cli has been installed
# check if docker is running



function testDestroyAgentCluster() {
  destroyAgentCluster "$AGENT_STACKNAME"
  assertEquals "destroyAgentCluster failed" 0 $?
}



# function oneTimeSetup()
# function oneTimeTearDown()
# function setup()
# function teardown()



# shellcheck source=/usr/local/bin/shunit
. "$SHUNIT"
