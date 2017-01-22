#!/usr/bin/env bash

# shellcheck source=agent.inc
. "$(dirname "${BASH_SOURCE[0]}")/agent.inc"

ENVLABEL=test

SHUNIT=$(which shunit)



function testCreateAgentCluster() {
  createAgentCluster "$ENVLABEL"
  assertEquals "createAgentCluster failed" 0 $?
}



# check if aws cli has been installed
# check if docker is running



function testDestroyAgentCluster() {
  destroyAgentCluster "$ENVLABEL"
  assertEquals "destroyAgentCluster failed" 0 $?
}



# function oneTimeSetup()
# function oneTimeTearDown()
# function setup()
# function teardown()



# shellcheck source=/usr/local/bin/shunit
. "$SHUNIT"
